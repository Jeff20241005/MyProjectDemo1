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
#include "CollisionQueryParams.h"


AMyPlayerController::AMyPlayerController()
{
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;

	// 确保Controller每帧都会Tick
	PrimaryActorTick.bCanEverTick = true;
}

ASpectatorPawn* AMyPlayerController::GetMySpectatorPawn()
{
	if (MySpectatorPawn) return MySpectatorPawn;

	// 创建并设置观察者Pawn
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	MySpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(ASpectatorPawn::StaticClass(),
	                                                         FVector(0, 0, 1000), FRotator::ZeroRotator, SpawnParams);
	if (MySpectatorPawn)
	{
		// 设置观察者Pawn的移动速度
		if (UFloatingPawnMovement* Movement = Cast<UFloatingPawnMovement>(MySpectatorPawn->GetMovementComponent()))
		{
			Movement->MaxSpeed = 1000.0f;
			Movement->Acceleration = 2000.0f;
			Movement->Deceleration = 2000.0f;
		}
	}

	return MySpectatorPawn;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 设置输入模式
	FInputModeGameAndUI InputMode;
	// 不锁定鼠标到视口
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true); // 捕获输入时隐藏鼠标
	SetInputMode(InputMode);

	// 设置摄像机初始视角
	/*
	if (APawn* DefaultPawn = GetPawn())
	{
		SetViewTarget(DefaultPawn);
	}
	*/

	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ThisClass::GetMouseLocation, 0.02f, true);

	DefaultObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn); // 检测Pawn
	DefaultObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic); // 检测静态物体
	DefaultObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // 检测动态物体
	DefaultObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody); // 检测物理物体

	GroundObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	CurrentObjectQueryParams = DefaultObjectQueryParams;
}


void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 绑定鼠标左键点击事件
	InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &AMyPlayerController::OnLeftMouseButtonDown);
	InputComponent->BindAction("RightMouseClick", IE_Pressed, this, &AMyPlayerController::OnRightMouseButtonDown);

	// 绑定WASD移动
	InputComponent->BindAxis("MoveForward", this, &AMyPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMyPlayerController::MoveRight);

	// 添加鼠标滚轮绑定
	InputComponent->BindAxis("MouseWheel", this, &AMyPlayerController::ZoomCamera);


	InputComponent->BindAction("TabClick", IE_Pressed, this, &AMyPlayerController::TabClick);
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
}


void AMyPlayerController::OnRightMouseButtonDown()
{
}

void AMyPlayerController::TabClick()
{
}


void AMyPlayerController::PlayerInputMovement(float Value, EAxis::Type Axis)
{
}

void AMyPlayerController::MoveForward(float Value)
{
	if (Value == 0.0f)return;
}

void AMyPlayerController::MoveRight(float Value)
{
	if (Value == 0.0f)return;
}

void AMyPlayerController::MouseLocationTraceExecute(FHitResult HitResult)
{
	MouseHoveringCursorOverLocation = HitResult.Location;
}

void AMyPlayerController::SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	// 添加平滑过渡
	TransitionParams.BlendTime = 0.5f; // 过渡时间
	TransitionParams.BlendFunction = VTBlend_EaseInOut;

	Super::SetViewTarget(NewViewTarget, TransitionParams);
}

void AMyPlayerController::ZoomCamera(float Value)
{
	if (Value == 0.0f) return;
}
