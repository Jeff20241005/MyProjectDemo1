// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticPlayerCharacter.h"

#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/Framework/Controllers/MyPlayerController.h"


void ATacticPlayerCharacter::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
}

ATacticPlayerCharacter::ATacticPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATacticPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 设置为玩家团队
	TeamComp->SetTeam(ETeamType::ETT_Player);
}

void ATacticPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATacticPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
