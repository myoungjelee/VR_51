// Fill out your copyright notice in the Description page of Project Settings.


#include "VR_Character.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "IKAnimInstance.h"


AVR_Character::AVR_Character()
{
	PrimaryActorTick.bCanEverTick = true;

	cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cam->SetupAttachment(RootComponent);

	leftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Left Controller"));
	leftController->SetupAttachment(RootComponent);
	// 모션 소스 선택
	leftController->MotionSource = "Left";

	rightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Right Controller"));
	rightController->SetupAttachment(RootComponent);
	rightController->MotionSource = "Right";

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AVR_Character::BeginPlay()
{
	Super::BeginPlay();
	
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

}

void AVR_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 모션 콘트롤러의 위치 값과 회전 값을 애니메이션 블루프린트 변수에 전달한다.
	UIKAnimInstance* myAnim = Cast<UIKAnimInstance>(GetMesh()->GetAnimInstance());
	
	if (myAnim != nullptr)
	{
		myAnim->controllerLocation_Left = leftController->GetComponentLocation();
		myAnim->controllerLocation_Right = rightController->GetComponentLocation();

		FRotator leftRot = leftController->GetComponentRotation();
		leftRot = FRotator(leftRot.Pitch *-1, leftRot.Yaw * -1, leftRot.Roll);

		FRotator rightRot = rightController->GetComponentRotation();
		rightRot = FRotator(rightRot.Pitch * -1, rightRot.Yaw, rightRot.Roll * -1);

		myAnim->controllerRotation_Left = leftRot;
		myAnim->controllerRotation_Right = rightRot;

		FRotator headRot = cam->GetComponentRotation();
		headRot = FRotator(headRot.Roll, headRot.Pitch * -1, headRot.Yaw * -1);

		myAnim->headLocation = cam->GetComponentLocation();
		myAnim->headRotation = headRot;
	}
}

void AVR_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

