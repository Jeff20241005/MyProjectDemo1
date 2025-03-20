// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerController.h"
#include "MyProjectDemo1/Framework/GameModes/TacticGameMode.h"
#include "TacticPlayerController.generated.h"

class UTacticSubsystem;
class ATacticGameState;
class ABaseCharacter; // Forward declaration instead of including the header

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API ATacticPlayerController : public AMyPlayerController
{

public:
	virtual void SetViewTarget(class AActor* NewViewTarget,
	                           FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CurrentLerpValue = 0;
	
	FDelegateHandle TempSwitchCharacterActionDelegate;
	void SwitchCharacterAction(ABaseCharacter* BaseCharacter);
	
protected:
	virtual void ZoomCamera(float Value) override;
	virtual void PlayerInputMovement(float Value, EAxis::Type Axis) override;

	virtual void Destroyed() override;

	virtual void OnRightMouseButtonDown() override;
	
	ATacticPlayerController();
	
	virtual void OnLeftMouseButtonDown() override;
	
	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;


	UPROPERTY()
	ATacticGameState* TacticGameState;
	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;


	GENERATED_BODY()
};
