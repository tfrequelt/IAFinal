// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPlayerLocation.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FindPlayerLocation::UBTTask_FindPlayerLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Find Player Location";
}

EBTNodeResult::Type UBTTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//get player character
	if(auto* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(),0))
	{
		//get player location
		auto const PlayerLocation = Player->GetActorLocation();
		if(SearchRandom)
		{
			FNavLocation Loc;
			//get navigation system and generate a random location near the player
			if(auto* const NavSys= UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				// try to get a random loc
				if(NavSys->GetRandomPointInNavigableRadius(PlayerLocation,SearchRadius,Loc))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector("TargetLocation",PlayerLocation);
					FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}
			}
		}else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector("TargetLocation",PlayerLocation);
			FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}
	}
			return EBTNodeResult::Failed;
}
