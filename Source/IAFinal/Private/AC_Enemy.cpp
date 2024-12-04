// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_Enemy.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

// Sets default values
AAC_Enemy::AAC_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
}

UBehaviorTree* AAC_Enemy::GetBehaviorTree() const
{
	if (!BehaviorTree)
	{
		UE_LOG(LogTemp, Error, TEXT("BehaviorTree is null in AAC_Enemy::GetBehaviorTree"));
	}
	return BehaviorTree;
}

// Called when the game starts or when spawned
void AAC_Enemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAC_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAC_Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAC_Enemy::SetupStimulusSource()
{
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	if(StimulusSource)
	{

		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimulusSource->RegisterWithPerceptionSystem();
	}
}

