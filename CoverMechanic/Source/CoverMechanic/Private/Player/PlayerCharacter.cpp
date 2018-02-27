// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Public/Utility/Cover/CoverActor.h"


APlayerCharacter::APlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	//cover related properties
	bIsCovered = false;
	bIsInCoverVolume = false;
	bIsCoverPressed = false;

	WallLocation = FVector::ZeroVector;
	WallNormal = FVector::ZeroVector;

	SidewayTraceDistance = 45.0f;
	WallForwardTraceDistance = 70.0f;
	PlayerToWallDistance = 35.0f;

	//do not rotate when the controller rotates
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void APlayerCharacter::SetIsInCoverVolume(bool Value)
{
	bIsInCoverVolume = Value;
}

void APlayerCharacter::SetCurrentCoverMesh(class ACoverActor* CoverMesh)
{
	CurrentCover = CoverMesh;
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsInCoverVolume)
	{
		bIsCoverPressed = false;
	}
}

void APlayerCharacter::MoveForward(float Value)
{
	if (Controller && CurrentCover && bIsCovered)
	{
		if (Value < 0.0f)
		{
			LeaveCover();
		}
	}
	else if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Controller && CurrentCover && bIsCovered)
	{
		if (Value != 0.0f)
		{
			bool bCanMoveInCover = CanMoveInCover(Value);
			if (bCanMoveInCover)
			{
				FRotator RotationFromXZ = UKismetMathLibrary::MakeRotFromXZ(WallNormal * -1.0f, GetCapsuleComponent()->GetUpVector());
				FVector WallParallelDirection = UKismetMathLibrary::GetRightVector(RotationFromXZ);
				AddMovementInput(WallParallelDirection, Value);
			}
		}
	}
	else if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::TurnAtRate(float Value)
{
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Value)
{
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::CoverPressed()
{
	if (bIsInCoverVolume && !bIsCovered)
	{
		bIsCoverPressed = true;
		GetCover();
	}
	else if (bIsInCoverVolume && bIsCovered)
	{
		bIsCoverPressed = false;
		LeaveCover();
	}
}

void APlayerCharacter::GetCover()
{
	if (bIsCovered)
	{
		LeaveCover();
	}
	else
	{
		//forward trace
		FHitResult LineTraceHitResult = FHitResult(ForceInit);
		FCollisionQueryParams LineTraceQueryParams = FCollisionQueryParams(TEXT("LineTraceQueryParams"), true, this);
		FVector CoverMeshForwardDirection = CurrentCover->GetActorForwardVector();

		float DotProductFactor = FVector::DotProduct(GetActorForwardVector(), CoverMeshForwardDirection) > 0.0f ? 1.0f : -1.0f;

		const FVector LineTraceStart = GetActorLocation();
		const FVector LineTraceEnd = LineTraceStart + (DotProductFactor) * (CoverMeshForwardDirection * WallForwardTraceDistance);
		bool LineTraceOutput = GetWorld()->LineTraceSingleByChannel(LineTraceHitResult, LineTraceStart, LineTraceEnd, ECollisionChannel::ECC_GameTraceChannel1, LineTraceQueryParams);

		//debug line
		DrawDebugLine(GetWorld(), LineTraceStart, LineTraceEnd, FColor::Red, false, 5.0f, 0, 1.0f);

		if (LineTraceOutput)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Line Trace Output Successful"));

			WallLocation = LineTraceHitResult.Location;
			WallNormal = LineTraceHitResult.ImpactNormal;

			float XDistance = GetActorForwardVector().X * PlayerToWallDistance;
			float YDistance = GetActorForwardVector().Y * PlayerToWallDistance;

			//the target location
			FVector TargetLocation = FVector(WallLocation.X - XDistance, WallLocation.Y - YDistance, GetActorLocation().Z);

			//the target rotation
			FVector UpVector = GetCapsuleComponent()->GetUpVector();
			FRotator TargetRotation = UKismetMathLibrary::MakeRotFromXZ(WallNormal, UpVector);

			FLatentActionInfo LatentAction;
			LatentAction.CallbackTarget = this;

			//move
			UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), TargetLocation, TargetRotation, true, false, GetWorld()->GetDeltaSeconds() * 10.0f, false, EMoveComponentAction::Move, LatentAction);

			//set PlayerCharacter to be in cover
			bIsCovered = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}
}

void APlayerCharacter::LeaveCover()
{
	bIsCovered = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

bool APlayerCharacter::CanMoveInCover(float Value)
{
	//line trace parameters
	FHitResult LineTraceHitResult = FHitResult(ForceInit);
	FCollisionQueryParams LineTraceQueryParams = FCollisionQueryParams(TEXT("LineTraceQueryParams"), true, this);
	FVector CoverMeshForwardDirection = CurrentCover->GetActorForwardVector();
	float DotProductFactor = FVector::DotProduct(GetActorForwardVector(), CoverMeshForwardDirection) > 0.0f ? 1.0f : -1.0f;

	FVector LineTraceStart;
	FVector LineTraceEnd;

	FVector UnitRightVector = FVector::CrossProduct(GetActorForwardVector(), GetCapsuleComponent()->GetUpVector()) / (FVector::CrossProduct(GetActorForwardVector(), GetCapsuleComponent()->GetUpVector())).Size();

	//left pressed - move to right
	if (CurrentCover && Value < 0.0f)
	{
		bool bCanMoveLeft = false;
		LineTraceStart = GetActorLocation() + (-UnitRightVector * SidewayTraceDistance);
		LineTraceEnd = LineTraceStart + (-DotProductFactor * CoverMeshForwardDirection * WallForwardTraceDistance);

		//UE_LOG(LogTemp, Warning, TEXT("LineTraceStartL %s"), *LineTraceStart.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("LineTraceEndL %s"), *LineTraceEnd.ToString());

		bCanMoveLeft = GetWorld()->LineTraceSingleByChannel(LineTraceHitResult, LineTraceStart, LineTraceEnd, ECollisionChannel::ECC_GameTraceChannel1, LineTraceQueryParams);

		//debug line
		DrawDebugLine(GetWorld(), LineTraceStart, LineTraceEnd, FColor::Red, false, 5.0f, 0, 1.0f);

		if (bCanMoveLeft)
		{
			WallLocation = LineTraceHitResult.Location;
			WallNormal = LineTraceHitResult.Normal;
		}

		return bCanMoveLeft;
	}

	//right pressed - move to left
	else if (CurrentCover && Value > 0.0f)
	{
		bool bCanMoveRight = false;
		LineTraceStart = GetActorLocation() + (UnitRightVector * SidewayTraceDistance);
		LineTraceEnd = LineTraceStart + (-DotProductFactor * CoverMeshForwardDirection * WallForwardTraceDistance);

		//UE_LOG(LogTemp, Warning, TEXT("LineTraceStartR %s"), *LineTraceStart.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("LineTraceEndR %s"), *LineTraceEnd.ToString());

		bCanMoveRight = GetWorld()->LineTraceSingleByChannel(LineTraceHitResult, LineTraceStart, LineTraceEnd, ECollisionChannel::ECC_GameTraceChannel1, LineTraceQueryParams);

		//debug line
		DrawDebugLine(GetWorld(), LineTraceStart, LineTraceEnd, FColor::Red, false, 5.0f, 0, 1.0f);

		if (bCanMoveRight)
		{
			WallLocation = LineTraceHitResult.Location;
			WallNormal = LineTraceHitResult.Normal;
		}

		return bCanMoveRight;
	}
	else
	{
		return false;
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	//axis bindings
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("TakeCover", EInputEvent::IE_Pressed, this, &APlayerCharacter::CoverPressed);
}
