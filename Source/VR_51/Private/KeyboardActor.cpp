// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyboardActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"


AKeyboardActor::AKeyboardActor()
{
	PrimaryActorTick.bCanEverTick = false;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	SetRootComponent(boxComp);
	boxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	boxComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	boxComp->SetGenerateOverlapEvents(true);
	boxComp->SetBoxExtent(FVector(50));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AKeyboardActor::BeginPlay()
{
	Super::BeginPlay();
	
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AKeyboardActor::OnPlayKey);

	// ���̳��� ��Ƽ���� �ν��Ͻ��� �����Ѵ�.
	keyMat = UMaterialInstanceDynamic::Create(meshComp->GetMaterial(0), this);
	meshComp->SetMaterial(0, keyMat);
}

void AKeyboardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKeyboardActor::OnPlayKey(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	// �Ҵ�� Ű ���带 ����Ѵ�.
	UGameplayStatics::PlaySoundAtLocation(this, keySound, GetActorLocation());

	// Ű�� ������ ������� ������ �Ѵ�.
	ChangeKeyColor(keyColor, glowPower);
	GetWorldTimerManager().ClearTimer(colorHandle);
	GetWorldTimerManager().SetTimer(colorHandle, this, &AKeyboardActor::ColorOff, 0.5f, false);
}

void AKeyboardActor::ColorOff()
{
	ChangeKeyColor(FVector4(1, 1, 1, 1), 0);
}

void AKeyboardActor::ChangeKeyColor(FVector4 color, float glow)
{
	keyMat->SetVectorParameterValue(TEXT("EmissiveColor"), color);
	keyMat->SetScalarParameterValue(TEXT("Glow"), glow);
}

