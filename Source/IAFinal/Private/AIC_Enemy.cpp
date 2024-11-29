// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Enemy.h"

#include "AC_Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "IAFinal/IAFinalCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AI_Tags.h"
#include "AI_Keys.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"



AAIC_Enemy::AAIC_Enemy(FObjectInitializer const& ObjectInitializer)
{
	SetupPerceptionSystem();
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

void AAIC_Enemy::onUpdated(TArray<AActor*> const& updatedActors)
{
	for(size_t x = 0; x<updatedActors.Num(); ++x)
	{
		FActorPerceptionBlueprintInfo info;
		GetPerceptionComponent()->GetActorsPerception(updatedActors[x], info);
		for(size_t k = 0; k < info.LastSensedStimuli.Num();++k)
		{
			FAIStimulus const stim = info.LastSensedStimuli[k];
			if(stim.Tag == tags::noise_tag)
			{
				if(GetBlackboardComponent()->GetValueAsInt(keys::awake_state) > 2)
				{
					int awakeState = GetBlackboardComponent()->GetValueAsInt(keys::awake_state);

					UE_LOG(LogTemp, Error, TEXT("awake..%d."), awakeState);
					// GetBlackboardComponent()->SetValueAsInt(keys::awake_state,awakeState+1);
					UE_LOG(LogTemp, Error, TEXT("2 heard..%s."),GetBlackboardComponent()->GetValueAsBool(keys::heard_player) ? TEXT("true") : TEXT("false"));	

					GetBlackboardComponent()->SetValueAsBool(keys::is_awake, true);
					GetBlackboardComponent()->SetValueAsVector(keys::target_location,stim.StimulusLocation);

				}else
				{
					
					GetBlackboardComponent()->SetValueAsBool(keys::heard_player, true);
					int awakeState = GetBlackboardComponent()->GetValueAsInt(keys::awake_state);
					UE_LOG(LogTemp, Error, TEXT("Alert..%s."),GetBlackboardComponent()->GetValueAsBool(keys::heard_player) ? TEXT("true") : TEXT("false"));	
					GetBlackboardComponent()->SetValueAsInt(keys::awake_state,awakeState+1);

				}
			}
			
			// else
			// {
			// 	GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", stim.WasSuccessfullySensed()); 
			// }
		}
	}
}

void AAIC_Enemy::onAlert()
{
	
}

void AAIC_Enemy::SetupPerceptionSystem() {
	SightConfig =
		CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	HearingConfig =
		CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
 
	if(SightConfig != nullptr || HearingConfig != nullptr){
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(
			TEXT("Perception Component")));
	}
 
	if (SightConfig != nullptr) {
		SightConfig->SightRadius = 500.F;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.F;
		SightConfig->PeripheralVisionAngleDegrees = 90.F;
		SightConfig->SetMaxAge(5.F); // seconds - perceived stimulus forgotten after this time
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.F;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
 
		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this,&AAIC_Enemy::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense(*SightConfig);
	}
 
	if(HearingConfig != nullptr){
		HearingConfig->HearingRange =10000.F;
		HearingConfig->SetMaxAge(5.F);
		HearingConfig->DetectionByAffiliation.bDetectEnemies =true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies =true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals =true;
		GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this,&AAIC_Enemy::onUpdated);
		GetPerceptionComponent()->ConfigureSense(*HearingConfig);
	}
 
	// if(SightConfig != nullptr || HearingConfig != nullptr){
	// 	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(
	// 		this, &AAIC_Enemy::OnTargetDetected);
	// }
}
	

void AAIC_Enemy::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if(auto* const ch  = Cast<AIAFinalCharacter>(Actor))
	{
		if(Stimulus.Type == SightConfig->GetSenseID())
		{
			GetBlackboardComponent()->SetValueAsBool(keys::can_see_player,Stimulus.WasSuccessfullySensed());
		}else if(Stimulus.Type == HearingConfig->GetSenseID())
		{
			//GetBlackboardComponent()->SetValueAsBool(keys::heard_player,Stimulus.WasSuccessfullySensed());

		}
	}
}
