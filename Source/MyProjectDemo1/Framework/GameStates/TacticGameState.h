// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameState.h"
#include "TacticGameState.generated.h"

class ABaseCharacter;
/**
 * 存储所有玩家都需要知道的游戏状态
 */
UCLASS()
class MYPROJECTDEMO1_API ATacticGameState : public AMyGameState
{
public:
	
	//任意角色行动值改变时，在Attribute里自动调用了此函数
	UFUNCTION(BlueprintCallable, Category = "Utility")
	void SortCharactersByActionValues();

	// 根据角色的团队类型自动添加到相应队伍
	UFUNCTION(BlueprintCallable, Category = "Utility")
	void AddCharacterToTeamByType(ABaseCharacter* Character);

	// 获取对特定角色敌对的所有角色
	UFUNCTION(BlueprintCallable, Category = "Utility")
	TArray<ABaseCharacter*> GetAllHostileCharacters(const ABaseCharacter* Character) const;

	// 获取对特定角色友好的所有角色
	UFUNCTION(BlueprintCallable, Category = "Utility")
	TArray<ABaseCharacter*> GetAllFriendlyCharacters(const ABaseCharacter* Character) const;

	// 检查是否所有敌人都已被击败
	UFUNCTION(BlueprintCallable, Category = "Utility")
	bool AreAllEnemiesDefeated() const;

	// 检查是否所有玩家角色都已被击败
	UFUNCTION(BlueprintCallable, Category = "Utility")
	bool AreAllPlayersDefeated() const;

	// 获取符合条件的目标角色
	UFUNCTION(BlueprintCallable, Category = "Utility")
	TArray<ABaseCharacter*> GetTargetCharacters(
		const ABaseCharacter* SourceCharacter,
		float MaxRange,
		bool bTargetEnemies = true,
		bool bIncludeSelf = false,
		bool bTargetAllTeams = false,
		bool bInfiniteRange = false) const;
	//todo 
	
	
	////
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
	TArray<ABaseCharacter*> GetAllCharactersInOrder() const { return AllCharactersInOrder; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
	TArray<ABaseCharacter*> GetPlayerTeam() const { return PlayerTeam; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
	TArray<ABaseCharacter*> GetEnemyTeam() const { return PlayerTeam; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility")
	TArray<ABaseCharacter*> GetNeutralTeam() const { return NeutralTeam; }

protected:
	UPROPERTY()
	TArray<ABaseCharacter*> EnemyTeam;
	UPROPERTY()
	TArray<ABaseCharacter*> PlayerTeam;
	UPROPERTY()
	TArray<ABaseCharacter*> NeutralTeam;
	// 行动顺序
	UPROPERTY()
	TArray<ABaseCharacter*> AllCharactersInOrder;
	GENERATED_BODY()
};
