// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "MyProjectDemo1/BlueprintFunctionLibrary/ThisProjectFunctionLibrary.h"
#include "Perception/AIPerceptionComponent.h"


void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(InPawn))
	{
	}
}

void ABaseAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

ETeamAttitude::Type ABaseAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	return Super::GetTeamAttitudeTowards(Other);
}

ABaseAIController::ABaseAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	//添加感知组件
	//PerceptionComponent = CreateComponent<UAIPerceptionComponent>();
	//PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::OnReceiveTargetUpdated);
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseAIController::MoveToLocationWithPathFinding(const FVector& MouseClickLocation, bool IsFreeToMove,
                                                      float RangeToMove)
{
	// 获取当前控制的角色
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	// 添加导航系统检查
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		return;
	}

	// 获取当前位置
	FVector CurrentLocation = ControlledPawn->GetActorLocation();

	// 计算目标位置
	FVector TargetLocation = MouseClickLocation;

	// 如果不是自由移动且有范围限制，则检查距离
	if (!IsFreeToMove && RangeToMove > 0.0f)
	{
		UThisProjectFunctionLibrary::ClampMoveRange(CurrentLocation, RangeToMove, TargetLocation);
	}

	// 在目标点周围找一个可到达的点
	/*
	FNavLocation ProjectedLocation;
	const float SearchRadius = 300.0f; // 搜索半径
	bool bProjectionSuccess = NavSys->ProjectPointToNavigation(
		TargetLocation,
		ProjectedLocation,
		FVector(SearchRadius, SearchRadius, SearchRadius) // 搜索范围
	);
	if (!bProjectionSuccess)
	{
		return;
	}
	*/

	// 配置移动请求
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation/*ProjectedLocation.Location*/);
	MoveRequest.SetAcceptanceRadius(5.0f); //todo AIMoveTo Bug / ShowVisualFeedback (it might cause)
	MoveRequest.SetUsePathfinding(true);
	MoveRequest.SetAllowPartialPath(true);

	// 执行移动
	FNavPathSharedPtr NavPath;
	MoveTo(MoveRequest, &NavPath);


	// 可视化 --Debug
	if (NavPath.IsValid())
	{
		// 绘制原始目标点和投影点
		DrawDebugSphere(GetWorld(), MouseClickLocation, 20.0f, 12, FColor::Red, false, DebugLifeTime);
		DrawDebugSphere(GetWorld(), TargetLocation, 20.0f, 12, FColor::Green, false, DebugLifeTime);

		// 如果有范围限制，绘制实际移动目标
		if (!IsFreeToMove && RangeToMove > 0.0f)
		{
			// Draw the target point
			DrawDebugSphere(GetWorld(), TargetLocation, 20.0f, 12, FColor::Blue, false, DebugLifeTime);
			// Draw line from current location to target
			//DrawDebugLine(GetWorld(), CurrentLocation, TargetLocation, FColor::Cyan, false, DebugLifeTime);

			// Draw a circle to represent the movement range (instead of a sphere)
			const int32 NumSegments = 32;
			const float AngleStep = 2.0f * PI / NumSegments;
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
	}
}

void ABaseAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
}
