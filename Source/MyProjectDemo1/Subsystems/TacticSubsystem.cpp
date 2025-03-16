// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticSubsystem.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/Framework/Controllers/MyPlayerController.h"
#include "MyProjectDemo1/Other/PathTracerComponent.h"
#include "NavFilters/NavigationQueryFilter.h"

void UTacticSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnSwitchCharacterAction.AddUObject(this, &ThisClass::SwitchCharacterAction);
	OnRoundFinish.AddUObject(this, &ThisClass::RoundFinish);

	OnSkillSelection.AddUObject(this, &ThisClass::SelectedSkill);

	ShowSkillVisualFeedBack();
}

void UTacticSubsystem::Deinitialize()
{
	// Clean up path visualization components
	Super::Deinitialize();
}

void UTacticSubsystem::ShowMove()
{
	{
		FString
			TempStr = FString::Printf(TEXT("Move"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}
}

void UTacticSubsystem::SwitchCharacterAction(ABaseCharacter* BaseCharacter)
{
	if (ETeamType::ETT_Player == BaseCharacter->GetTeamComp()->GetTeam())
	{
		CurrentControlCharacter = BaseCharacter;
	}
}

void UTacticSubsystem::BeginDrawVisualFeedBack()
{
	if (!CurrentControlCharacter) { return; }

	if (AMyPlayerController* MyPlayerController = GetWorld()->GetFirstPlayerController<AMyPlayerController>())
	{
		FVector projectedLoc;
		//---角色移动范围---
		FVector FinalLocation = UKismetMathLibrary::ClampVectorSize(
			MyPlayerController->MouseCursorOverLocation - CurrentControlCharacter->GetActorLocation(),
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
				// Visualize the path using spline meshes
				VisualizePath(MovePoints);
			}
		}
	}
}

void UTacticSubsystem::ShowSkillVisualFeedBack()
{
	GetWorld()->GetTimerManager().SetTimer(VisualFeedBackTimeHandle, this, &ThisClass::BeginDrawVisualFeedBack, 0.07,
	                                       true);
}

void UTacticSubsystem::RoundFinish(ABaseCharacter* BaseCharacter)
{
	// Clear path visualization when round finishes
}

void UTacticSubsystem::SelectedSkill(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility)
{
}

void UTacticSubsystem::VisualizePath(const TArray<FVector>& PathPoints)
{
	//	UPathTracerComponent* PathTracerComponent = CreateUPathTracerComponent();
	//	PathTracerComponent->DrawPath(PathPoints);


	// For now, just draw debug lines and spheres

	if (PathPoints.Num() < 2) return;


	for (int32 i = 0; i < PathPoints.Num() - 1; i++)
	{
		DrawDebugLine(
			GetWorld(),
			PathPoints[i],
			PathPoints[i + 1],
			FColor::Green,
			false,
			DebugLifeTime,
			0,
			3.0f
		);


		DrawDebugSphere(
			GetWorld(),
			PathPoints[i],
			20.0f,
			12,
			FColor::Yellow,
			false,
			DebugLifeTime
		);
	}


	// Draw the last point

	if (PathPoints.Num() > 0)

	{
		DrawDebugSphere(

			GetWorld(),

			PathPoints.Last(),

			20.0f,

			12,

			FColor::Red,

			false,

			DebugLifeTime

		);
	}
}

UPathTracerComponent* UTacticSubsystem::CreateUPathTracerComponent()
{
	if (!PathMesh || !PathMaterial) return nullptr;

	// Create a new spline mesh component
	UPathTracerComponent* SplineMesh = NewObject<UPathTracerComponent>(GetWorld());
	if (!SplineMesh) return nullptr;

	// Register the component
	SplineMesh->RegisterComponent();

	return SplineMesh;
}
