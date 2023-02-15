// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"


AHoldActor::AHoldActor()
{
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(boxComp);
	boxComp->SetBoxExtent(FVector(50));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	meshComp->SetupAttachment(RootComponent);
}

void AHoldActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHoldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

