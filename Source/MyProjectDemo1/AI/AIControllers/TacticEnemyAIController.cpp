// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticEnemyAIController.h"

#include "MyProjectDemo1/AI/Characters/TacticEnemyCharacter.h"


void ATacticEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ATacticEnemyCharacter* EnemyCharacter = Cast<ATacticEnemyCharacter>(InPawn))
	{
		if (EnemyCharacter->GetBehaviorTree())
		{
			RunBehaviorTree(EnemyCharacter->GetBehaviorTree());
		}
	}
}

ATacticEnemyAIController::ATacticEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATacticEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ATacticEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
