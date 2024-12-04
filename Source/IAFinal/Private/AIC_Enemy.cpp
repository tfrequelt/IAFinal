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

	if (AAC_Enemy* const enemy = Cast<AAC_Enemy>(InPawn))
	{
		if (UBehaviorTree* const tree = enemy->GetBehaviorTree())
		{
			UBlackboardComponent* b = nullptr;

			if (UseBlackboard(tree->BlackboardAsset, b))
			{
				Blackboard = b; // Assign the BlackboardComponent

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
	if (!GetPerceptionComponent()) {
		UE_LOG(LogTemp, Error, TEXT("PerceptionComponent is null in onUpdated"));
		return;
	}

	if (!GetBlackboardComponent()) {
		UE_LOG(LogTemp, Error, TEXT("BlackboardComponent is null in onUpdated"));
		return;
	}

	for (size_t x = 0; x < updatedActors.Num(); ++x) {
		FActorPerceptionBlueprintInfo info;
		GetPerceptionComponent()->GetActorsPerception(updatedActors[x], info);

		if (info.LastSensedStimuli.Num() == 0) {
			UE_LOG(LogTemp, Warning, TEXT("No stimuli detected for actor %s"), *updatedActors[x]->GetName());
			continue;
		}

		for (size_t k = 0; k < info.LastSensedStimuli.Num(); ++k) {
			const FAIStimulus& stim = info.LastSensedStimuli[k];

			if (stim.Tag == tags::noise_tag) {
				auto* BlackboardComponent = GetBlackboardComponent(); 

				int awakeState = BlackboardComponent->GetValueAsInt(keys::awake_state);
				if (awakeState > 2) {
					UE_LOG(LogTemp, Error, TEXT("Awake state: %d"), awakeState);
					BlackboardComponent->SetValueAsBool(keys::is_awake, true);
					BlackboardComponent->SetValueAsVector(keys::target_location, stim.StimulusLocation);
				} else {
					BlackboardComponent->SetValueAsBool(keys::heard_player, true);
					BlackboardComponent->SetValueAsInt(keys::awake_state, awakeState + 1);
					UE_LOG(LogTemp, Error, TEXT("Alert state updated to: %d"), awakeState + 1);
				}
			}
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
		SightConfig->SightRadius = 1000.F;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 100.F;
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
		//GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this,&AAIC_Enemy::OnTargetDetected);
		GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this,&AAIC_Enemy::onUpdated);
		GetPerceptionComponent()->ConfigureSense(*HearingConfig);
	}
 
	// if(SightConfig != nullptr || HearingConfig != nullptr){
	// 	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(
	// 		this, &AAIC_Enemy::OnTargetDetected);
	// }
}
	
//

void AAIC_Enemy::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if(auto* const ch  = Cast<AIAFinalCharacter>(Actor))
	{
		if(Stimulus.Type == SightConfig->GetSenseID())
		{
			UE_LOG(LogTemp, Error, TEXT("SIGHT TRIGGERED"));

			GetBlackboardComponent()->SetValueAsBool(keys::can_see_player,Stimulus.WasSuccessfullySensed());
		}else if(Stimulus.Type == HearingConfig->GetSenseID())
		{
			//GetBlackboardComponent()->SetValueAsBool(keys::heard_player,Stimulus.WasSuccessfullySensed());
			//onUpdated(Actor);
		}
	}
}
