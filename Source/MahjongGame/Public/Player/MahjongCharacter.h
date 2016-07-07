// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongCharacter.generated.h"

UCLASS(Abstract)
class AMahjongCharacter : public ACharacter
{
    GENERATED_UCLASS_BODY()

public:

protected:

    /** Default Hand class */
//    UPROPERTY(EditDefaultsOnly, Category = Hand)
//    TSubclassOf<class UPlayerHand> DefaultHandClass;

    /** Hand instance */
//    UPROPERTY(Transient, Replicated)
//    class UPlayerHand* PlayerHand;

    /** Material instances for setting player wind (3rd person view - the wind symbol will be the face of the player) */
    UPROPERTY(Transient)
    TArray<UMaterialInstanceDynamic*> MeshMIDs;

private:

    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    USkeletalMeshComponent* Mesh1P;

public:

    /** Spawn hand, setup initial variables. */
    virtual void PostInitializeComponents() override;

    /** Update the character. */
    virtual void Tick(float DeltaSeconds) override;

    /** Clean up hand. */
    virtual void Destroyed() override;

    /** Update mesh for first person view. */
    virtual void PawnClientRestart() override;

    /** [server] Perform PlayerState related setup */
    virtual void PossessedBy(class AController* C) override;

    /** [client] Perform PlayerState related setup */
    virtual void OnRep_PlayerState() override;

    /** Add camera pitch to first person mesh. */
    void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation);


    /** Play anim montage */
    virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

    /** Stop playing montage */
    virtual void StopAnimMontage(class UAnimMontage* AnimMontage) override;

    /** Stop playing all montages */
    void StopAllAnimMontages();

    /** Kill this pawn */
    virtual void KilledBy(class APawn* EventInstigator);
    

    /** Setup pawn specific input handlers */
    virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;


    USkeletalMeshComponent* GetPawnMesh() const;

    USkeletalMeshComponent* GetSpecificPawnMesh(bool WantFirstPerson) const;

    UFUNCTION(BlueprintCallable, Category = Mesh)
    virtual bool IsFirstPerson() const;


    /** Called on the actor right before replication occurs */
    virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) override;

    /** Update the wind material of all player meshes. */
    void UpdateWindMaterialsAllMIDs();

protected:

    /** Handle mesh visibility and updates */
    void UpdatePawnMeshes();

    /** Update mesh wind material on specified material instance. */
    void UpdateWindMaterial(UMaterialInstanceDynamic* UseMID);


    /** [server] Spawns default hand */
    void SpawnDefaultHand();

    /** [server] Remove all tiles from hand and destroy them */
    void DestroyHand();

    FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
};