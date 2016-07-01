// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MahjongCharacter.h"
#include "MahjongAICharacter.generated.h"

UCLASS()
class AMahjongAICharacter : public AMahjongCharacter {

	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree* AIBehavior;

	virtual bool IsFirstPerson() const override;
	virtual void FaceRotation(FRotator NewRotation, float DeltaTime = 0.f) override;
};