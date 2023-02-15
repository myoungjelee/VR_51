// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GazeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VR_51_API UGazeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGazeComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly)
	FVector min = FVector(0.05f);

	UPROPERTY(EditDefaultsOnly)
	FVector max = FVector(0.5f);

	UPROPERTY(EditDefaultsOnly)
	class AMyMovieActor* moviePlayer;

	bool increase = false;
	

private:
	class AVR_Player* player;
	float currentSize;
	bool canChange = true;

	void ChangeSize(FVector minSize, FVector maxSize);
};
