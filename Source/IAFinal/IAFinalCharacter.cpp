// Copyright Epic Games, Inc. All Rights Reserved.

#include "IAFinalCharacter.h"
#include "IAFinalProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "VectorUtil.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AIAFinalCharacter

AIAFinalCharacter::AIAFinalCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	CrouchEyeOffset = FVector(0.0f);
	CrouchSpeed = 12.f;

	bIsWalking = false;
	WalkSpeed = 600.f;
	SlowWalkSpeed = 400.f;

	bIsSprinting = false;
	SprintSpeed = 1200.f;

	// MaxEndurance = 100.f;
	// MinEndurance = 0.f;
	// CurrentEndurance = MaxEndurance;
}

void AIAFinalCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AIAFinalCharacter::Tick(float DeltaTime)
{
	// Call the base class  
	Super::Tick(DeltaTime);

	float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaTime);
	CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;

	// if(bIsSprinting)
	// {
	// 	CurrentEndurance = UE::Shader::Clamp(CurrentEndurance, MaxEndurance, MinEndurance);
	// }
}	

//////////////////////////////////////////////////////////////////////////// Input

void AIAFinalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIAFinalCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AIAFinalCharacter::Look);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AIAFinalCharacter::StartCrouching);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AIAFinalCharacter::StopCrouching);

		// Walking
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &AIAFinalCharacter::SlowWalk);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AIAFinalCharacter::StartSprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AIAFinalCharacter::StopSprinting);

		// Firing
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AIAFinalCharacter::Fire);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AIAFinalCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AIAFinalCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AIAFinalCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if(HalfHeightAdjust == 0.f)
	{
		return;
	}
	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z +=  StartBaseEyeHeight - BaseEyeHeight + HalfHeightAdjust;
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
}

void AIAFinalCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if(HalfHeightAdjust == 0.f)
	{
		return;
	}
	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z +=  StartBaseEyeHeight - BaseEyeHeight - HalfHeightAdjust;
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
}

void AIAFinalCharacter::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	if(FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->GetCameraView(DeltaTime, OutResult);
		OutResult.Location += CrouchEyeOffset;
	}
}

void AIAFinalCharacter::SlowWalk(const FInputActionValue& Value)
{
	if(bIsWalking)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = SlowWalkSpeed;
	}
	bIsWalking = !bIsWalking;
}

void AIAFinalCharacter::StartSprinting(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("StartSprinting"));
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	bIsSprinting = true;
}

void AIAFinalCharacter::StopSprinting(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("StopSprinting"));
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bIsSprinting = false;
}

// void AIAFinalCharacter::Sprint(const FInputActionValue& Value)
// {
// 	if(bIsSprinting)
// 	{
// 		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
// 	}
// 	else
// 	{
// 		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
// 	}
// 	bIsSprinting = !bIsSprinting;
// }

void AIAFinalCharacter::StartCrouching(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("StartCrouching"));
	Crouch();
}

void AIAFinalCharacter::StopCrouching(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("StopCrouching"));
	UnCrouch();
}

void AIAFinalCharacter::Fire()
{
	FVector Start, LineTraceEnd, ForwardVector;

	Start = GetActorLocation();

	ForwardVector = GetActorLocation();

	LineTraceEnd = Start + (ForwardVector * 10000);
	
	Raycast(Start,LineTraceEnd);
}

void AIAFinalCharacter::Raycast(FVector StartTrace, FVector EndTrace)
{
	FHitResult* HitResult = new FHitResult();
	FCollisionQueryParams* CQP = new FCollisionQueryParams();
	ECollisionChannel Channel = ECC_Visibility; // Or another channel of your choice
	if(GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace,Channel, *CQP))
	{
		DrawDebugLine(
		GetWorld(),
		StartTrace,
		EndTrace,
		FColor(255, 0, 0),
		false, 2, 0,
		2
		);
		AAIShootCharacter* aiChar = Cast<AAIShootCharacter>(HitResult->GetActor());
		if(aiChar != NULL)
		{
			aiChar->OnHit(20,this);
		}
		
		// Spawn Niagara Beam Effect
		if (LaserBeamEffect)
		{
			UNiagaraComponent* Beam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				LaserBeamEffect,
				StartTrace
			);

			if (Beam)
			{
				// Set the beam's start and end positions
				Beam->SetVectorParameter(FName("BeamStart"), StartTrace);
				Beam->SetVectorParameter(FName("BeamEnd"), HitResult->ImpactPoint);

				// Activation du système Niagara
				Beam->Activate();
			}
		}
	}
}