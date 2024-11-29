// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIC_Enemy.generated.h"

/**
 * 
 */
UCLASS()
class IAFINAL_API AAIC_Enemy : public AAIController
{
	GENERATED_BODY()
public:
	explicit AAIC_Enemy(FObjectInitializer const& ObjectInitializer);

protected:
	virtual void OnPossess(APawn* InPawn) override;
};
