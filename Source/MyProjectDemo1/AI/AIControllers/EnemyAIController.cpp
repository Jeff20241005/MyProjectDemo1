// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "MyProjectDemo1/AI/Characters/EnemyCharacter.h"


void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(InPawn))
	{
		if (EnemyCharacter->GetBehaviorTree())
		{
			RunBehaviorTree(EnemyCharacter->GetBehaviorTree());
		}
	}
}

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
