// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBaseCharacter.h"


AAIBaseCharacter::AAIBaseCharacter(): ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
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
