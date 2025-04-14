// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class UPathTracerComp;
class AMyGameMode;
class UInteractionComp;
class ABaseCharacter;
class ABaseAIController;
struct FPathFollowingResult;
struct FAIRequestID;
class AAIController;
class ATacticEnemyCharacter;
class ATacticPlayerCharacter;
/**
 * 
 */

UCLASS()
class MYPROJECTDEMO1_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MouseAction)
	FVector CustomHoveredLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MouseAction)
	AActor* HoveredActor;

	//点击的物品（操作角色靠近或者捡起等）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MouseAction)
	AActor* ClickedItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MouseAction)
	FVector DefaultHoveredLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MouseAction)
	ABaseCharacter* DefaultHoveredBaseCharacter;


	virtual void SetViewTarget(class AActor* NewViewTarget,
	                           FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;


	FCollisionObjectQueryParams CustomObjectQueryParams;
	FCollisionObjectQueryParams DefaultObjectQueryParams;
	FCollisionObjectQueryParams GroundObjectQueryParams;
	FCollisionObjectQueryParams GroundPlusBaseCharcterObjectQueryParams;

	void SetGroundObjectQueryParams();
	void SetDefaultObjectQueryParams();
	void SetGroundPlusBaseCharcterObjectQueryParams();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float TraceDistance = 100000;

	// 摄像机缩放相关
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinCameraDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxCameraDistance = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraZoomSpeed = 100.0f;


	// 处理鼠标左键点击
	virtual void OnLeftMouseButtonDown();

	virtual void OnRightMouseButtonDown();
	virtual void TabClick();
	// 绑定输入事件
	virtual void SetupInputComponent() override;

	void PerformTraceTimer();
	virtual void BeginPlay() override;


	ABasePlayerController();

	// WASD移动相关
	virtual void MoveForward(float Value);
	virtual void MoveRight(float Value);

	virtual void PlayerInputMovement(float Value, EAxis::Type Axis);

	virtual void ZoomCamera(float Value);

	UFUNCTION(BlueprintCallable)
	ASpectatorPawn* GetMySpectatorPawn();
	UPROPERTY()
	ASpectatorPawn* MySpectatorPawn;

	template <class TMemberFunc>
	void PerformLineTrace(TMemberFunc Function, FCollisionObjectQueryParams InCollisionObjectQueryParams);
};

template <class TLambdaFunc>
void ABasePlayerController::PerformLineTrace(TLambdaFunc Function,
                                             FCollisionObjectQueryParams InCollisionObjectQueryParams)
{
	float MouseX, MouseY;
	if (GetMousePosition(MouseX, MouseY))
	{
		FVector WorldLocation, WorldDirection;
		if (DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection))
		{
			FVector TraceStart = WorldLocation;
			FVector TraceEnd = WorldLocation + (WorldDirection * TraceDistance);

			// 配置射线检测参数
			FCollisionQueryParams QueryParams;
			// QueryParams.AddIgnoredActor(GetPawn());
			QueryParams.bTraceComplex = true; // 使用复杂碰撞检测

			// 执行射线检测
			FHitResult HitResult;
			bool bHit = GetWorld()->LineTraceSingleByObjectType(
				HitResult,
				TraceStart,
				TraceEnd,
				InCollisionObjectQueryParams,
				QueryParams
			);
			/*
						// 用于调试的射线可视化
						#if ENABLE_DRAW_DEBUG
							DrawDebugLine(
								GetWorld(),
								TraceStart,
								TraceEnd,
								FColor::Red,
								false,
								2.0f,
								0,
								1.0f
							);
						#endif
			*/
			if (bHit)
			{
				Function(HitResult);
			}
		}
	}
}
