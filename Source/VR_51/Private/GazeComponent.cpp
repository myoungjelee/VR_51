// Fill out your copyright notice in the Description page of Project Settings.


#include "GazeComponent.h"
#include "VR_Player.h"
#include "Camera/CameraComponent.h"
#include "MyMovieActor.h"
#include "EngineUtils.h"


UGazeComponent::UGazeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UGazeComponent::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<AVR_Player>(GetOwner());

	for (TActorIterator<AMyMovieActor> mp(GetWorld()); mp; ++mp)
	{
		moviePlayer = *mp;
	}

}


void UGazeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ChangeSize(min, max);

	FVector startLoc = player->cam->GetComponentLocation();
	FVector endLoc = startLoc + player->cam->GetForwardVector() * 1000.0f;
	FHitResult hitInfo;
	FCollisionQueryParams params;
	params.AddIgnoredActor(player);

	bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startLoc, endLoc, ECC_GameTraceChannel2, params);
	increase = isHit;
	if (isHit && moviePlayer != nullptr && canChange)
	{
		canChange = !canChange;
		moviePlayer->ChangeMovie();
	}
}

void UGazeComponent::ChangeSize(FVector minSize, FVector maxSize)
{
	// 만일, increase 라면 

	// 현재 사이즈에서 maxSize로 증가시킨다.

	// 그렇지 않다면

	// 현재 사이즈에서 minSize로 감소시킨다.
	currentSize = increase ? currentSize + GetWorld()->GetDeltaSeconds() : currentSize -GetWorld()->GetDeltaSeconds();
	currentSize = FMath::Clamp(currentSize, 0.0f, 1.0f);

	FVector targetSize = FMath::Lerp(minSize, maxSize, currentSize);
	player->gazePointer->SetRelativeScale3D(targetSize);

}

