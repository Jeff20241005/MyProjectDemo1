// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticPlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"

ATacticPlayerCharacter::ATacticPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	
	//WidgetComponent->SetVisibility(false);
	//PathTracer = CreateDefaultSubobject<UActorComponent_PathTracer>("PathTracer");

	//SetupCharacterDataFromBP();
}


void ATacticPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	TacticSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTacticSubsystem>();
}

void ATacticPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}