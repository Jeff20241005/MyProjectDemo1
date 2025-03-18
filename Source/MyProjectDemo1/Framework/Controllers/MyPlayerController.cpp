// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/Characters/PlayerCharacter.h"
#include "MyProjectDemo1/Components/InteractionComp.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Framework/GameModes/MyGameMode.h"

AMyPlayerController::AMyPlayerController()
{
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;

	// 确保Controller每帧都会Tick
	PrimaryActorTick.bCanEverTick = true;

}

void AMyPlayerController::PossesSpawnedSpectatorPawn()
{
	Possess(MySpectatorPawn);
	if (CurrentMouseClickPlayer)
	{
		SetViewTarget(CurrentMouseClickPlayer);
	}
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	MyGameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	// 创建并设置观察者Pawn
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	MySpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(ASpectatorPawn::StaticClass(),
	                                                         FVector(0, 0, 1000), FRotator::ZeroRotator, SpawnParams);

	if (MySpectatorPawn)
	{
		PossesSpawnedSpectatorPawn();

		// 设置观察者Pawn的移动速度
		if (UFloatingPawnMovement* Movement = Cast<UFloatingPawnMovement>(MySpectatorPawn->GetMovementComponent()))
		{
			Movement->MaxSpeed = 1000.0f;
			Movement->Acceleration = 2000.0f;
			Movement->Deceleration = 2000.0f;
		}
	}

	// 设置输入模式
	FInputModeGameAndUI InputMode;
	// 不锁定鼠标到视口
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true); // 捕获输入时隐藏鼠标
	SetInputMode(InputMode);

	// 设置摄像机初始视角
	if (APawn* DefaultPawn = GetPawn())
	{
		SetViewTarget(DefaultPawn);
	}

	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ThisClass::GetMouseLocation, 0.02f, true);
}


void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 绑定鼠标左键点击事件
	InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &AMyPlayerController::OnLeftMouseButtonDown);
	InputComponent->BindAction("RightMouseClick", IE_Pressed, this, &AMyPlayerController::OnRightMouseButtonDown);
	InputComponent->BindAction("TabClick", IE_Pressed, this, &AMyPlayerController::OnTabClick);

	// 绑定WASD移动
	InputComponent->BindAxis("MoveForward", this, &AMyPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMyPlayerController::MoveRight);

	// 添加鼠标滚轮绑定
	InputComponent->BindAxis("MouseWheel", this, &AMyPlayerController::ZoomCamera);
}

void AMyPlayerController::GetMouseLocation()
{
	PerformLineTrace(this, &ThisClass::MouseLocationTraceExecute);
}

void AMyPlayerController::OnLeftMouseButtonDown()
{
	PerformLineTrace(this, &ThisClass::LeftMouseLineTraceExecute);
}

void AMyPlayerController::LeftMouseLineTraceExecute(FHitResult HitResult)
{
	if (const AActor* ClickedActor = HitResult.GetActor())
	{
		if (UInteractionComp* InteractionComp = Cast<UInteractionComp>(
			ClickedActor->GetComponentByClass(UInteractionComp::StaticClass())))
		{
			InteractionComp->OnClickedActor();
			return;
		}
	}

	FVector AdjustedLocation = HitResult.Location; //+ FVector(0, 0, 20.0f);
	LastClickLocation = AdjustedLocation;


	if (CurrentMouseClickPlayer)
	{
		CurrentMouseClickPlayer->BaseCharacterAIMoveTo(AdjustedLocation);
		PossesSpawnedSpectatorPawn();
	}
}


void AMyPlayerController::OnRightMouseButtonDown()
{
}

void AMyPlayerController::OnTabClick()
{
}


void AMyPlayerController::EnsurePlayerControl()
{
	if (CurrentMouseClickPlayer && CurrentMouseClickPlayer->GetController() != this)
	{
		// 停止AI移动
		if (ABaseAIController* AIController = CurrentMouseClickPlayer->BaseAIController)
		{
			AIController->StopMovement();
			AIController->UnPossess();
		}
		// 切换到玩家控制
		Possess(CurrentMouseClickPlayer);
	}
}

void AMyPlayerController::PlayerInputMovement(float Value, EAxis::Type Axis)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (Cast<ASpectatorPawn>(ControlledPawn))
		{
			//FRotator ControlRotation = GetControlRotation();
			FRotator DirectionWithOutZ(0, GetControlRotation().Yaw, GetControlRotation().Roll);
			const FVector Direction = FRotationMatrix(DirectionWithOutZ).GetUnitAxis(Axis);
			// 应用移动
			ControlledPawn->AddMovementInput(Direction, Value);
			return;
		}

		//	if (MyGameMode->CurrentControlMode == EControlMode::FreeRoamMode)
		{
			// 创建移动旋转
			FRotator MovementRotation(0.0f, 0.0f, 0.0f);

			const FVector Direction = FRotationMatrix(MovementRotation).GetUnitAxis(Axis);
			// 应用移动
			ControlledPawn->AddMovementInput(Direction, Value);
		}
	}
}

void AMyPlayerController::MoveForward(float Value)
{
	if (Value == 0.0f)return;

	if (MyGameMode->IsTacticMode()) return;
	PlayerInputMovement(Value, EAxis::X);
	EnsurePlayerControl();
}

void AMyPlayerController::MoveRight(float Value)
{
	if (Value == 0.0f)return;

	PlayerInputMovement(Value, EAxis::Y);
	if (MyGameMode->IsTacticMode()) return;
	EnsurePlayerControl();
}

void AMyPlayerController::MouseLocationTraceExecute(FHitResult HitResult)
{
	MouseHoverdCursorOverLocation = HitResult.Location;
}

void AMyPlayerController::SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	if (CurrentSpringArmLength != 0 && CurrentMouseClickPlayer && CurrentMouseClickPlayer->SpringArmComponent)
	{
		CurrentMouseClickPlayer->SpringArmComponent->TargetArmLength = CurrentSpringArmLength;
	}

	// 添加平滑过渡
	TransitionParams.BlendTime = 0.5f; // 过渡时间
	TransitionParams.BlendFunction = VTBlend_EaseInOut;

	Super::SetViewTarget(NewViewTarget, TransitionParams);
}

void AMyPlayerController::ZoomCamera(float Value)
{
	if (Value == 0.0f) return;

	if (CurrentMouseClickPlayer && CurrentMouseClickPlayer->SpringArmComponent)
	{
		if (CurrentSpringArmLength == 0)
		{
			CurrentSpringArmLength = CurrentMouseClickPlayer->SpringArmComponent->TargetArmLength;
		}
		// 获取当前臂长并计算新的臂长
		CurrentSpringArmLength = FMath::Clamp(
			CurrentSpringArmLength - Value * CameraZoomSpeed,
			MinCameraDistance,
			MaxCameraDistance
		);
		// 设置新的臂长
		CurrentMouseClickPlayer->SpringArmComponent->TargetArmLength = CurrentSpringArmLength;
	}
}
