// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticSubsystem.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/Framework/Controllers/MyPlayerController.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Other/PathTracerComponent.h"
#include "NavFilters/NavigationQueryFilter.h"

void UTacticSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnSwitchCharacterAction.AddUObject(this, &ThisClass::SwitchCharacterAction);
	OnRoundFinish.AddUObject(this, &ThisClass::RoundFinish);

	OnSkillSelection.AddUObject(this, &ThisClass::SelectedSkill);

	// 确保在游戏世界有效时才获取控制器
	if (GetWorld())
	{
		MyPlayerController = GetWorld()->GetFirstPlayerController<AMyPlayerController>();
		
		// 只有在控制器有效时才调用
		if (MyPlayerController)
		{
			ShowSkillVisualFeedBack();
		}
	}
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

	// 检查必要的指针是否有效
	if (!CurrentControlCharacter || !MyPlayerController)
	{ 
		// 如果定时器仍在运行但对象无效，停止定时器
		if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(VisualFeedBackTimeHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);
		}
		return; 
	}

		
	FVector projectedLoc;
	//---角色移动范围---
	FVector FinalLocation = UKismetMathLibrary::ClampVectorSize(
		MyPlayerController->MouseHoverdCursorOverLocation - CurrentControlCharacter->GetActorLocation(),
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

			MyPlayerController.
			// Visualize the path using spline meshes
			DebugVisual(MovePoints);
		}
	}
}

void UTacticSubsystem::ShowSkillVisualFeedBack()
{
	// 确保先清除之前的定时器
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);
		
		// 确保MyPlayerController有效
		if (!MyPlayerController)
		{
			MyPlayerController = GetWorld()->GetFirstPlayerController<AMyPlayerController>();
		}
		
		// 只有在必要的指针有效时才设置定时器
		if (MyPlayerController)
		{
			GetWorld()->GetTimerManager().SetTimer(VisualFeedBackTimeHandle, this, &ThisClass::BeginDrawVisualFeedBack, 0.07, true);
		}
	}
}

void UTacticSubsystem::RoundFinish(ABaseCharacter* BaseCharacter)
{
	// Clear path visualization when round finishes
}

void UTacticSubsystem::SelectedSkill(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility)
{
}

void UTacticSubsystem::DebugVisual(const TArray<FVector>& PathPoints)
{
	// 检查必要的指针和条件
	if (PathPoints.Num() < 2 || !CurrentControlCharacter || !MyPlayerController || !GetWorld()) 
		return;
	FString TempStr = FString::Printf(TEXT("No specific class specified at "));
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
	UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	// 如果有范围限制，绘制实际移动目标
	float RangeToMove = CurrentControlCharacter->GetBaseCharacterAttributeSet()->GetMoveRange();
	
	AMyGameMode* MyGameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (MyGameMode && MyGameMode->IsTacticMode())
	{
		if (RangeToMove > 0.0f)
			// Draw the target point
			DrawDebugSphere(GetWorld(), MyPlayerController->MouseHoverdCursorOverLocation, 20.0f, 12,
							FColor::Blue, false, DebugLifeTime);

		// Draw a circle to represent the movement range (instead of a sphere)
		const int32 NumSegments = 32;
		const float AngleStep = 2.0f * PI / NumSegments;
		FVector CurrentLocation = CurrentControlCharacter->GetActorLocation();
		FVector PrevPoint = CurrentLocation + FVector(RangeToMove, 0.0f, 0.0f);

		for (int32 i = 1; i <= NumSegments; i++)
		{
			const float Angle = AngleStep * i;
			const FVector CurrentPoint = CurrentLocation +
				FVector(RangeToMove * FMath::Cos(Angle), RangeToMove * FMath::Sin(Angle), 0.0f);

			DrawDebugLine(
				GetWorld(),
				PrevPoint,
				CurrentPoint,
				FColor::Yellow,
				false,
				DebugLifeTime,
				0,
				2.0f
			);

			PrevPoint = CurrentPoint;
		}
	}

	// 绘制路径
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

