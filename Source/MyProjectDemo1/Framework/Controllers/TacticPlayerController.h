// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "TacticPlayerController.generated.h"

class UTacticSubsystem;
class ABaseCharacter;
class UBaseAbility;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API ATacticPlayerController : public ABasePlayerController
{
	GENERATED_BODY()

public:
	virtual void SetViewTarget(class AActor* NewViewTarget,
	                           FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;
	

	// 暴露摄像机组件给蓝图
	UPROPERTY(BlueprintReadWrite, Category = "Camera")
	UCameraComponent* CameraComponent;
	
	// 设置相机为16:9比例
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bUse16By9Ratio = true;
	
	// 正交宽度设置(可在蓝图中调整)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta=(EditCondition="CameraComponent != nullptr"))
	float OrthoWidth = 2000.0f;

protected:
	bool IsWASDMoved;
	virtual void OnLeftMouseButtonDown() override;

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

	virtual void MoveRight(float Value) override;
	virtual void MoveForward(float Value) override;
	void ZoomCameraTick(float DeltaTime);
	virtual void ZoomCamera(float Value) override;
	virtual void PlayerInputMovement(float Value, EAxis::Type Axis) override;
	void ToggleAutoMove();
	virtual void SetupInputComponent() override;

	virtual void Destroyed() override;

	virtual void OnRightMouseButtonDown() override;

	ATacticPlayerController();

	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;

	FTimerHandle CameraTransitionTimerHandle;

	virtual void Tick(float DeltaSeconds) override;
	void CancelSkill();
	void SwitchToNextCharacterActionDelay();
	void SwitchToNextCharacterAction();
	void PreMove(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility);
	void SkillSelectedTimer(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility);
	void CancelMove();
	virtual void BeginPlay() override;

	void CameraSetting(ASpectatorPawn* SpecPawn);
	template <class Comp>
	Comp* CreateComponent();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
	UCameraComponent* TheCamera;
};

//记得包含头文件在C++文件
template <typename Comp>
Comp* ATacticPlayerController::CreateComponent()
{
	TArray<UActorComponent*> ExistingComponents;
	GetComponents(ExistingComponents);
	int32 CompCount = 0;
	for (UActorComponent* CompInstance : ExistingComponents)
	{
		if (CompInstance->IsA<Comp>())
		{
			CompCount++;
		}
	}

	FName CompName = FName(*(Comp::StaticClass()->GetName() + TEXT("_") + FString::FromInt(CompCount)));

	Comp* TheComp = CreateDefaultSubobject<Comp>(CompName);
	if (USceneComponent* TempSceneComponentTemp = Cast<USceneComponent>(TheComp))
	{
		if (TempSceneComponentTemp)
		{
			TempSceneComponentTemp->SetupAttachment(RootComponent);
		}
	}

	return TheComp;
}
