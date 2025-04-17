// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticPlayerController.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "MyProjectDemo1/Characters/TacticPlayerCharacter.h"
#include "MyProjectDemo1/Framework/HUD/BaseHUD.h"
#include "MyProjectDemo1/Framework/HUD/TacticHUD.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"

void ATacticPlayerController::MoveForward(float Value)
{
	Super::MoveForward(Value);

	if (Value == 0.0f)return;
	PlayerInputMovement(Value, EAxis::X);
}

void ATacticPlayerController::ZoomCameraTick(float DeltaTime)
{
	// Get current spectator pawn
	ASpectatorPawn* SpecPawn = Cast<ASpectatorPawn>(GetPawn());
	if (!SpecPawn) return;

	FVector CurrentLocation = SpecPawn->GetActorLocation();

	// Update target camera rotation
	TargetCameraRotation.Pitch = DesiredPitch;

	// Calculate forward offset for curved path
	// More offset as we zoom in (lower to the ground)

	//float ForwardOffset = ZoomFactor * 200.0f;
	// Adjust this value for more/less curve

	// Get forward vector (ignoring pitch) for offset calculation
	// Calculate target position with height and forward offset
	FVector GroundLocation = FVector(CurrentLocation.X, CurrentLocation.Y, 0);
	// FVector ForwardVector = FRotationMatrix(FRotator(0, TargetCameraRotation.Yaw, 0)).GetUnitAxis(EAxis::X);
	FVector TargetLocation = GroundLocation + FVector(0, 0, DesiredHeight); /* (ForwardVector * ForwardOffset);*/

	// Smoothly move to new position
	FVector NewLocation = FMath::VInterpTo(
		CurrentLocation,
		TargetLocation,
		DeltaTime,
		ZoomInterpSpeed
	);
	// Smoothly move to new position
	FVector NewLocationFinal = FMath::VInterpTo(
		NewLocation,
		TargetLocation,
		DeltaTime,
		ZoomInterpSpeed
	);

	// Set the new location
	SpecPawn->SetActorLocation(NewLocationFinal);
}

void ATacticPlayerController::ZoomCamera(float Value)
{
	// Ignore if no value
	if (Value == 0.0f)
	{
		return;
	}

	// Update zoom factor based on scroll direction
	ZoomFactor = FMath::Clamp(ZoomFactor - (Value * ZoomSensitivity), 0.0f, 1.0f);

	// Get current location and calculate desired height
	DesiredHeight = FMath::Lerp(MaxCameraHeight, MinCameraHeight, ZoomFactor);
	// Calculate the desired pitch (angle)
	DesiredPitch = FMath::Lerp(MaxCameraPitch, MinCameraPitch, ZoomFactor);
}

void ATacticPlayerController::RefocusToCurrentActionCharacter()
{
	if (TacticSubsystem && TacticSubsystem->CurrentActionBaseCharacter && IsWASDMoved)
	{
		SetViewTarget(TacticSubsystem->CurrentActionBaseCharacter);
		IsWASDMoved = false;
	}
}

void ATacticPlayerController::TabClick()
{
	Super::TabClick();
	RefocusToCurrentActionCharacter();
}

void ATacticPlayerController::SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	if (!NewViewTarget) return;

	ASpectatorPawn* MySpecPawn = GetMySpectatorPawn();
	if (!MySpecPawn) return;

	// 获取相机当前位置和旋转
	FVector CurrentLocation = MySpecPawn->GetActorLocation();
	FRotator CamRotation = GetControlRotation();

	// 1. 从目标位置出发
	FVector TargetLocation = NewViewTarget->GetActorLocation();
	FVector AdjustTargetLocation = FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z + MinCameraHeight);

	// 2. 获取相机朝向的反方向（相机看向的反方向）
	FVector CameraForward = CamRotation.Vector().GetSafeNormal(); // 前向向量
	FVector InverseCameraDirection = -CameraForward; // 相机反方向

	// 3. 沿着反方向射出一条射线
	const float AdjustDistance = MaxCameraHeight + MinCameraHeight + -DesiredPitch; //镜头越高，角度越大，距离越远。
	const FVector CameraPositionBeforeProjection = AdjustTargetLocation + InverseCameraDirection * AdjustDistance;

	// 清除现有过渡计时器
	GetWorldTimerManager().ClearTimer(CameraTransitionTimerHandle);
	AlphaTimer = 0;

	// 设置平滑过渡的计时器
	GetWorldTimerManager().SetTimer(
		CameraTransitionTimerHandle,
		[=, this]()
		{
			// 增加过渡值
			AlphaTimer += GetWorld()->GetDeltaSeconds() / SetViewTargetDuration;

			// 插值计算位置
			FVector TheFinalLocation = FMath::Lerp(
				CurrentLocation,
				CameraPositionBeforeProjection,
				AlphaTimer);
			// 检查是否完成过渡
			if (AlphaTimer >= 1.0f)
			{
				AlphaTimer = 1.0f;
				//MySpecPawn->SetActorLocation(CameraPositionBeforeProjection);
				GetWorldTimerManager().ClearTimer(CameraTransitionTimerHandle);
				return;
			}

			// 应用新位置
			MySpecPawn->SetActorLocation(TheFinalLocation);
		},
		0.016f, // ~60 fps
		true // 重复计时器
	);
}

ATacticPlayerController::ATacticPlayerController()
{
	TheCamera = CreateComponent<UCameraComponent>();
}

void ATacticPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Get current location and calculate desired height
	DesiredHeight = FMath::Lerp(MaxCameraHeight, MinCameraHeight, ZoomFactor);

	// Calculate the desired pitch (angle)
	DesiredPitch = FMath::Lerp(MaxCameraPitch, MinCameraPitch, ZoomFactor);

	TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>();

	TacticSubsystem->OnSwitchToNextCharacterAction.AddUObject(this, &ThisClass::SwitchToNextCharacterAction);

	// Create and possess the spectator pawn
	ASpectatorPawn* SpecPawn = GetMySpectatorPawn();
	Possess(SpecPawn);

	CameraSetting(SpecPawn);

	// Disable vertical movement in the pawn's movement component
	if (UPawnMovementComponent* MovementComp = SpecPawn->GetMovementComponent())
	{
		// Restrict movement to horizontal plane only
		MovementComp->SetPlaneConstraintEnabled(true);
		MovementComp->SetPlaneConstraintNormal(FVector(0, 0, 1)); // Z-axis normal
	}

	// Set initial camera height
	SpecPawn->SetActorLocation(FVector(
		SpecPawn->GetActorLocation().X,
		SpecPawn->GetActorLocation().Y,
		MaxCameraHeight
	));

	// Set initial camera rotation
	TargetCameraRotation = FRotator(MaxCameraPitch, TargetCameraRotation.Yaw, 0.0f);
	CurrentCameraRotation = TargetCameraRotation;
	SetControlRotation(TargetCameraRotation);

	// 绑定委托到子系统
	TacticSubsystem->OnPreMove.AddUObject(this, &ATacticPlayerController::PreMove);

	TacticSubsystem->OnCancelSkill.AddUObject(this, &ATacticPlayerController::CancelSkill);
	TacticSubsystem->OnCancelMove.AddUObject(this, &ATacticPlayerController::CancelMove);
	TacticSubsystem->OnSkillSelectedTimer.AddUObject(this, &ATacticPlayerController::SkillSelectedTimer);
}

void ATacticPlayerController::PlayerInputMovement(float Value, EAxis::Type Axis)
{
	Super::PlayerInputMovement(Value, Axis);

	IsWASDMoved = true;

	// Use a rotator that only includes yaw for movement direction
	FRotator DirectionWithOutZ(0, GetControlRotation().Yaw, 0);
	const FVector Direction = FRotationMatrix(DirectionWithOutZ).GetUnitAxis(Axis);
	GetPawn()->AddMovementInput(Direction, Value);
}

void ATacticPlayerController::ToggleAutoMove()
{
	ATacticHUD* TacticHUD = Cast<ATacticHUD>(GetHUD());
	if (TacticHUD)
	{
		TacticHUD->ToggleAutomaticMoveBySkill();
	}
}

void ATacticPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Q and E key bindings for rotation
	InputComponent->BindAxis("RotateLeft", this, &ATacticPlayerController::RotateLeft);
	InputComponent->BindAxis("RotateRight", this, &ATacticPlayerController::RotateRight);

	// Override the vertical movement keys with empty functions to disable them
	InputComponent->BindAxis("Space", this, &ATacticPlayerController::DisableVerticalMovement);
	InputComponent->BindAxis("Ctrl", this, &ATacticPlayerController::DisableVerticalMovement);

	//InputComponent->BindAction("AlphabetZ", IE_Pressed, this, &ThisClass::ToggleAutoMove);
}

void ATacticPlayerController::RotateLeft(float Value)
{
	if (Value == 0.0f) return;

	// Update the target rotation (only adjust yaw)
	TargetCameraRotation.Yaw -= YawRotationAmount;

	// Keep yaw in 0-360 range
	if (TargetCameraRotation.Yaw < 0)
	{
		TargetCameraRotation.Yaw += 360.0f;
	}
}

void ATacticPlayerController::RotateRight(float Value)
{
	if (Value == 0.0f) return;

	// Update the target rotation (only adjust yaw)
	TargetCameraRotation.Yaw += YawRotationAmount;

	// Keep yaw in 0-360 range
	if (TargetCameraRotation.Yaw >= 360.0f)
	{
		TargetCameraRotation.Yaw -= 360.0f;
	}
}

void ATacticPlayerController::Destroyed()
{
	Super::Destroyed();
}

void ATacticPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Smoothly interpolate camera rotation
	CurrentCameraRotation = FMath::RInterpTo(
		CurrentCameraRotation,
		TargetCameraRotation,
		DeltaSeconds,
		RotationInterpSpeed
	);

	// Apply the interpolated rotation
	SetControlRotation(FRotator(CurrentCameraRotation.Pitch, TargetCameraRotation.Yaw, CurrentCameraRotation.Roll));

	ZoomCameraTick(DeltaSeconds);
}

void ATacticPlayerController::CancelSkill()
{
	SetDefaultObjectQueryParams();
}

void ATacticPlayerController::SwitchToNextCharacterActionDelay()
{
	if (TacticSubsystem->CurrentActionBaseCharacter)
	{
		SetViewTarget(TacticSubsystem->CurrentActionBaseCharacter);
	}
}

void ATacticPlayerController::SwitchToNextCharacterAction()
{
	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ThisClass::SwitchToNextCharacterActionDelay, 0.2f);
}

void ATacticPlayerController::PreMove(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility)
{
	SetGroundObjectQueryParams();
}

void ATacticPlayerController::SkillSelectedTimer(ATacticPlayerController* TacticPlayerController,
                                                 UBaseAbility* BaseAbility)
{
	SetGroundObjectQueryParams();
}

void ATacticPlayerController::CancelMove()
{
	//	CurrentObjectQueryParams = GroundObjectQueryParams;
}


void ATacticPlayerController::MoveRight(float Value)
{
	Super::MoveRight(Value);
	if (Value == 0.0f)return;
	PlayerInputMovement(Value, EAxis::Y);
}

void ATacticPlayerController::OnLeftMouseButtonDown()
{
	TacticSubsystem->TryReleaseSkillOrMove(this);
	Super::OnLeftMouseButtonDown();
}


void ATacticPlayerController::OnRightMouseButtonDown()
{
	// Intentionally empty to disable right-click rotation/movement

	//Call CancelSkillAndMovement
	TacticSubsystem->CancelMoveAndSkill();
}

// Empty function to disable vertical movement
void ATacticPlayerController::DisableVerticalMovement(float Value)
{
	// Intentionally empty to prevent movement
}

void ATacticPlayerController::CameraSetting(ASpectatorPawn* SpecPawn)
{
	if (!SpecPawn) return;

	if (TheCamera)
	{
		// 设置为正交投影
		TheCamera->SetProjectionMode(ECameraProjectionMode::Orthographic);
		TheCamera->OrthoWidth = OrthoWidth;

		// 设置16:9比例
		if (bUse16By9Ratio)
		{
			TheCamera->bConstrainAspectRatio = true;
			TheCamera->AspectRatio = 16.0f / 9.0f;
		}
		else
		{
			TheCamera->bConstrainAspectRatio = false;
		}
	}
}
