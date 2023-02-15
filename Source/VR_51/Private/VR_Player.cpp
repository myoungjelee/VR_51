// Fill out your copyright notice in the Description page of Project Settings.


#include "VR_Player.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "MoveComponent.h"
#include "GraspComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "WidgetPointerComponent.h"
#include "GazeComponent.h"
#include "ClimbComponent.h"


// Sets default values
AVR_Player::AVR_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cam->SetupAttachment(RootComponent);

	gazePointer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gaze Pointer"));
	gazePointer->SetupAttachment(cam);
	gazePointer->SetRelativeLocation(FVector(300, 0, 0));

	headMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head"));
	headMesh->SetupAttachment(cam);
	headMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	leftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Left Controller"));
	leftController->SetupAttachment(RootComponent);
	// ��� �ҽ� ����
	leftController->MotionSource = "Left";

	leftHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHand"));
	leftHand->SetupAttachment(leftController);
	leftHand->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	leftHand->SetRelativeRotation(FRotator(-25.0f, 180.0f, 90.0f));

	leftLog = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Left Log Text"));
	leftLog->SetupAttachment(leftController);
	leftLog->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	leftLog->SetTextRenderColor(FColor::Yellow);
	leftLog->SetHorizontalAlignment(EHTA_Center);
	leftLog->SetVerticalAlignment(EVRTA_TextCenter);

	rightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Right Controller"));
	rightController->SetupAttachment(RootComponent);
	rightController->MotionSource = "Right";

	rightHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHand"));
	rightHand->SetupAttachment(rightController);
	rightHand->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	rightHand->SetRelativeRotation(FRotator(25.0f, 0.0f, 90.0f));

	widgetPointer_right = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("Widget Pointer"));
	widgetPointer_right->SetupAttachment(rightController);

	rightLog = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Right Log Text"));
	rightLog->SetupAttachment(rightController);
	rightLog->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	rightLog->SetTextRenderColor(FColor::Yellow);
	rightLog->SetHorizontalAlignment(EHTA_Center);
	rightLog->SetVerticalAlignment(EVRTA_TextCenter);

	bUseControllerRotationPitch = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// ���� ������Ʈ�� �߰�
	moveComp = CreateDefaultSubobject<UMoveComponent>(TEXT("Move Component"));
	graspComp = CreateDefaultSubobject<UGraspComponent>(TEXT("Grasp Component"));
	widgetPointerComp = CreateDefaultSubobject<UWidgetPointerComponent>(TEXT("Widget Pointer Component"));
	gazeComp = CreateDefaultSubobject<UGazeComponent>(TEXT("Gaze Component"));
	climbComp = CreateDefaultSubobject<UClimbComponent>(TEXT("Climb Component"));
}

// Called when the game starts or when spawned
void AVR_Player::BeginPlay()
{
	Super::BeginPlay();
	
	// ��� ����� ���� ��ġ�� �����Ѵ�.
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(trackOrigin.GetValue());

	// 1. �÷��̾� ��Ʈ�ѷ��� �����´�.
	APlayerController* playerCon = GetWorld()->GetFirstPlayerController();

	// 2. �÷��̾� ��Ʈ�ѷ����� EnhancedInputLocalPlayerSubsystem�� �����´�.
	UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerCon->GetLocalPlayer());

	// 3. ������ Subsystem�� IMC�� ����Ѵ�.(�켱���� 0��)
	subsys->AddMappingContext(myMapping, 0);
}

// Called every frame
void AVR_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVR_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (enhancedInputComponent != nullptr)
	{
		enhancedInputComponent->BindAction(leftInputs[0], ETriggerEvent::Triggered, this, &AVR_Player::OnTriggerLeft);
		enhancedInputComponent->BindAction(leftInputs[0], ETriggerEvent::Completed, this, &AVR_Player::OnTriggerLeft);
		enhancedInputComponent->BindAction(leftInputs[1], ETriggerEvent::Started, this, &AVR_Player::Recenter);

		moveComp->SetupPlayerInputComponent(enhancedInputComponent);
		graspComp->SetupPlayerInputComponent(enhancedInputComponent);
		widgetPointerComp->SetupPlayerInputComponent(enhancedInputComponent);
		climbComp->SetupPlayerInputComponent(enhancedInputComponent);
	}
}

void AVR_Player::OnTriggerLeft(const FInputActionValue& value)
{
	float val = value.Get<float>();

	// �޼� �α׿� ���� ����Ѵ�.
	FString msg = FString::Printf(TEXT("%f"), val);
	leftLog->SetText(FText::FromString(msg));
}

void AVR_Player::Recenter()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}


