// Fill out your copyright notice in the Description page of Project Settings.


#include "GraspComponent.h"
#include "EnhancedInputComponent.h"
#include "VR_Player.h"
#include "VRHandAnimInstance.h"
#include "DrawDebugHelpers.h"
#include "PickUpActor.h"
#include "Components/TextRenderComponent.h"
#include "Components/SphereComponent.h"
#include "MotionControllerComponent.h"
#include "Components/BoxComponent.h"
#include "Haptics/HapticFeedbackEffect_Base.h"


// Sets default values for this component's properties
UGraspComponent::UGraspComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGraspComponent::BeginPlay()
{
	Super::BeginPlay();

	// �÷��̾� ĳ���Ϳ� ������ �ִϸ��̼� �ν��Ͻ��� ĳ���Ѵ�.
	player = Cast<AVR_Player>(GetOwner());
	rightHandAnim = Cast<UVRHandAnimInstance>(player->rightHand->GetAnimInstance());

	if (rightHandAnim != nullptr)
	{
		ResetRightFingers();
	}
}


// Called every frame
void UGraspComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UGraspComponent::SetupPlayerInputComponent(UEnhancedInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction(grip_right, ETriggerEvent::Started, this, &UGraspComponent::GripRightAction);
	PlayerInputComponent->BindAction(grip_right, ETriggerEvent::Completed, this, &UGraspComponent::GripRightRelease);
	PlayerInputComponent->BindAction(trigger_right, ETriggerEvent::Started, this, &UGraspComponent::TriggerRightAction);
	PlayerInputComponent->BindAction(trigger_right, ETriggerEvent::Completed, this, &UGraspComponent::TriggerRightAction);
	PlayerInputComponent->BindAction(trigger_right_touch, ETriggerEvent::Started, this, &UGraspComponent::TrigerRightTouch);
	PlayerInputComponent->BindAction(trigger_right_touch, ETriggerEvent::Completed, this, &UGraspComponent::TrigerRightTouchEnd);
	PlayerInputComponent->BindAction(thumb_right_touch, ETriggerEvent::Started, this, &UGraspComponent::ThumbRightTouch);
	PlayerInputComponent->BindAction(thumb_right_touch, ETriggerEvent::Completed, this, &UGraspComponent::ThumbRightTouchEnd);
	PlayerInputComponent->BindAction(btn_A, ETriggerEvent::Started, this, &UGraspComponent::ReadyToShoot);
	PlayerInputComponent->BindAction(btn_A, ETriggerEvent::Completed, this, &UGraspComponent::ReadyToShoot);

}

void UGraspComponent::GripRightAction(const FInputActionValue& value)
{
	rightHandAnim->PoseAlphaGrasp = value.Get<float>();
	// ���������� ��ü ���
	GrabObject(player->rightHand);

	// �����տ� ���� ȿ�� �ֱ�
	GetWorld()->GetFirstPlayerController()->PlayHapticEffect(grabHaptic, EControllerHand::Right, 1, false);
	
}

void UGraspComponent::GripRightRelease(const struct FInputActionValue& value)
{
	bIsGrab = false;
	//ReleaseObject(player->rightHand);
}

void UGraspComponent::TriggerRightAction(const FInputActionValue& value)
{
	rightHandAnim->PoseAlphaIndexCurl = value.Get<float>();
}

void UGraspComponent::TrigerRightTouch()
{
	rightHandAnim->PoseAlphaPoint = 0;
}

void UGraspComponent::TrigerRightTouchEnd()
{
	rightHandAnim->PoseAlphaPoint = 1;
}

void UGraspComponent::ThumbRightTouch()
{
	rightHandAnim->PoseAlphaThumbUp = 0;
}

void UGraspComponent::ThumbRightTouchEnd()
{
	rightHandAnim->PoseAlphaThumbUp = 1;
}

// �հ��� �ִϸ��̼� ������ �⺻ ������ �ǵ�����
void UGraspComponent::ResetRightFingers()
{
	rightHandAnim->PoseAlphaGrasp = 0;
	rightHandAnim->PoseAlphaIndexCurl = 0;
	rightHandAnim->PoseAlphaPoint = 1;
	rightHandAnim->PoseAlphaThumbUp = 1;
}

// ��ü�� ��� �Լ�
void UGraspComponent::GrabObject(USkeletalMeshComponent* selectHand)
{
	if (myGrabType == EGrabType::ELine)
	{
		// ��� 1 - LineTrace ���
		FVector startLoc = selectHand->GetComponentLocation();
		FVector endLoc = startLoc + selectHand->GetRightVector() * grabDistance;
		FHitResult hitInfo;

		if (GetWorld()->LineTraceSingleByProfile(hitInfo, startLoc, endLoc, TEXT("PickUp")) && grabedObject == nullptr)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Yellow, hitInfo.GetActor()->GetName());
			hitInfo.GetActor()->AttachToComponent(selectHand, FAttachmentTransformRules::KeepWorldTransform);
			grabedObject = Cast<APickUpActor>(hitInfo.GetActor());
		}
	}
	else if (myGrabType == EGrabType::ESweep)
	{
		// ��� 2 - SphereTrace ���
		FVector center = selectHand->GetComponentLocation();
		//FVector endLoc = center + selectHand->GetRightVector() * grabDistance;
		FHitResult hitInfo;
		FCollisionQueryParams params;
		params.AddIgnoredActor(player);

		if (GetWorld()->SweepSingleByChannel(hitInfo, center, center, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(grabDistance), params) && grabedObject == nullptr)
		{
			player->rightLog->SetText(FText::FromString(hitInfo.GetActor()->GetName()));
			grabedObject = Cast<APickUpActor>(hitInfo.GetActor());
			// ����, �˻��� ��ü�� physics�� simulate ���̶��...
			if (IsValid(grabedObject))
			//if (grabedObject != nullptr)
			{
				// ����� physics�� ���ش�.
				UBoxComponent* boxComp = Cast<UBoxComponent>(grabedObject->GetRootComponent());
				if (boxComp != nullptr)
				{
					physicsState = boxComp->IsSimulatingPhysics();
					boxComp->SetSimulatePhysics(false);
				}

				// ���� ���Ͽ� �˻��� PickUp ���͸� ���δ�.
				hitInfo.GetActor()->AttachToComponent(selectHand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GrabPoint"));
				grabedObject->SetActorRelativeLocation(grabedObject->gripOffset);
			}
		}
	}
	else if (myGrabType == EGrabType::EOverlap)
	{
		// ��� 3 - ������ ������� �˻��ϱ�
		FVector center = selectHand->GetComponentLocation();
		TArray<FOverlapResult> hitInfos;
		FCollisionObjectQueryParams objectQuerry;
		FCollisionQueryParams params;
		objectQuerry.AddObjectTypesToQuery(ECC_WorldDynamic);

		if (GetWorld()->OverlapMultiByObjectType(hitInfos, center, FQuat::Identity, objectQuerry, FCollisionShape::MakeSphere(grabDistance), params))
		{
			// �ε��� ��� ����� ��ȸ�Ѵ�.
			for (FOverlapResult& hitInfo : hitInfos)
			{
				// �˻� ��� �߿� PickUpActor Ŭ������ ��쿡�� �տ� ���δ�.
				APickUpActor* picks = Cast<APickUpActor>(hitInfo.GetActor());
				if (picks != nullptr)
				{
					hitInfo.GetActor()->AttachToComponent(selectHand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GrabPoint"));
				}
			}
		}
	}

	bIsGrab = true;
}

void UGraspComponent::ReleaseObject(USkeletalMeshComponent* selectHand, FVector torque)
{
	// ����, ��� �ִ� ��ü�� �ִٸ�...
	if (grabedObject != nullptr)
	{
		// ��� �ִ� ��ü�� �����.
		grabedObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// ��ü�� ���� ������ on/off ���θ� �ǵ����ش�.
		UBoxComponent* boxComp = Cast<UBoxComponent>(grabedObject->GetRootComponent());
		if (boxComp != nullptr)
		{
			boxComp->SetSimulatePhysics(physicsState);
		}

		throwDirection.Normalize();
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString::Printf(TEXT("%.3f, %.3f, %.3f"), throwDirection.X, throwDirection.Y, throwDirection.Z));
		UE_LOG(LogTemp, Log, TEXT("%.3f, %.3f, %.3f"), throwDirection.X, throwDirection.Y, throwDirection.Z);
		// ���� ������ ����� ���Ѵ�.
		boxComp->AddImpulse(throwDirection * throwPower);

		boxComp->AddTorqueInDegrees(torque * torquePower, NAME_None, true);

		// grabedObject ������ ������ nullptr�� �����Ѵ�.
		grabedObject = nullptr;
	}

}

void UGraspComponent::DrawGrabRange()
{
	if (myGrabType == EGrabType::ELine)
	{
		// �� �׸���(Line)
		FVector startLoc = player->rightHand->GetComponentLocation();
		FVector endLoc = startLoc + player->rightHand->GetRightVector() * grabDistance;
		DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Cyan, false, -1, 0, 2);
	}
	else if (myGrabType == EGrabType::ESweep || myGrabType == EGrabType::EOverlap)
	{
		FVector center = player->rightHand->GetComponentLocation();

		// �� �׸���(Sphere)
		DrawDebugSphere(GetWorld(), center, grabDistance, 30, FColor::Cyan, false, -1, 0, 1);
	}
}

void UGraspComponent::ReadyToShoot()
{
	if (grabedObject != nullptr)
	{
		if (!bIsReady)
		{
			//DrawGrabRange();
			prevLocation = player->rightController->GetComponentLocation();
			prevForward = player->rightController->GetForwardVector();
			UE_LOG(LogTemp, Log, TEXT("X Press!!!!!"));
		}
		else
		{
			// ���� ����
			throwDirection = player->rightController->GetComponentLocation() - prevLocation;

			// ȸ����
			FVector rotAxis = FVector::CrossProduct(prevForward, player->rightController->GetForwardVector());
			float angle = FMath::Acos(FVector::DotProduct(prevForward, player->rightController->GetForwardVector()));
			angle = FMath::RadiansToDegrees(angle);

			ReleaseObject(player->rightHand, rotAxis * angle);

			UE_LOG(LogTemp, Log, TEXT("X Release!!!!!"));
			DrawDebugLine(GetWorld(), player->rightController->GetComponentLocation(), player->rightController->GetComponentLocation() + throwDirection * 50, FColor::Red, false, 5, 0, 3);
		}
		bIsReady = !bIsReady;
	}
}
