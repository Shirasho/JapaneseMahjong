// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MahjongAICharacter.h"
#include "MahjongAIController.h"


AMahjongAIController::AMahjongAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Specify that this AIController wants its own PlayerState.
	bWantsPlayerState = true;
}

void AMahjongAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	AMahjongAICharacter* AICharacter = Cast<AMahjongAICharacter>(InPawn);

	if (AICharacter && AICharacter->AIBehavior)
	{
		if (AICharacter->AIBehavior->BlackboardAsset)
		{
			BlackboardComponent->InitializeBlackboard(*AICharacter->AIBehavior->BlackboardAsset);
		}

		if (BehaviorComponent)
		{
			BehaviorComponent->StartTree(*(AICharacter->AIBehavior));
		}
	}
}

void AMahjongAIController::UnPossess()
{
	Super::UnPossess();

	if (BehaviorComponent)
	{
		BehaviorComponent->StopTree();
	}
}

void AMahjongAIController::BeginInactiveState()
{
	Super::BeginInactiveState();

	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}

void AMahjongAIController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	if (BehaviorComponent)
	{
		BehaviorComponent->StopTree();
	}
}