// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticSubsystem.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyProjectDemo1/Actors/ShowVisualFeedbackActor.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/Framework/Controllers/MyPlayerController.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "MyProjectDemo1/Components/PathTracerComponent.h"
#include "MyProjectDemo1/FilePaths/TacticPaths.h"


void UTacticSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnSwitchCharacterAction.AddUObject(this, &ThisClass::SwitchCharacterAction);
	OnRoundFinish.AddUObject(this, &ThisClass::RoundFinish);
	OnPostSkillSelected.AddUObject(this, &ThisClass::SelectedSkill);
}

void UTacticSubsystem::Deinitialize()
{
	// Clean up path visualization components
	Super::Deinitialize();
}

void UTacticSubsystem::SwitchCharacterAction(ABaseCharacter* BaseCharacter)
{
	if (ETeamType::ETT_Player == BaseCharacter->GetTeamComp()->GetTeam())
	{
		CurrentControlCharacter = BaseCharacter;
	}
}

void UTacticSubsystem::ShowMove()
{
	FVector projectedLoc;
	//---角色移动范围---
	FVector FinalLocation = UKismetMathLibrary::ClampVectorSize(
		GetMyPlayerController()->MouseHoverdCursorOverLocation - CurrentControlCharacter->GetActorLocation(),
		0.0f,
		1000.0f) + CurrentControlCharacter->GetActorLocation();

	if (UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), FinalLocation, projectedLoc, nullptr,
	                                                     UNavigationQueryFilter::StaticClass(),
	                                                     FVector(1000, 1000, 1000)))
	{
		UNavigationPath* NaviValue = UNavigationSystemV1::FindPathToLocationSynchronously(
			GetWorld(), CurrentControlCharacter->GetActorLocation(), projectedLoc);

		if (NaviValue != nullptr)
		{
			MovePoints = NaviValue->PathPoints;
			GetShowVisualFeedbackActor()->GetPathTracerComponent()->DrawPath(MovePoints);
		}
	}
}

void UTacticSubsystem::ShowVisualFeedback_Move()
{
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
