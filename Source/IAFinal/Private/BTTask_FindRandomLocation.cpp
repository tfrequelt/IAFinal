// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindRandomLocation.h"

#include "AC_Enemy.h"
#include "AIC_Enemy.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Find Random Location";
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//get aicontroller and its npc
	if(auto const controller = Cast<AAIC_Enemy>(OwnerComp.GetAIOwner()))
	{
		if(auto* const npc = controller->GetPawn())
		{
			//obtain npc location to use as origin
			auto const origin = npc->GetActorLocation();
			if(auto* const navSys = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				FNavLocation navLoc;
				
				if(navSys->GetRandomPointInNavigableRadius(origin,SearchRadius,navLoc))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector("TargetLocation",navLoc.Location);
				}
				//success
				FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
	//return Super::ExecuteTask(OwnerComp, NodeMemory);
}
