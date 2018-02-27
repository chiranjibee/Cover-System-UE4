// Fill out your copyright notice in the Description page of Project Settings.

#include "CoverActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Public/Player/PlayerCharacter.h"

ACoverActor::ACoverActor()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	CoverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoverMesh"));
	CoverMesh->AttachToComponent(SceneComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	CoverMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

	CoverVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("CoverVolume"));
	CoverVolume->AttachToComponent(CoverMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	CoverVolume->bGenerateOverlapEvents = true;
	CoverVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

void ACoverActor::BeginPlay()
{
	Super::BeginPlay();
	
	CoverVolume->OnComponentBeginOverlap.AddDynamic(this, &ACoverActor::CoverVolumeBeginOverlap);
	CoverVolume->OnComponentEndOverlap.AddDynamic(this, &ACoverActor::CoverVolumeEndOverlap);
}

void ACoverActor::CoverVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APlayerCharacter* tempPC = Cast<APlayerCharacter>(OtherActor);
	if (tempPC)
	{
		//in cover volume
		tempPC->SetIsInCoverVolume(true);
		tempPC->SetCurrentCoverMesh(this);
	}
}

void ACoverActor::CoverVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* tempPC = Cast<APlayerCharacter>(OtherActor);
	if (tempPC)
	{
		//not in cover volume
		tempPC->SetIsInCoverVolume(false);
		tempPC->SetCurrentCoverMesh(nullptr);
	}
}
