// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS(config=Game)
class COVERMECHANIC_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* FollowCamera;


public:

	APlayerCharacter();

	//BaseTurnRate in deg/sec
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	//BaseLookUpRate in deg/sec
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UFUNCTION(BlueprintCallable, Category="Cover System")
	void SetIsInCoverVolume(bool Value);

	UFUNCTION(BlueprintCallable, Category="Cover System")
	void SetCurrentCoverMesh(class ACoverActor* CoverMesh);

	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:

	UPROPERTY(VisibleAnywhere)
	uint8 bIsInCoverVolume : 1;

	UPROPERTY(VisibleAnywhere)
	uint8 bIsCoverPressed : 1;

	UPROPERTY(VisibleAnywhere)
	uint8 bIsCovered : 1;

	UPROPERTY(VisibleAnywhere)
	FVector WallNormal;

	UPROPERTY(VisibleAnywhere)
	FVector WallLocation;

	//offset distance from PlayerCharacter to left/right
	UPROPERTY(EditAnywhere)
	float SidewayTraceDistance;

	//offset distance from PlayerCharacter to wall
	UPROPERTY(EditAnywhere)
	float WallForwardTraceDistance;

	//distance between PlayerCharacter and wall when in cover
	UPROPERTY(EditAnywhere)
	float PlayerToWallDistance;

	UPROPERTY(VisibleAnywhere)
	class ACoverActor* CurrentCover;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);
	void CoverPressed();

	void GetCover();
	void LeaveCover();
	bool CanMoveInCover(float Value);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
