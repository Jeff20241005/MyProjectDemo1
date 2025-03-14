// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamComp.h"

#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Framework/GameStates/TacticGameState.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"


UTeamComp::UTeamComp()
{
	PrimaryComponentTick.bCanEverTick = true;
	// 默认设置为无团队
	TeamType = ETeamType::ETT_None;
	// ...
}


void UTeamComp::BeginPlay()
{
	Super::BeginPlay();
	TacticGameState = Cast<ATacticGameState>(GetWorld()->GetGameState());
	Owner_BaseCharacter = Cast<ABaseCharacter>(GetOwner());

	// ...
}


void UTeamComp::SetTeam(ETeamType NewTeam)
{
	TeamType = NewTeam;

	// 可以在这里添加团队变更时的逻辑
	// 例如更新角色外观、通知GameState等

	// 如果使用AIController，可以更新其团队设置

	{
		// 如果你的AIController有团队设置，可以在这里同步
		// BaseAIController->SetTeam(NewTeam);
	}
}

bool UTeamComp::IsHostileTo(const ABaseCharacter* Other) const
{
	if (!Other)
		return false;

	// 基本敌对规则：
	// 1. 玩家团队与敌人团队互相敌对
	// 2. 中立团队不与任何团队敌对
	// 3. 同一团队的角色不互相敌对

	if (TeamType == Other->GetTeamComp()->TeamType)
		return false; // 同一团队不互相敌对

	if (TeamType == ETeamType::ETT_Neutral || Other->GetTeamComp()->TeamType == ETeamType::ETT_Neutral)
		return false; // 中立团队不与任何团队敌对

	if ((TeamType == ETeamType::ETT_Player && Other->GetTeamComp()->TeamType == ETeamType::ETT_Enemy) ||
		(TeamType == ETeamType::ETT_Enemy && Other->GetTeamComp()->TeamType == ETeamType::ETT_Player))
		return true; // 玩家和敌人互相敌对

	return false; // 默认不敌对
}

bool UTeamComp::IsFriendlyTo(const ABaseCharacter* Other) const
{
	if (!Other)
		return false;

	// 友好规则：
	// 1. 同一团队的角色互相友好
	// 2. 中立团队对所有团队友好

	if (TeamType == Other->GetTeamComp()->TeamType)
		return true; // 同一团队互相友好

	if (TeamType == ETeamType::ETT_Neutral || Other->GetTeamComp()->TeamType == ETeamType::ETT_Neutral)
		return true; // 中立团队对所有团队友好

	return false; // 默认不友好
}

ABaseCharacter* UTeamComp::GetNearestHostileCharacter() const
{
	ABaseCharacter* NearestHostile = nullptr;
	float MinDistance = MAX_FLT;

	// 获取所有敌对角色
	TArray<ABaseCharacter*> HostileCharacters = TacticGameState->GetAllHostileCharacters(Owner_BaseCharacter);

	// 找出最近的敌对角色
	for (ABaseCharacter* Hostile : HostileCharacters)
	{
		if (Hostile)
		{
			float Distance = FVector::Dist(Owner_BaseCharacter-> GetActorLocation(), Hostile->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				NearestHostile = Hostile;
			}
		}
	}


	return NearestHostile;
}

ABaseCharacter* UTeamComp::GetNearestFriendlyCharacter() const
{
	ABaseCharacter* NearestFriendly = nullptr;
	float MinDistance = MAX_FLT;

	// 获取GameState
	// 获取所有友好角色
	TArray<ABaseCharacter*> FriendlyCharacters = TacticGameState->GetAllFriendlyCharacters(Owner_BaseCharacter);

	// 找出最近的友好角色（排除自己）
	for (ABaseCharacter* Friendly : FriendlyCharacters)
	{
		if (Friendly && Friendly != Owner_BaseCharacter)
		{
			float Distance = FVector::Dist(Owner_BaseCharacter->GetActorLocation(), Friendly->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				NearestFriendly = Friendly;
			}
		}
	}

	return NearestFriendly;
}

bool UTeamComp::CanAttackTarget(const ABaseCharacter* Target) const
{
	if (!Target || !Owner_BaseCharacter->GetBaseCharacterAttributeSet())
		return false;

	// 检查是否是敌对关系
	if (!IsHostileTo(Target))
		return false;

	// 检查距离是否在攻击范围内
	float AttackRange = Owner_BaseCharacter->GetBaseCharacterAttributeSet()->GetAttackRange();
	float Distance = FVector::Dist(Owner_BaseCharacter->GetActorLocation(), Target->GetActorLocation());

	return Distance <= AttackRange;
}

TArray<ABaseCharacter*> UTeamComp::GetHostileCharactersInAttackRange() const
{
	TArray<ABaseCharacter*> HostilesInRange;

	// 获取所有敌对角色
	TArray<ABaseCharacter*> HostileCharacters = TacticGameState->GetAllHostileCharacters(Owner_BaseCharacter);

	// 筛选在攻击范围内的敌对角色
	float AttackRange = Owner_BaseCharacter->GetBaseCharacterAttributeSet()->GetAttackRange();
	for (ABaseCharacter* Hostile : HostileCharacters)
	{
		if (Hostile)
		{
			float Distance = FVector::Dist(Owner_BaseCharacter->GetActorLocation(), Hostile->GetActorLocation());
			if (Distance <= AttackRange)
			{
				HostilesInRange.Add(Hostile);
			}
		}
	}

	return HostilesInRange;
}
