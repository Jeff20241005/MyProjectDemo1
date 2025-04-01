// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeamComp.generated.h"


class UTacticSubsystem;
class ATacticBaseCharacter;

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	ETT_Player UMETA(DisplayName = "Player"),
	ETT_Enemy UMETA(DisplayName = "Enemy"),
	ETT_Neutral UMETA(DisplayName = "Neutral"),
	ETT_None UMETA(DisplayName = "None")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECTDEMO1_API UTeamComp : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Team")
	void SetTeam(ETeamType NewTeam);

	UFUNCTION(BlueprintPure, Category = "Team")
	ETeamType GetTeam() const { return TeamType; }

	UFUNCTION(BlueprintPure, Category = "Team")
	bool IsPlayerTeam() const { return TeamType == ETeamType::ETT_Player; }

	UFUNCTION(BlueprintPure, Category = "Team")
	bool IsEnemyTeam() const { return TeamType == ETeamType::ETT_Enemy; }

	UFUNCTION(BlueprintPure, Category = "Team")
	bool IsNeutralTeam() const { return TeamType == ETeamType::ETT_Neutral; }

	UFUNCTION(BlueprintPure, Category = "Team")
	bool IsHostileTo(const ATacticBaseCharacter* Other) const;

	UFUNCTION(BlueprintPure, Category = "Team")
	bool IsFriendlyTo(const ATacticBaseCharacter* Other) const;

	// 获取最近的敌对角色
	UFUNCTION(BlueprintCallable, Category = "Team")
	ATacticBaseCharacter* GetNearestHostileCharacter() const;

	// 获取最近的友好角色
	UFUNCTION(BlueprintCallable, Category = "Team")
	ATacticBaseCharacter* GetNearestFriendlyCharacter() const;

	// 检查是否可以攻击目标角色（基于距离和敌对关系）
	UFUNCTION(BlueprintCallable, Category = "Team")
	bool CanAttackTarget_ByAttackRange(const ATacticBaseCharacter* Target) const;

	// 获取在攻击范围内的敌对角色
	UFUNCTION(BlueprintCallable, Category = "Team")
	TArray<ATacticBaseCharacter*> GetHostileCharactersInAttackRange() const;

	UFUNCTION(BlueprintCallable, Category = "Team")
	void Charm();

	UFUNCTION(BlueprintCallable, Category = "Team")
	void UnCharm();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	ETeamType TeamType;
	UPROPERTY()
	ATacticBaseCharacter* Owner_BaseCharacter;
	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;

	UTeamComp();

	virtual void BeginPlay() override;

private:
	ETeamType OriginalTeam;
};
