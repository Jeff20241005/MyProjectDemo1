// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticNPCAIController.h"


ATacticNPCAIController::ATacticNPCAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATacticNPCAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATacticNPCAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATacticNPCAIController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

