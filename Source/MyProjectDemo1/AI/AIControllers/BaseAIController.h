// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"


UCLASS()
class MYPROJECTDEMO1_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	// 可以添加自定义的移动函数
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	virtual void MoveToLocationWithPathFinding(const FVector& MouseClickLocation);

private:
	friend ATacticBaseCharacter;
	void MoveToLocationInTacticMode(float RangeToMove);
private:
	// 当前移动目标点的索引
	int32 CurrentPathPointIndex = 0;

	// 路径点数组缓存
	TArray<FVector> CurrentPathPoints;
	
	// 移动相关变量
	FVector StartLocation;
	FVector TargetLocation;
	float CurrentMovementAlpha = 0.0f;
	float MovementDuration = 0.0f;
	float MovementSpeed = 600.0f;

	// 移动定时器
	FTimerHandle MoveTimerHandle;

	// 开始移动到下一个点
	void StartMovingToNextPoint();

	// 更新移动位置
	void UpdateMovement();

	// 处理移动完成
	void HandleMoveCompleted(bool bSuccess);

protected:
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	ABaseAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	virtual void BeginPlay() override;

	template <class Comp>
	Comp* CreateComponent();
	// 移动完成的回调
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	virtual void Tick(float DeltaTime) override;
	float DebugLifeTime = 1.5f;
};

//记得包含头文件在C++文件
template <typename Comp>
Comp* ABaseAIController::CreateComponent()
{
	TArray<UActorComponent*> ExistingComponents;
	GetComponents(ExistingComponents);
	int32 CompCount = 0;
	for (UActorComponent* CompInstance : ExistingComponents)
	{
		if (CompInstance->IsA<Comp>())
		{
			CompCount++;
		}
	}

	FName CompName = FName(*(Comp::StaticClass()->GetName() + TEXT("_") + FString::FromInt(CompCount)));

	Comp* TheComp = CreateDefaultSubobject<Comp>(CompName);
	if (USceneComponent* TempSceneComponentTemp = Cast<USceneComponent>(TheComp))
	{
		if (TempSceneComponentTemp)
		{
			TempSceneComponentTemp->SetupAttachment(RootComponent);
		}
	}

	return TheComp;
}
