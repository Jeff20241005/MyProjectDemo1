// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "FreeRoamPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API AFreeRoamPlayerController : public ABasePlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MouseAction)
	ATacticPlayerCharacter* FreeRoamCurrentControlPlayer;
	float CurrentSpringArmLength;

	void OnFreeModeLeftMouseButtonDown(FHitResult HitResult);
	virtual void OnLeftMouseButtonDown() override;

	virtual void ZoomCamera(float Value) override;
	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	void EnsurePlayerControl();
	
	UFUNCTION(BlueprintCallable)
	void PossesSpawnedSpectatorPawn();
	
	virtual void PlayerInputMovement(float Value, EAxis::Type Axis) override;


	virtual void BeginPlay() override;
};
