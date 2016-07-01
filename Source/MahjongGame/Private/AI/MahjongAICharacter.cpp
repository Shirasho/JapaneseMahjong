// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongAICharacter.h"

#include "MahjongAIController.h"

AMahjongAICharacter::AMahjongAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = AMahjongAIController::StaticClass();

	UpdatePawnMeshes();

	bUseControllerRotationYaw = true;
}

bool AMahjongAICharacter::IsFirstPerson() const
{
	return false;
}

void AMahjongAICharacter::FaceRotation(FRotator NewRotation, float DeltaTime)
{
	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 8.f);

	Super::FaceRotation(CurrentRotation, DeltaTime);
}