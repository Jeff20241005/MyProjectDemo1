// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerController.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"
#include "TacticPlayerController.generated.h"

class UTacticSubsystem;
class ABaseCharacter;

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API ATacticPlayerController : public AMyPlayerController
{
	GENERATED_BODY()

public:
	virtual void SetViewTarget(class AActor* NewViewTarget,
	                           FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;

protected:
	virtual void OnLeftMouseButtonDown() override;//todo skill release broadcast!
	
	// Camera zoom properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomFactor = 0.3f; // 0 = highest point, 1 = lowest point

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxCameraHeight = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinCameraHeight = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxCameraPitch = -45.0f; // Looking down at x degrees

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinCameraPitch = 0.0f; // Parallel to ground

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomInterpSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomSensitivity = 0.1f;


	// Camera  properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float RotationInterpSpeed = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float YawRotationAmount = 3.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=SetViewTarget)
	float SetViewTargetDuration = 0.5f;

	FRotator TargetCameraRotation;
	FRotator CurrentCameraRotation;
	float DesiredHeight;
	float DesiredPitch;
	float AlphaTimer = 0;


	// Camera rotation methods
	void RotateLeft(float Value);
	void RotateRight(float Value);

	void RefocusToCurrentActionCharacter();
	virtual void TabClick() override;

	// Method to disable vertical movement
	void DisableVerticalMovement(float Value);

	FDelegateHandle TempSwitchCharacterActionDelegate;


	virtual void MoveRight(float Value) override;
	virtual void MoveForward(float Value) override;
	void ZoomCameraTick(float DeltaTime);
	virtual void ZoomCamera(float Value) override;
	virtual void PlayerInputMovement(float Value, EAxis::Type Axis) override;
	virtual void SetupInputComponent() override;

	virtual void Destroyed() override;

	virtual void OnRightMouseButtonDown() override;

	ATacticPlayerController();

	virtual void OnLeftMouseButtonDown() override;


	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;

	FTimerHandle CameraTransitionTimerHandle;

	virtual void Tick(float DeltaSeconds) override;
	void CancelMoveAndSkill();
	void PostSkillSelected(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility);
	void SwitchToNextCharacterAction();
	virtual void BeginPlay() override;
};
