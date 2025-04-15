// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyProjectDemo1/BlueprintFunctionLibrary/ThisProjectFunctionLibrary.h"
#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
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


void ABaseAIController::MoveToLocationWithPathFinding(const FVector& MouseClickLocation)
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
	FVector GoalLocation = MouseClickLocation;

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
	MoveRequest.SetGoalLocation(GoalLocation/*ProjectedLocation.Location*/);
	//MoveRequest.SetAcceptanceRadius(5.0f); //todo AIMoveTo Bug / ShowVisualFeedback (it might cause)
	MoveRequest.SetUsePathfinding(true);
	MoveRequest.SetAllowPartialPath(true);

	// 执行移动
	FNavPathSharedPtr NavPath;
	MoveTo(MoveRequest, &NavPath);

	/*
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
	*/
}

void ABaseAIController::MoveToLocationInTacticMode(float RangeToMove)
{
	// 如果不是自由移动且有范围限制，则检查距离
	if (RangeToMove <= 0.0f) return;

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) { return; }

	// 获取缓存的移动路径
	UTacticSubsystem* TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>();
	if (!TacticSubsystem || TacticSubsystem->CachedMovePoints.Num() < 2)
	{
		return;
	}

	// 清除任何正在进行的移动
	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);

	// 保存路径点数组
	CurrentPathPoints = TacticSubsystem->CachedMovePoints;

	//FVector ControlledPawnLocation = ControlledPawn->GetActorLocation();
	// 确保起点是角色当前位置
	//CurrentPathPoints[0] = ControlledPawnLocation;

	for (FVector&
	     CurrentPathPoint : CurrentPathPoints)
	{
		//角色移动时不会陷入地里
		CurrentPathPoint.Z += ControlledPawn->GetDefaultHalfHeight();
	}

	// 重置当前路径点索引和移动进度
	CurrentPathPointIndex = 0;
	CurrentMovementAlpha = 0.0f;

	// 获取角色移动组件，设置移动速度（这个速度会用于计算移动时间）
	if (ATacticBaseCharacter* TacticBaseCharacter = Cast<ATacticBaseCharacter>(ControlledPawn))
	{
		MovementSpeed = TacticBaseCharacter->GetMovementComponent()->GetMaxSpeed();
	}

	// 开始向第一个目标点移动
	StartMovingToNextPoint();
}

void ABaseAIController::StartMovingToNextPoint()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || CurrentPathPoints.Num() <= 1 || CurrentPathPointIndex >= CurrentPathPoints.Num() - 1)
	{
		// 已经到达最后一个点或路径无效
		HandleMoveCompleted(true);
		return;
	}

	// 更新当前索引
	CurrentPathPointIndex++;

	// 获取当前位置和下一个目标位置
	StartLocation = ControlledPawn->GetActorLocation();
	TargetLocation = CurrentPathPoints[CurrentPathPointIndex];

	// 计算方向并立即转向
	FVector Direction = (TargetLocation - StartLocation).GetSafeNormal2D();
	FRotator TargetRotation = Direction.Rotation();
	//TargetRotation.Pitch = 0.0f; // 保持角色直立
	//TargetRotation.Roll = 0.0f;
	ControlledPawn->SetActorRotation(TargetRotation);

	// 计算移动距离和所需时间
	float Distance = FVector::Dist(StartLocation, TargetLocation);
	float MoveTime = Distance / MovementSpeed;

	// 重置移动进度
	CurrentMovementAlpha = 0.0f;

	// 启动定时器来平滑移动
	GetWorld()->GetTimerManager().SetTimer(
		MoveTimerHandle,
		this,
		&ABaseAIController::UpdateMovement,
		0.016f, // 约60帧每秒
		true
	);

	// 存储计算的移动时间
	MovementDuration = MoveTime;

	/*if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("Starting move to point %d, distance: %.2f, time: %.2f"),
		                                   CurrentPathPointIndex, Distance, MoveTime);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, DebugMsg);
	}*/
}

void ABaseAIController::UpdateMovement()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
		return;
	}

	// 更新移动进度
	CurrentMovementAlpha += GetWorld()->GetDeltaSeconds() / MovementDuration;

	// 限制进度在0-1之间
	CurrentMovementAlpha = FMath::Clamp(CurrentMovementAlpha, 0.0f, 1.0f);

	// 使用线性插值计算当前位置
	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, CurrentMovementAlpha);

	// 设置角色位置
	ControlledPawn->SetActorLocation(NewLocation);

	// 如果到达目标点，移动到下一个点或结束
	if (CurrentMovementAlpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);

		// 如果还有更多点，继续移动
		if (CurrentPathPointIndex < CurrentPathPoints.Num() - 1)
		{
			StartMovingToNextPoint();
		}
		else
		{
			// 已完成所有点
			HandleMoveCompleted(true);
		}
	}
}

void ABaseAIController::HandleMoveCompleted(bool bSuccess)
{
	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);

	if (UTacticSubsystem* TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>())
	{
		if (bSuccess)
		{
			// 只有在成功完成所有路径点后才调用技能释放
			TacticSubsystem->TryReleaseSkillAfterMove();

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("移动完成"));
			}
		}
		else
		{
			// 移动失败处理
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("移动失败"));
			}
		}
	}
}

void ABaseAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
}
