// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Enemy.h"

#include "AC_Enemy.h"

AAIC_Enemy::AAIC_Enemy(FObjectInitializer const& ObjectInitializer)
{
}

void AAIC_Enemy::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Check if InPawn is valid and can be cast to AAC_Enemy
	if (AAC_Enemy* const enemy = Cast<AAC_Enemy>(InPawn))
	{
		// Check if the behavior tree is valid
		if (UBehaviorTree* const tree = enemy->GetBehaviorTree())
		{
			UBlackboardComponent* b = nullptr;

			// Ensure UseBlackboard succeeds
			if (UseBlackboard(tree->BlackboardAsset, b))
			{
				Blackboard = b;  // Assign the BlackboardComponent
                
				// Attempt to run the behavior tree
				if (!RunBehaviorTree(tree))
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to run Behavior Tree in AAIC_Enemy::OnPossess"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to use Blackboard in AAIC_Enemy::OnPossess"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Behavior Tree is null in AAIC_Enemy::OnPossess"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InPawn is not of type AAC_Enemy in AAIC_Enemy::OnPossess"));
	}
}
