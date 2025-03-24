// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamComp.h"

#include "MyAbilityComp.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/FilePaths/FilePaths.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"


UTeamComp::UTeamComp(): Owner_BaseCharacter(nullptr), OriginalTeam()
{
	PrimaryComponentTick.bCanEverTick = true;
	// 默认设置为无团队
	TeamType = ETeamType::ETT_None;
}


void UTeamComp::BeginPlay()
{
	Super::BeginPlay();
	Owner_BaseCharacter = Cast<ABaseCharacter>(GetOwner());
	TacticSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTacticSubsystem>();
}


void UTeamComp::SetTeam(ETeamType NewTeam)
{
	{
		FString
			TempStr = FString::Printf(TEXT("SetTem%hhd"), NewTeam);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}

	// Remove from original team
	if (TacticSubsystem->GetAllCharactersInOrder().Contains(Owner_BaseCharacter))
	{
		TacticSubsystem->RemoveCharacterFromTeamByType(Owner_BaseCharacter);
	}

	TeamType = NewTeam;


	// Add to new team	
	TacticSubsystem->AddCharacterToTeamByType(Owner_BaseCharacter);

	// 可以在这里添加团队变更时的逻辑
	// 例如更新角色外观、等

	// 如果使用AIController，可以更新其团队设置

	{
		// 如果AIController有团队设置，可以在这里同步
		// BaseAIController->SetTeam(NewTeam);

		//如果player被魅惑，变成敌方，那么就要实现一些变成敌方的逻辑
	}
}

bool UTeamComp::IsHostileTo(const ABaseCharacter* Other) const
{
	if (!Other || !Owner_BaseCharacter)
		return false;

	// 检查魅惑状态
	FGameplayTag CharmTag = FGameplayTag::RequestGameplayTag(FName(*Charm_Path));
	if (Owner_BaseCharacter->GetMyAbilityComp()->HasMatchingGameplayTag(CharmTag))
	{
		return true; // 所有人对被魅惑的角色都敌对
	}

	ETeamType OtherTeam = Other->GetTeamComp()->GetTeam();

	// 基本敌对规则
	switch (TeamType)
	{
	case ETeamType::ETT_Player:
		return OtherTeam == ETeamType::ETT_Enemy || OtherTeam == ETeamType::ETT_Neutral; // P->E, P->N
	case ETeamType::ETT_Enemy:
		return OtherTeam == ETeamType::ETT_Player || OtherTeam == ETeamType::ETT_Neutral; // E->P, E->N
	case ETeamType::ETT_Neutral:
		return OtherTeam == ETeamType::ETT_Enemy; // N->E
	default:
		return false;
	}
}

bool UTeamComp::IsFriendlyTo(const ABaseCharacter* Other) const
{
	if (!Other || !Owner_BaseCharacter)
		return false;

	// 检查魅惑状态
	FGameplayTag CharmTag = FGameplayTag::RequestGameplayTag(FName(*Charm_Path));
	if (Owner_BaseCharacter->GetMyAbilityComp()->HasMatchingGameplayTag(CharmTag))
	{
		return false; // 所有人对被魅惑的角色都不友好
	}

	ETeamType OtherTeam = Other->GetTeamComp()->GetTeam();

	// 基本友好规则
	switch (TeamType)
	{
	case ETeamType::ETT_Player:
		return OtherTeam == ETeamType::ETT_Player || OtherTeam == ETeamType::ETT_Neutral; // P->P, P->N
	case ETeamType::ETT_Enemy:
		return OtherTeam == ETeamType::ETT_Enemy; // E->E
	case ETeamType::ETT_Neutral:
		return OtherTeam == ETeamType::ETT_Player || OtherTeam == ETeamType::ETT_Neutral; // N->P, N->N
	default:
		return false;
	}
}

ABaseCharacter* UTeamComp::GetNearestHostileCharacter() const
{
	ABaseCharacter* NearestHostile = nullptr;
	float MinDistance = MAX_FLT;

	// 获取所有敌对角色
	TArray<ABaseCharacter*> HostileCharacters = TacticSubsystem->GetAllHostileCharacters(Owner_BaseCharacter);

	// 找出最近的敌对角色
	for (ABaseCharacter* Hostile : HostileCharacters)
	{
		if (Hostile)
		{
			float Distance = FVector::Dist(Owner_BaseCharacter->GetActorLocation(), Hostile->GetActorLocation());
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

	// 获取所有友好角色
	TArray<ABaseCharacter*> FriendlyCharacters = TacticSubsystem->GetAllFriendlyCharacters(Owner_BaseCharacter);

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

bool UTeamComp::CanAttackTarget_ByAttackRange(const ABaseCharacter* Target) const
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
	TArray<ABaseCharacter*> HostileCharacters = TacticSubsystem->GetAllHostileCharacters(Owner_BaseCharacter);

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

void UTeamComp::Charm()
{
	if (!Owner_BaseCharacter) return;
	// 保存原始队伍类型
	OriginalTeam = TeamType;

	// 根据原始队伍类型设置新队伍
	switch (TeamType)
	{
	case ETeamType::ETT_Enemy:
		SetTeam(ETeamType::ETT_Neutral); //E->N
		break;
	case ETeamType::ETT_Neutral:
	case ETeamType::ETT_Player:
		SetTeam(ETeamType::ETT_Enemy); //N->E //P->E
		break;
	default:
		break;
	}
}

void UTeamComp::UnCharm()
{
	if (!Owner_BaseCharacter) return;
	// 恢复原始队伍
	SetTeam(OriginalTeam);
}
