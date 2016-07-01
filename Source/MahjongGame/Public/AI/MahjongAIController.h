// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "AIController.h"
#include "MahjongAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;


UCLASS(config = Game)
class AMahjongAIController : public AAIController {

	GENERATED_UCLASS_BODY()

private:

	UPROPERTY(Transient)
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(Transient)
	UBehaviorTreeComponent* BehaviorComponent;

public:

	// Begin AController interface
	virtual void GameHasEnded(class AActor* EndGameFocus = NULL, bool bIsWinner = false) override;
	virtual void Possess(class APawn* InPawn) override;
	virtual void UnPossess() override;
	virtual void BeginInactiveState() override;
	// End APlayerController interface

public:
	/** Returns BlackboardComp subobject **/
	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const
{ return BlackboardComponent; }
	/** Returns BehaviorComp subobject **/
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComponent() const
{ return BehaviorComponent; }
};