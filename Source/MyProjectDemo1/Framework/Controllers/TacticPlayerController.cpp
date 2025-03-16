// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticPlayerController.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Characters/PlayerCharacter.h"
#include "MyProjectDemo1/Framework/GameStates/TacticGameState.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "MyProjectDemo1/Other/PathTracerComponent.h"

ATacticPlayerController::ATacticPlayerController()
{
	//NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	//SetupInputKeys();

	PathTracerComponent = CreateComponent<UPathTracerComponent>();
}

void ATacticPlayerController::BeginPlay()
{
	Super::BeginPlay();
	TacticGameState = Cast<ATacticGameState>(GetWorld()->GetGameState());
	TacticSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTacticSubsystem>();

	TacticSubsystem->OnSwitchCharacterAction.AddUObject(this, &ThisClass::SwitchCharacterAction);
	//UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	//if (Subsystem)Subsystem->AddMappingContext(DefaultMappingContext,0);
}


void ATacticPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	/*
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		EnhancedInputComponent->BindAction(LeftMouseButtonAction, ETriggerEvent::Triggered, this,
		                                   &AMainPlayerController::OnLeftMouseButtonDown);
		EnhancedInputComponent->BindAction(RightMouseButtonAction, ETriggerEvent::Triggered, this,
		                                   &AMainPlayerController::OnRightMouseButtonDown);
		                                   */
}

void ATacticPlayerController::PlayerInputMovement(float Value, EAxis::Type Axis)
{
	Super::PlayerInputMovement(Value, Axis);

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
	SetViewTarget(BaseCharacter);
}


void ATacticPlayerController::OnLeftMouseButtonDown()
{
	Super::OnLeftMouseButtonDown();
}


void ATacticPlayerController::OnRightMouseButtonDown()
{
}
