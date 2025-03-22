// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCCharacter.h"

#include "MyProjectDemo1/AI/AIControllers/NPCAIController.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/Framework/GameStates/TacticGameState.h"


ANPCCharacter::ANPCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = ANPCAIController::StaticClass();
}


void ANPCCharacter::BeginPlay()
{
	Super::BeginPlay();
	TeamComp->SetTeam(ETeamType::ETT_Neutral);
}

void ANPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANPCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
