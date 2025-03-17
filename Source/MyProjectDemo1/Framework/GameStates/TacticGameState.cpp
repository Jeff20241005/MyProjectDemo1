// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticGameState.h"

#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"

void ATacticGameState::SortCharactersByActionValues()
{
	AllCharactersInOrder.Sort([](const ABaseCharacter& A, const ABaseCharacter& B) -> bool
	{
		const float ValueA = A.GetBaseCharacterAttributeSet() ? A.GetBaseCharacterAttributeSet()->GetActionValues() : 0.0f;
		const float ValueB = B.GetBaseCharacterAttributeSet() ? B.GetBaseCharacterAttributeSet()->GetActionValues() : 0.0f;
		return ValueA > ValueB;
	});

	// debug field
	/*
	for (const ABaseCharacter* BaseChar : ActionOrder)
	{
		if (BaseChar)
		{
			float ActionValuesTemp = BaseChar->BaseCharacterAttributeSet->GetActionValues();
			FString DebugMsg = FString::Printf(TEXT("Character: %s, ActionValuesTemp: %.1f"),
											   *BaseChar->GetName(), ActionValuesTemp);
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, DebugMsg);
		}
	}
	*/
}


void ATacticGameState::AddCharacterToTeamByType(ABaseCharacter* Character)
{
	if (!Character)
		return;

	// 根据角色的团队类型添加到相应队伍
	switch (Character->GetTeamComp()->GetTeam())
	{
	case ETeamType::ETT_Player:
		PlayerTeam.AddUnique(Character);
		break;
	case ETeamType::ETT_Enemy:
		EnemyTeam.AddUnique(Character);
		break;
	case ETeamType::ETT_Neutral:
		NeutralTeam.AddUnique(Character);
		break;
	default:
		break;
	}

	// 更新行动顺序
	AllCharactersInOrder.AddUnique(Character);
	SortCharactersByActionValues();
}

TArray<ABaseCharacter*> ATacticGameState::GetAllHostileCharacters(const ABaseCharacter* Character) const
{
	if (!Character)
		return TArray<ABaseCharacter*>();

	TArray<ABaseCharacter*> HostileCharacters;
	
	// 筛选敌对角色
	for (ABaseCharacter* OtherCharacter : AllCharactersInOrder)
	{
		if (Character->GetTeamComp()->IsHostileTo(OtherCharacter))
		{
			HostileCharacters.Add(OtherCharacter);
		}
	}

	return HostileCharacters;
}

TArray<ABaseCharacter*> ATacticGameState::GetAllFriendlyCharacters(const ABaseCharacter* Character) const
{
	if (!Character)
		return TArray<ABaseCharacter*>();

	TArray<ABaseCharacter*> FriendlyCharacters;

	// 筛选友好角色
	for (ABaseCharacter* OtherCharacter : AllCharactersInOrder)
	{
		if (Character->GetTeamComp()->IsFriendlyTo(OtherCharacter))
		{
			FriendlyCharacters.Add(OtherCharacter);
		}
	}

	return FriendlyCharacters;
}

bool ATacticGameState::AreAllEnemiesDefeated() const
{
	// 检查敌人队伍是否为空或所有敌人都已死亡
	for (const ABaseCharacter* Enemy : EnemyTeam)
	{
		if (Enemy && Enemy->GetBaseCharacterAttributeSet()->GetHealth() > 0)
		{
			return false;
		}
	}
	return true;
}

bool ATacticGameState::AreAllPlayersDefeated() const
{
	// 检查玩家队伍是否为空或所有玩家角色都已死亡
	for (const ABaseCharacter* Player : PlayerTeam)
	{
		if (Player && Player->GetBaseCharacterAttributeSet()->GetHealth() > 0)
		{
			return false;
		}
	}
	return true;
}

TArray<ABaseCharacter*> ATacticGameState::GetTargetCharacters(
	const ABaseCharacter* SourceCharacter,
	float MaxRange,
	bool bTargetEnemies,
	bool bIncludeSelf,
	bool bTargetAllTeams,
	bool bInfiniteRange) const
{
	TArray<ABaseCharacter*> TargetCharacters;
	
	if (!SourceCharacter)
		return TargetCharacters;
	
	// 如果只选择自己
	if (!bTargetEnemies && bIncludeSelf && !bTargetAllTeams)
	{
		TargetCharacters.Add(const_cast<ABaseCharacter*>(SourceCharacter));
		return TargetCharacters;
	}
	
	// 遍历所有角色
	for (ABaseCharacter* PotentialTarget : AllCharactersInOrder)
	{
		// 跳过无效目标
		if (!PotentialTarget || PotentialTarget->GetBaseCharacterAttributeSet()->GetHealth() <= 0)
			continue;
			
		// 处理自身
		if (PotentialTarget == SourceCharacter)
		{
			if (bIncludeSelf)
				TargetCharacters.Add(PotentialTarget);
			continue;
		}
		
		// 检查团队关系
		bool bIsValidTeamTarget = false;
		
		if (bTargetAllTeams)
		{
			// 选择所有团队
			bIsValidTeamTarget = true;
		}
		else if (bTargetEnemies)
		{
			// 选择敌对目标
			bIsValidTeamTarget = SourceCharacter->GetTeamComp()->IsHostileTo(PotentialTarget);
		}
		else
		{
			// 选择友方目标
			bIsValidTeamTarget = SourceCharacter->GetTeamComp()->IsFriendlyTo(PotentialTarget);
		}
		
		if (!bIsValidTeamTarget)
			continue;
			
		// 检查距离
		if (!bInfiniteRange)
		{
			float Distance = FVector::Distance(
				SourceCharacter->GetActorLocation(),
				PotentialTarget->GetActorLocation());
				
			if (Distance > MaxRange)
				continue;
		}
		
		// 添加有效目标
		TargetCharacters.Add(PotentialTarget);
	}
	
	return TargetCharacters;
}
