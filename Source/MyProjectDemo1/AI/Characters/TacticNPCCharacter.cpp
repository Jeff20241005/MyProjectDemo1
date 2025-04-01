// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticNPCCharacter.h"

#include "MyProjectDemo1/AI/AIControllers/TacticNPCAIController.h"
#include "MyProjectDemo1/Components/TeamComp.h"


ATacticNPCCharacter::ATacticNPCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = ATacticNPCAIController::StaticClass();
}


void ATacticNPCCharacter::BeginPlay()
{
	Super::BeginPlay();
	TeamComp->SetTeam(ETeamType::ETT_Neutral);
}

void ATacticNPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATacticNPCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
