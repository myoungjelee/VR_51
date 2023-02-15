// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClimbComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VR_51_API UClimbComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UClimbComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetupPlayerInputComponent(class UEnhancedInputComponent* enhancedInputComponent);
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* grab_l;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* grab_r;

private:
	bool bClimbing_left = false;
	bool bClimbing_right = false;
	bool CanClimb = false;
	class AVR_Player* player;
	FVector climb_start;
	class UMotionControllerComponent* currentController;

	void StartClimb(bool isLeft);
	void EndClimb();
	void GrabLeft();
	void GrabRight();
	void ReleaseLeft();
	void ReleaseRight();
	bool CheckHolder(class UMotionControllerComponent* controller);
};
