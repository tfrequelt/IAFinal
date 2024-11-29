// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_Enemy.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

// Sets default values
AAC_Enemy::AAC_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetupStimulusSource();
}

UBehaviorTree* AAC_Enemy::GetBehaviorTree() const
{
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

