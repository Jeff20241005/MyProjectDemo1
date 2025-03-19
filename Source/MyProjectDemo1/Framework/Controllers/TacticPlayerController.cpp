// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticPlayerController.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Characters/PlayerCharacter.h"
#include "MyProjectDemo1/Framework/GameModes/MyGameMode.h"
#include "MyProjectDemo1/Framework/GameStates/TacticGameState.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"

ATacticPlayerController::ATacticPlayerController()
{
	//NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	//SetupInputKeys();
}

void ATacticPlayerController::BeginPlay()
{
	Super::BeginPlay();
	TacticGameState = Cast<ATacticGameState>(GetWorld()->GetGameState());
	TacticSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTacticSubsystem>();

	TacticSubsystem->OnSwitchCharacterAction.AddUObject(this, &ThisClass::SwitchCharacterAction);
	//UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	//if (Subsystem)Subsystem->AddMappingContext(DefaultMappingContext,0);

	OnFreeViewport.AddUObject(this, &ThisClass::FreeViewportChange);
	OnCharacterFocus.AddUObject(this, &ThisClass::CharacterFocus);
}

void ATacticPlayerController::PlayerInputMovement(float Value, EAxis::Type Axis)

{
	Super::PlayerInputMovement(Value, Axis);
}


void ATacticPlayerController::FreeViewportChange()
{
	if (MyGameMode->IsTacticMode() && !bIsFreeViewport)
	{
		if (CurrentFocusCharacter && CurrentFocusCharacter->CameraComponent)
		{
			MySpectatorPawn->SetActorLocation(CurrentFocusCharacter->CameraComponent->GetComponentLocation());
			MySpectatorPawn->SetActorRotation(GetControlRotation());
			PossesSpawnedSpectatorPawn();
		}
		bIsFreeViewport = true;
	}
}

/*
void ATacticPlayerController::SetupInputKeys()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextObject(
		TEXT("/Game/Controller/MainInputMappingContext"));
	if (MappingContextObject.Object) DefaultMappingContext = MappingContextObject.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> LeftButtonActionObject(
		TEXT("/Game/Controller/LeftButtonInputAction"));
	if (LeftButtonActionObject.Object) LeftMouseButtonAction = LeftButtonActionObject.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> RightButtonActionObject(
		TEXT("/Game/Controller/RightButtonInputAction"));
	if (RightButtonActionObject.Object) RightMouseButtonAction = RightButtonActionObject.Object;
}
*/

void ATacticPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATacticPlayerController::SwitchCharacterAction(ABaseCharacter* BaseCharacter)
{
	CurrentFocusCharacter = BaseCharacter;
	SetViewTarget(BaseCharacter);
}

void ATacticPlayerController::CharacterFocus()
{
	bIsFreeViewport = false;
}

void ATacticPlayerController::OnTabClick()
{
	Super::OnTabClick();
	if (OnCharacterFocus.IsBound())
	{
		OnCharacterFocus.Broadcast();
	}
}


void ATacticPlayerController::OnLeftMouseButtonDown()
{
	Super::OnLeftMouseButtonDown();

	if (!CurrentFocusCharacter) return;
	if ( APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(CurrentFocusCharacter))
	{
		//todo		
	}
}


void ATacticPlayerController::OnRightMouseButtonDown()
{
	if (OnFreeViewport.IsBound())
	{
		OnFreeViewport.Broadcast();
	}
}
