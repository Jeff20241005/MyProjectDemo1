// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticEnemyCharacter.h"

#include "MyProjectDemo1/AI/AIControllers/TacticEnemyAIController.h"
#include "MyProjectDemo1/Components/TeamComp.h"


void ATacticEnemyCharacter::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
}

ATacticEnemyCharacter::ATacticEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = ATacticEnemyAIController::StaticClass();
}

void ATacticEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 设置为敌人团队
	TeamComp->SetTeam(ETeamType::ETT_Enemy);
}

void ATacticEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATacticEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
