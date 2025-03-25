// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeRoamPlayerController.h"

#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Characters/PlayerCharacter.h"

void AFreeRoamPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// Find the player character
	if (!FreeRoamCurrentControlPlayer)
	{
		FreeRoamCurrentControlPlayer = Cast<APlayerCharacter>(
			UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));

		if (!FreeRoamCurrentControlPlayer)
		{
			UE_LOG(LogTemp, Error, TEXT("FreeRoamPlayerController: Could not find a PlayerCharacter in the world"));
		}
		else
		{
			// Set the view target to the found player character
			SetViewTarget(FreeRoamCurrentControlPlayer);
		}
	}
}

void AFreeRoamPlayerController::OnFreeModeLeftMouseButtonDown(FHitResult HitResult)
{
	if (FreeRoamCurrentControlPlayer)
	{
		// 暂时取消玩家控制
		UnPossess();
		FreeRoamCurrentControlPlayer->BaseAIController->Possess(FreeRoamCurrentControlPlayer);
		FreeRoamCurrentControlPlayer->BaseAIController->MoveToLocationWithPathFinding(LastClickLocation);
		PossesSpawnedSpectatorPawn();
	}
}

void AFreeRoamPlayerController::OnLeftMouseButtonDown()
{
	Super::OnLeftMouseButtonDown();
	PerformLineTrace(this, &AFreeRoamPlayerController::OnFreeModeLeftMouseButtonDown);
}

void AFreeRoamPlayerController::ZoomCamera(float Value)
{
	Super::ZoomCamera(Value);
	if (FreeRoamCurrentControlPlayer
		&& FreeRoamCurrentControlPlayer->SpringArmComponent)
	{
		if (CurrentSpringArmLength == 0)
		{
			CurrentSpringArmLength = FreeRoamCurrentControlPlayer->SpringArmComponent->TargetArmLength;
		}
		// 获取当前臂长并计算新的臂长
		CurrentSpringArmLength = FMath::Clamp(
			CurrentSpringArmLength - Value * CameraZoomSpeed,
			MinCameraDistance,
			MaxCameraDistance
		);
		// 设置新的臂长
		FreeRoamCurrentControlPlayer->SpringArmComponent->TargetArmLength = CurrentSpringArmLength;
	}
}

void AFreeRoamPlayerController::MoveForward(float Value)
{
	Super::MoveForward(Value);
	if (Value == 0.0f)return;
	PlayerInputMovement(Value, EAxis::X);
	EnsurePlayerControl();
}

void AFreeRoamPlayerController::MoveRight(float Value)
{
	Super::MoveRight(Value);
	if (Value == 0.0f)return;
	PlayerInputMovement(Value, EAxis::Y);
	EnsurePlayerControl();
}

void AFreeRoamPlayerController::EnsurePlayerControl()
{
	if (FreeRoamCurrentControlPlayer && FreeRoamCurrentControlPlayer->GetController() != this)
	{
		// 停止AI移动
		if (ABaseAIController* AIController = FreeRoamCurrentControlPlayer->BaseAIController)
		{
			AIController->StopMovement();
			AIController->UnPossess();
		}
		// 切换到玩家控制
		Possess(FreeRoamCurrentControlPlayer);
	}
}

void AFreeRoamPlayerController::PossesSpawnedSpectatorPawn()
{
	Possess(GetMySpectatorPawn());
	if (FreeRoamCurrentControlPlayer)
	{
		SetViewTarget(FreeRoamCurrentControlPlayer);
	}
}

void AFreeRoamPlayerController::PlayerInputMovement(float Value, EAxis::Type Axis)
{
	Super::PlayerInputMovement(Value, Axis);

	FRotator MovementRotation(0.0f, 0.0f, 0.0f);
	const FVector Direction = FRotationMatrix(MovementRotation).GetUnitAxis(Axis);
	GetPawn()->AddMovementInput(Direction, Value);
}
