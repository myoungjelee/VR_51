// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbComponent.h"
#include "VR_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MotionControllerComponent.h"
#include "EnhancedInputComponent.h"
#include "HoldActor.h"


UClimbComponent::UClimbComponent()
{
	PrimaryComponentTick.bCanEverTick = true;



}


void UClimbComponent::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<AVR_Player>(GetOwner());
}


void UClimbComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CanClimb)
	{
		return;
	}

	FVector delta;

	if (bClimbing_right || bClimbing_left)
	{
		// 최초 위치에서 손의 위치를 뺀 벡터를 구한다.
		delta = climb_start - currentController->GetComponentLocation();
	}

	// 앞에서 구한 벡터를 플레이어 액터의 위치에 더한다.
	player->AddActorWorldOffset(delta);

}

void UClimbComponent::SetupPlayerInputComponent(class UEnhancedInputComponent* enhancedInputComponent)
{
	enhancedInputComponent->BindAction(grab_r, ETriggerEvent::Started, this, &UClimbComponent::GrabRight);
	enhancedInputComponent->BindAction(grab_l, ETriggerEvent::Started, this, &UClimbComponent::GrabLeft);
	enhancedInputComponent->BindAction(grab_r, ETriggerEvent::Completed, this, &UClimbComponent::ReleaseRight);
	enhancedInputComponent->BindAction(grab_l, ETriggerEvent::Completed, this, &UClimbComponent::ReleaseLeft);
}

void UClimbComponent::StartClimb(bool isLeft)
{
	CanClimb = true;

	// 플레이어의 중력을 off한다.
	player->GetCharacterMovement()->GravityScale = 0;

	// 최초로 잡은 위치 벡터를 저장한다.
	if (isLeft)
	{
		climb_start = player->leftController->GetComponentLocation();
	}
	else
	{
		climb_start = player->rightController->GetComponentLocation();
	}

}

void UClimbComponent::EndClimb()
{
	// 양손이 모두 놓은 상태일 때에만 실행한다.
	if (!bClimbing_left && !bClimbing_right)
	{
		CanClimb = false;
		player->GetCharacterMovement()->GravityScale = 1.0f;
		bClimbing_right = false;
	}
}

void UClimbComponent::GrabLeft()
{
	currentController = player->leftController;

	// 홀더를 잡았다면...
	if (CheckHolder(currentController))
	{
		bClimbing_left = true;
		StartClimb(true);
	}
}

void UClimbComponent::GrabRight()
{
	currentController = player->rightController;

	if (CheckHolder(currentController))
	{
		bClimbing_right = true;
		StartClimb(false);
	}
}

void UClimbComponent::ReleaseLeft()
{
	bClimbing_left = false;
	EndClimb();
}

void UClimbComponent::ReleaseRight()
{
	bClimbing_right = false;
	EndClimb();
}

bool UClimbComponent::CheckHolder(class UMotionControllerComponent* controller)
{
	// 손 주변에 스피어 트레이스를 사용해서 닿은 오브젝트가 AHoldActor 클래스라면...

	FHitResult hitInfo;
	FVector startLoc = controller->GetComponentLocation();
	FCollisionShape cols = FCollisionShape::MakeSphere(50);
	FCollisionQueryParams params;
	params.AddIgnoredActor(player);

	FCollisionObjectQueryParams objectParams;
	objectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	bool result = false;

	if (GetWorld()->SweepSingleByObjectType(hitInfo, startLoc, startLoc, FQuat::Identity, objectParams, cols, params))
	{
		if (hitInfo.GetActor()->IsA(AHoldActor::StaticClass()))
		{
			result = true;
		}
	}

	return result;
}

