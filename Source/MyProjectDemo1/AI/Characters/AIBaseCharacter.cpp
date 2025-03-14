// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBaseCharacter.h"

#include "MyProjectDemo1/Components/TeamComp.h"


AAIBaseCharacter::AAIBaseCharacter(): ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	TeamComp->SetTeam(ETeamType::ETT_Neutral);
}

void AAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AAIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAIBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
