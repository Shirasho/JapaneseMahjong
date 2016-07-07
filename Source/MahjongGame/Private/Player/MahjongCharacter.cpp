// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongCharacter.h"

#include "MahjongPlayerState.h"
#include "MahjongGameMode.h"


AMahjongCharacter::AMahjongCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer) 
{
    Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMesh1P"));
    Mesh1P->SetupAttachment(GetCapsuleComponent());
    Mesh1P->bOnlyOwnerSee = true;
    Mesh1P->bOwnerNoSee = false;
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->bReceivesDecals = false;
    Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
    Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
    Mesh1P->SetCollisionObjectType(ECC_Pawn);
    Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);

    GetMesh()->bOnlyOwnerSee = false;
    GetMesh()->bOwnerNoSee = true;
    GetMesh()->bReceivesDecals = false;
    GetMesh()->SetCollisionObjectType(ECC_Pawn);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AMahjongCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (Role == ROLE_Authority)
    {
        SpawnDefaultHand();
    }

    // Set initial mesh visibility.
    UpdatePawnMeshes();

    // Create material instance depending on wind (3rd person)
    int32 MaterialCount = GetMesh()->GetNumMaterials();
    for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
    {
        MeshMIDs.Add(GetMesh()->CreateAndSetMaterialInstanceDynamic(MaterialIndex));
    }

    // Play spawn effects (if not server)
    if (GetNetMode() != NM_DedicatedServer)
    {
//        if (ParticleFX)
        {
//           /UGameplayStatics::SpawnEmitterAtLocation(this, ParticleFX, GetActorLocation, GetActorRotation());
        }
    }
}

void AMahjongCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
    check(InputComponent);
    /*InputComponent->BindAxis("Turn", this, &APawn::AMahjongCharacter);
    InputComponent->BindAxis("TurnRate", this, &AMahjongCharacter::TurnAtRate);
    InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    InputComponent->BindAxis("LookUpRate", this, &AMahjongCharacter::LookUpAtRate);

    InputComponent->BindAction("Select", IE_Pressed, this, &AMahjongCharacter::OnStartSelect);
    InputComponent->BindAction("Select", IE_Released, this, &AMahjongCharacter::OnStopSelect);*/
}

void AMahjongCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AMahjongCharacter::Destroyed()
{
    Super::Destroyed();
    DestroyHand();
}

void AMahjongCharacter::PawnClientRestart()
{
    Super::PawnClientRestart();

    UpdatePawnMeshes();

    // Set wind for first person view
    UMaterialInstanceDynamic* Mesh1PMID = Mesh1P->CreateAndSetMaterialInstanceDynamic(0);
    UpdateWindMaterial(Mesh1PMID);
}

void AMahjongCharacter::PossessedBy(class AController* InController)
{
    Super::PossessedBy(InController);

    // [server] As soon as PlayerState is assigned, set wind of this pawn for local player.
    UpdateWindMaterialsAllMIDs();
}

void AMahjongCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    if (PlayerState != nullptr)
    {
        // [server] As soon as PlayerState is assigned, set wind of this pawn for local player.
        UpdateWindMaterialsAllMIDs();
    }
}

void AMahjongCharacter::UpdatePawnMeshes()
{
    bool const bFirstPerson = IsFirstPerson();

    Mesh1P->MeshComponentUpdateFlag = !bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
    Mesh1P->SetOwnerNoSee(!bFirstPerson);

    GetMesh()->MeshComponentUpdateFlag = bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
    GetMesh()->SetOwnerNoSee(bFirstPerson);
}

void AMahjongCharacter::UpdateWindMaterial(UMaterialInstanceDynamic* UseMID)
{
    if (UseMID)
    {
        AMahjongPlayerState* MyPlayerState = Cast<AMahjongPlayerState>(PlayerState);
        if (MyPlayerState)
        {
            float MaterialParam = (float)MyPlayerState->GetPlayerWind();
            UseMID->SetScalarParameterValue(TEXT("Player Wind"), MaterialParam);
        }
    }
}

void AMahjongCharacter::UpdateWindMaterialsAllMIDs()
{
    int32 MeshMIDCount = MeshMIDs.Num();
    for (int32 MeshMIDIndex = 0; MeshMIDIndex < MeshMIDCount; ++MeshMIDIndex)
    {
        UpdateWindMaterial(MeshMIDs[MeshMIDIndex]);
    }
}

void AMahjongCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation)
{
    USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("PawnMesh1P")));
    const FMatrix DefMeshLS = FRotationTranslationMatrix(DefMesh1P->RelativeRotation, DefMesh1P->RelativeLocation);
    const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

    // Mesh rotating code expect uniform scale in LocalToWorld matrix

    const FRotator RotCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);
    const FRotator RotCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);

    const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RotCameraYaw, CameraLocation) * LocalToWorld.Inverse();
    const FMatrix PitchedCameraLS = FRotationMatrix(RotCameraPitch) * LeveledCameraLS;
    const FMatrix MeshRelativeToCamera = DefMeshLS * LeveledCameraLS.Inverse();
    const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

    Mesh1P->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(), PitchedMesh.Rotator());
}

void AMahjongCharacter::SpawnDefaultHand()
{
    if (Role < ROLE_Authority)
    {
        return;
    }

    //@TODO Create hand object here.
}

void AMahjongCharacter::DestroyHand()
{
    if (Role < ROLE_Authority)
    {
        return;
    }

    ////@TODO Destroy hand object here.
}

float AMahjongCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
    USkeletalMeshComponent* UseMesh = GetPawnMesh();
    if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance)
    {
        return UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
    }

    return 0.f;
}

void AMahjongCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
    USkeletalMeshComponent* UseMesh = GetPawnMesh();
    if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance &&
        UseMesh->AnimScriptInstance->Montage_IsPlaying(AnimMontage))
    {
        UseMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime());
    }
}

void AMahjongCharacter::StopAllAnimMontages()
{
    USkeletalMeshComponent* UseMesh = GetPawnMesh();
    if (UseMesh && UseMesh->AnimScriptInstance)
    {
        UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
    }
}

void AMahjongCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
    Super::PreReplication(ChangedPropertyTracker);
}

//@TODO This is only generated via GENERATED_BODY/GENERATED_UCLASS_BODY if at least 1 UPROPERTY is marked
// as Replicated. Right now the player hand is disabled, so no UPROPERTY values are marked.
//void AMahjongCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//    DOREPLIFETIME(AMahjongCharacter, PlayerHand);
//}

USkeletalMeshComponent* AMahjongCharacter::GetPawnMesh() const
{
    return IsFirstPerson() ? Mesh1P : GetMesh();
}

USkeletalMeshComponent* AMahjongCharacter::GetSpecificPawnMesh(bool WantFirstPerson) const
{
    return WantFirstPerson ? Mesh1P : GetMesh();
}

bool AMahjongCharacter::IsFirstPerson() const
{
    return Controller && Controller->IsLocalPlayerController();
}

void AMahjongCharacter::KilledBy(APawn* EventInstigator)
{
    if (Role == ROLE_Authority)
    {
        AController* Killer = nullptr;
        if (EventInstigator)
        {
            Killer = EventInstigator->Controller;
            LastHitBy = nullptr;
        }

        if (!IsPendingKill()
            && Role == ROLE_Authority
            && GetWorld()->GetAuthGameMode() != nullptr
            && GetWorld()->GetAuthGameMode()->GetMatchState() != MatchState::LeavingMap)
        {
            // If we want to inform the game mode about the destruction of this character
            // we can do it here.

            //GetWorld()->GetAuthGameMode<AMahjongGameMode>()->Killed(this);

            // Make it look like the item hasn't updated in a long time.
            NetUpdateFrequency = GetDefault<AMahjongCharacter>()->NetUpdateFrequency;

            UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
            if (MovementComponent)
            {
                // Force a replication update.
                MovementComponent->ForceReplicationUpdate();
            }
        }
    }
}