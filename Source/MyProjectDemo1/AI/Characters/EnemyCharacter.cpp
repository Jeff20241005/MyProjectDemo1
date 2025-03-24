// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "MyProjectDemo1/AI/AIControllers/EnemyAIController.h"
#include "MyProjectDemo1/Components/TeamComp.h"


void AEnemyCharacter::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
}

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AEnemyAIController::StaticClass();
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 设置为敌人团队
	TeamComp->SetTeam(ETeamType::ETT_Enemy);
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
