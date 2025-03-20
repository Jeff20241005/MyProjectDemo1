// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticPlayerController.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Characters/PlayerCharacter.h"
#include "MyProjectDemo1/Framework/GameModes/MyGameMode.h"
#include "MyProjectDemo1/Framework/GameStates/TacticGameState.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"

void ATacticPlayerController::ZoomCamera(float Value)
{
	Super::ZoomCamera(Value);

	//FRotator ControlRotation = GetControlRotation();
}

void ATacticPlayerController::SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	CurrentLerpValue;

	Super::SetViewTarget(NewViewTarget, TransitionParams);
}

ATacticPlayerController::ATacticPlayerController()
{
}

void ATacticPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TacticGameState = Cast<ATacticGameState>(GetWorld()->GetGameState());
	TacticSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTacticSubsystem>();

	TempSwitchCharacterActionDelegate = TacticSubsystem->OnSwitchCharacterAction.AddUObject(
		this, &ThisClass::SwitchCharacterAction);

	Possess(GetMySpectatorPawn());
}

void ATacticPlayerController::PlayerInputMovement(float Value, EAxis::Type Axis)

{
	Super::PlayerInputMovement(Value, Axis);

	FRotator DirectionWithOutZ(0, GetControlRotation().Yaw, GetControlRotation().Roll);
	const FVector Direction = FRotationMatrix(DirectionWithOutZ).GetUnitAxis(Axis);
	// 应用移动
	GetPawn()->AddMovementInput(Direction, Value);
}

void ATacticPlayerController::Destroyed()
{
	TacticSubsystem->OnSwitchCharacterAction.Remove(TempSwitchCharacterActionDelegate);
	
	Super::Destroyed();
}

void ATacticPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATacticPlayerController::SwitchCharacterAction(ABaseCharacter* BaseCharacter)
{
	SetViewTarget(BaseCharacter);
}

void ATacticPlayerController::OnLeftMouseButtonDown()
{
	Super::OnLeftMouseButtonDown();

	//todo TacticSubsystem->OnMove.Broadcast();
}


void ATacticPlayerController::OnRightMouseButtonDown()
{
}
