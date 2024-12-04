// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_StopInvestigating.h"

#include "AI_Keys.h"


EBTNodeResult::Type UBTTask_StopInvestigating::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto const cont = Cast<AAIC_Enemy>(OwnerComp.GetAIOwner());
	cont->GetBlackboardComponent()->SetValueAsBool(keys::awake_state,false);
	cont->GetBlackboardComponent()->SetValueAsBool(keys::heard_player,false);
	cont->GetBlackboardComponent()->SetValueAsInt(keys::awake_state,0);


	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
