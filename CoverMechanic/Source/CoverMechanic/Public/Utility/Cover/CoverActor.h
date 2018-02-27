// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoverActor.generated.h"

UCLASS()
class COVERMECHANIC_API ACoverActor : public AActor
{
	GENERATED_BODY()
	
public:

	ACoverActor();

	virtual void BeginPlay() override;

	UFUNCTION()
	void CoverVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void CoverVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE class UStaticMeshComponent* GetCoverMesh() const { return CoverMesh; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover System", meta=(AllowPrivateAccess="true"))
	class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover System", meta=(AllowPrivateAccess="true"))
	class UStaticMeshComponent* CoverMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover System", meta=(AllowPrivateAccess="true"))
	class UBoxComponent* CoverVolume;
};
