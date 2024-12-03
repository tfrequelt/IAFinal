// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
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
private:
	class UAISenseConfig_Hearing* HearingConfig;
	class UAISenseConfig_Sight* SightConfig;
	UFUNCTION()
	void onUpdated(TArray<AActor*> const& updatedActors);
	void onAlert();
	void SetupPerceptionSystem();
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
};
