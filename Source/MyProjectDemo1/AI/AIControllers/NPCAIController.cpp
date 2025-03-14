// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCAIController.h"


ANPCAIController::ANPCAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANPCAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANPCAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANPCAIController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

