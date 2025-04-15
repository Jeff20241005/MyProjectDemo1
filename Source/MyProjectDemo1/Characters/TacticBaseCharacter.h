// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "TacticBaseCharacter.generated.h"

UCLASS()
class MYPROJECTDEMO1_API ATacticBaseCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	void ShowMoveRange();
	void CloseMoveRange();

	void TacticMove(FVector MoveLocation);
	bool CanMove();
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
	float In_Radius = 0.96f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FName MaterialName_Radius = FName("Radius");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FName MaterialName_Color = FName("Color");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FVector DefaultColorValueOfSphereSM = FVector(1, 1, 7);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTeamComp* GetTeamComp() const { return TeamComp; }

	void SetWidgetHealth(float Health, float MaxHealth);

	// 切换为轮换处理图标
	UFUNCTION(BlueprintCallable, Category = "Appearance|Indicators")
	void ShowRotationHandleIndicator();
	
	// 切换为起始轮换处理图标
	UFUNCTION(BlueprintCallable, Category = "Appearance|Indicators")
	void ShowStartRotationHandleIndicator();
	
	// 隐藏头顶图标
	UFUNCTION(BlueprintCallable, Category = "Appearance|Indicators")
	void HideHeadIndicator();

	// 开始旋转头顶图标
	UFUNCTION(BlueprintCallable, Category = "Appearance|Indicators")
	void StartHeadIndicatorRotation();
	
	// 停止旋转头顶图标
	UFUNCTION(BlueprintCallable, Category = "Appearance|Indicators")
	void StopHeadIndicatorRotation();

protected:
	virtual void Destroyed() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;

	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UTeamComp* TeamComp;

	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UWidgetComponent* HealthWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
	UStaticMeshComponent* MoveRangeStaticMeshComponent;

	// 头顶图标组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	UStaticMeshComponent* HeadIndicatorMeshComponent;
	
	// 轮换处理图标的静态网格资源路径
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance|Resources")
	TSoftObjectPtr<UStaticMesh> RotationHandleIndicatorMesh;
	
	// 起始轮换处理图标的静态网格资源路径
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance|Resources")
	TSoftObjectPtr<UStaticMesh> StartRotationHandleIndicatorMesh;

	// 旋转头顶图标
	void RotateHeadIndicator();
	
	// 旋转速度（度/秒）
	UPROPERTY(EditDefaultsOnly, Category = "Appearance|Indicators")
	float HeadIndicatorRotationSpeed = 75.0f;
	
	// 旋转定时器句柄
	FTimerHandle HeadIndicatorRotationTimerHandle;

	ATacticBaseCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
