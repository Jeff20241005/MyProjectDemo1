// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/Characters/TacticPlayerCharacter.h"
#include "MyProjectDemo1/Components/InteractionComp.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Framework/GameModes/MyGameMode.h"
#include "CollisionQueryParams.h"


ABasePlayerController::ABasePlayerController()
{
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;

	// 确保Controller每帧都会Tick
	PrimaryActorTick.bCanEverTick = true;
}

ASpectatorPawn* ABasePlayerController::GetMySpectatorPawn()
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

void ABasePlayerController::BeginPlay()
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
	GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ThisClass::PerformTraceTimer, 0.02f, true);

	//注意，不能加Pawn，SpecPawn也是Pawn。。
	DefaultObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic); // 检测静态物体
	DefaultObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // 检测动态物体
	DefaultObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody); // 检测物理物体
	DefaultObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel1); // 检测地面（虚幻自带自定义
	DefaultObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel2); // 检测角色（虚幻自带自定义

	GroundObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	GroundPlusBaseCharcterObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
	GroundPlusBaseCharcterObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel2);

	SetDefaultObjectQueryParams();
}

void ABasePlayerController::SetGroundObjectQueryParams()
{
	CustomObjectQueryParams = GroundObjectQueryParams;
}

void ABasePlayerController::SetDefaultObjectQueryParams()
{
	CustomObjectQueryParams = DefaultObjectQueryParams;
}

void ABasePlayerController::SetGroundPlusBaseCharcterObjectQueryParams()
{
	CustomObjectQueryParams = GroundPlusBaseCharcterObjectQueryParams;
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 绑定鼠标左键点击事件
	InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &ABasePlayerController::OnLeftMouseButtonDown);
	InputComponent->BindAction("RightMouseClick", IE_Pressed, this, &ABasePlayerController::OnRightMouseButtonDown);

	// 绑定WASD移动
	InputComponent->BindAxis("MoveForward", this, &ABasePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABasePlayerController::MoveRight);

	// 添加鼠标滚轮绑定
	InputComponent->BindAxis("MouseWheel", this, &ABasePlayerController::ZoomCamera);


	InputComponent->BindAction("TabClick", IE_Pressed, this, &ABasePlayerController::TabClick);
}

void ABasePlayerController::PerformTraceTimer()
{
	PerformLineTrace([&](const FHitResult& HitResult)
	{
		CustomHoveredLocation = HitResult.Location;
	}, CustomObjectQueryParams);

	PerformLineTrace([&](const FHitResult& HitResult)
	{
		DefaultHoveredLocation = HitResult.Location;
		if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(HitResult.GetActor()))
		{
			DefaultHoveredBaseCharacter = BaseCharacter;
		}
		else
		{
			DefaultHoveredBaseCharacter = nullptr;
		}
	}, DefaultObjectQueryParams);
}

void ABasePlayerController::OnLeftMouseButtonDown()
{
	PerformLineTrace([&](const FHitResult& HitResult)
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
		//LastClickLocation = AdjustedLocation;
	}, CustomObjectQueryParams);
}


void ABasePlayerController::OnRightMouseButtonDown()
{
}

void ABasePlayerController::TabClick()
{
}


void ABasePlayerController::PlayerInputMovement(float Value, EAxis::Type Axis)
{
}

void ABasePlayerController::MoveForward(float Value)
{
	if (Value == 0.0f)return;
}

void ABasePlayerController::MoveRight(float Value)
{
	if (Value == 0.0f)return;
}

void ABasePlayerController::SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	// 添加平滑过渡
	TransitionParams.BlendTime = 0.5f; // 过渡时间
	TransitionParams.BlendFunction = VTBlend_EaseInOut;

	Super::SetViewTarget(NewViewTarget, TransitionParams);
}

void ABasePlayerController::ZoomCamera(float Value)
{
	if (Value == 0.0f) return;
}
