// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "AIC_Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTTask_StopInvestigating.generated.h"
/**
 * 
 */
UCLASS()
class IAFINAL_API UBTTask_StopInvestigating : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
