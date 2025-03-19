// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticSubsystem.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyProjectDemo1/Actors/ShowVisualFeedbackActor.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "MyProjectDemo1/Components/PathTracerComponent.h"
#include "MyProjectDemo1/FilePaths/TacticPaths.h"
#include "MyProjectDemo1/Framework/Controllers/TacticPlayerController.h"
#include "MyProjectDemo1/Framework/GameStates/MyGameState.h"
#include "MyProjectDemo1/Framework/GameStates/TacticGameState.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"

void UTacticSubsystem::TestFunc_SwitchCharacter_RanOutOfAction()
{
	//todo test
	auto test = GetTacticGameState()->SortCharactersByActionValues();
	if (OnSwitchCharacterAction.IsBound() && test)
	{
		OnSwitchCharacterAction.Broadcast(test);
	}

	//直接设置ActionValue，代表执行完毕Action。
	test->GetBaseCharacterAttributeSet()->SetActionValues(0);
}

void UTacticSubsystem::PreSkillSelection(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility)
{
	//todo !!!
	/*if (BaseCharacter && BaseAbility)
	{
		TArray<ABaseCharacter*> PotentialTargets = GetTacticGameState()->GetTargetCharacters(
		
		);
	}*/
}

void UTacticSubsystem::Move(ABaseCharacter* BaseCharacter)
{
	if (bCanMove)
	{
/*		if (MyGameMode->())
		{
			// 暂时取消玩家控制
			CurrentActionCharacter->BaseAIController->Possess(CurrentActionCharacter);
			
			CurrentActionCharacter->BaseCharacterAIMoveTo(AdjustedLocation);
			PossesSpawnedSpectatorPawn();
		}*/
	}
	
	GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);
	bCanMove=false;
}

void UTacticSubsystem::CancelMove(ABaseCharacter* BaseCharacter)
{
	
	
	GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);
	bCanMove=false;
}

ATacticGameState* UTacticSubsystem::GetTacticGameState()
{
	return Cast<ATacticGameState>(UGameplayStatics::GetGameState(GetWorld()));
}


void UTacticSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnSwitchCharacterAction.AddUObject(this, &ThisClass::SwitchCharacterAction);
	OnRoundFinish.AddUObject(this, &ThisClass::RoundFinish);
	OnPostSkillSelected.AddUObject(this, &ThisClass::SelectedSkill);
	OnPreSkillSelection.AddUObject(this, &ThisClass::PreSkillSelection);

	OnPreMove.AddUObject(this, &ThisClass::CharacterPreMove);
	OnMove.AddUObject(this, &ThisClass::Move);
	OnCancelMove.AddUObject(this, &ThisClass::CancelMove);
}

void UTacticSubsystem::Deinitialize()
{
	// Clean up path visualization components
	Super::Deinitialize();
}

void UTacticSubsystem::PreMoveBroadCast()
{
	OnPreMove.Broadcast(CurrentActionCharacter);
}

void UTacticSubsystem::SwitchCharacterAction(ABaseCharacter* BaseCharacter)
{
	CurrentActionCharacter = BaseCharacter;

	if (BaseCharacter->GetTeamComp()->IsPlayerTeam())
	{
		CurrentControlPlayer = BaseCharacter;
	}
}

void UTacticSubsystem::ShowMove()
{
	if (!CurrentControlPlayer)
	{
		{
			FString
				TempStr = FString::Printf(TEXT("!CurrentControlCharacter"));
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
		return;
	}

	FVector projectedLoc;
	//---角色移动范围---
	FVector FinalLocation = UKismetMathLibrary::ClampVectorSize(
		GetTacticPlayerController()->MouseHoveredCursorOverLocation - CurrentControlPlayer->GetActorLocation(),
		0.0f,
		1000.0f) + CurrentControlPlayer->GetActorLocation();

	if (UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), FinalLocation, projectedLoc, nullptr,
	                                                     UNavigationQueryFilter::StaticClass(),
	                                                     FVector(1000, 1000, 1000)))
	{
		UNavigationPath* NaviValue = UNavigationSystemV1::FindPathToLocationSynchronously(
			GetWorld(), CurrentControlPlayer->GetActorLocation(), projectedLoc);

		if (NaviValue != nullptr)
		{
			MovePoints = NaviValue->PathPoints;
			GetShowVisualFeedbackActor()->GetPathTracerComponent()->DrawPath(MovePoints);
		}
	}
}

void UTacticSubsystem::CharacterPreMove(ABaseCharacter* InBaseCharacter)
{
	bCanMove = true;
	GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(VisualFeedBackTimeHandle, this, &ThisClass::ShowMove,
		                                       0.07, true, 0.1f);
	}
}

void UTacticSubsystem::HideVisualFeedback_Move()
{
	GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);
}

UTacticSubsystem::UTacticSubsystem()
{
	FindMyClass(VisualFeedbackActorClass, *VisualFeedbackActor_Path);
}

AShowVisualFeedbackActor* UTacticSubsystem::GetShowVisualFeedbackActor()
{
	if (!ShowVisualFeedbackActor)
	{
		ShowVisualFeedbackActor = GetWorld()->SpawnActor<AShowVisualFeedbackActor>(VisualFeedbackActorClass);
	}
	return ShowVisualFeedbackActor;
}

void UTacticSubsystem::RoundFinish(ABaseCharacter* BaseCharacter)
{
	// Clear path visualization when round finishes
}

void UTacticSubsystem::SelectedSkill(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility)
{
	//选择技能后，
}
