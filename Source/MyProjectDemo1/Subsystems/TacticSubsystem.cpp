// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticSubsystem.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyProjectDemo1/Actors/ShowVisualFeedbackActor.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Characters/PlayerCharacter.h"
#include "MyProjectDemo1/Components/InteractionComp.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "MyProjectDemo1/Components/PathTracerComponent.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/FilePaths/FilePaths.h"
#include "MyProjectDemo1/Framework/Controllers/TacticPlayerController.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"


void UTacticSubsystem::PreSkillSelection(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility)
{
	//todo !!!
	/*if (BaseCharacter && BaseAbility)
	{
		
		);
	}*/
}

void UTacticSubsystem::Move(ABaseCharacter* BaseCharacter)
{
	if (bCanMove)
	{
		UBaseCharacterAttributeSet* BaseCharacterAttributeSet = BaseCharacter->GetBaseCharacterAttributeSet();
		float CurrentActionValues = BaseCharacterAttributeSet->GetActionValues();
		if (BaseCharacter->GetMyAbilityComp() && CurrentActionValues >= 1)
		{
			float MoveRange = BaseCharacterAttributeSet->GetMoveRange();
			BaseCharacter->BaseAIController->MoveToLocationWithPathFinding(
				GetTacticPlayerController()->LastClickLocation, false, MoveRange);
			// 修改ActionValues属性值
			BaseCharacterAttributeSet->SetActionValues(CurrentActionValues - 1.0f);
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);
	bCanMove = false;
}

void UTacticSubsystem::CancelMove(ABaseCharacter* BaseCharacter)
{
	GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);
	bCanMove = false;
}

void UTacticSubsystem::MyMouseEndCursorOver(ABaseCharacter* BaseCharacter)
{
}

void UTacticSubsystem::MyMouseBeginCursorOver(ABaseCharacter* BaseCharacter)
{
	//if () todo 如果按下技能，则只显示自身的移动范围：  全局就一个圈，我们只调用VisualFeedBackActor就可以了
	{
	}
}


void UTacticSubsystem::SkillRelease(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility,
                                    TArray<ABaseCharacter*> PotentialTargets)
{
	TArray<ABaseCharacter*> TempPotentialTargets;
	// 调用 GetPotentialTargets 并传递参数
	BaseAbility->GetPotentialTargets(
		GetShowVisualFeedbackActor(),
		GetTacticPlayerController()->LastClickLocation,
		TempPotentialTargets,
		BaseCharacter);

	for (ABaseCharacter*
	     TempPotentialTarget : TempPotentialTargets)
	{
		//todo cut these to BaseAbility class in Activation function 
		/*FGameplayEffectSpecHandle SpecHandle = BaseCharacter->GetMyAbilityComp()->MakeOutgoingSpec(
			UEffectBase::StaticClass(), 1, BaseCharacter->GetMyAbilityComp()->MakeEffectContext()
		);

		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("TODO")), NewHealth);
			BaseCharacter->GetMyAbilityComp()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}*/
	}
}


void UTacticSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnSwitchCharacterAction.AddUObject(this, &ThisClass::SwitchCharacterAction);
	OnRoundFinish.AddUObject(this, &ThisClass::RoundFinish);
	OnPreSkillSelection.AddUObject(this, &ThisClass::PreSkillSelection);
	OnPostSkillSelected.AddUObject(this, &ThisClass::PostSkillSelected);
	OnSkillRelease.AddUObject(this, &ThisClass::SkillRelease);

	OnPreMove.AddUObject(this, &ThisClass::CharacterPreMove);
	OnMove.AddUObject(this, &ThisClass::Move);
	OnCancelMove.AddUObject(this, &ThisClass::CancelMove);

	OnMyMouseBeginCursorOver.AddUObject(this, &ThisClass::MyMouseBeginCursorOver);
	OnMyMouseEndCursorOver.AddUObject(this, &ThisClass::MyMouseEndCursorOver);
}

void UTacticSubsystem::Deinitialize()
{
	// Clean up path visualization components
	Super::Deinitialize();
}

void UTacticSubsystem::PreMoveBroadCast()
{
	OnPreMove.Broadcast(CurrentActionCharacter);
}

void UTacticSubsystem::SwitchCharacterAction(ABaseCharacter* BaseCharacter)
{
	CurrentActionCharacter = BaseCharacter;

	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(BaseCharacter))
	{
		CurrentControlPlayer = PlayerCharacter;
	}
}

void UTacticSubsystem::ShowMove()
{
	if (!CurrentControlPlayer)
	{
		{
			FString
				TempStr = FString::Printf(TEXT("!CurrentControlCharacter"));
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
		return;
	}

	FVector projectedLoc;
	//---角色移动范围---
	FVector FinalLocation = UKismetMathLibrary::ClampVectorSize(
		GetTacticPlayerController()->MouseHoveringCursorOverLocation - CurrentControlPlayer->GetActorLocation(),
		0.0f,
		1000.0f) + CurrentControlPlayer->GetActorLocation();

	if (UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), FinalLocation, projectedLoc, nullptr,
	                                                     UNavigationQueryFilter::StaticClass(),
	                                                     FVector(1000, 1000, 1000)))
	{
		UNavigationPath* NaviValue = UNavigationSystemV1::FindPathToLocationSynchronously(
			GetWorld(), CurrentControlPlayer->GetActorLocation(), projectedLoc);

		if (NaviValue != nullptr)
		{
			MovePoints = NaviValue->PathPoints;
			GetShowVisualFeedbackActor()->GetPathTracerComponent()->DrawPath(MovePoints);
		}
	}
}

void UTacticSubsystem::CharacterPreMove(ABaseCharacter* InBaseCharacter)
{
	bCanMove = true;
	GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(VisualFeedBackTimeHandle, this, &ThisClass::ShowMove,
		                                       0.07, true, 0.1f);
	}
}

void UTacticSubsystem::HideVisualFeedback_Move()
{
	GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);
}

UTacticSubsystem::UTacticSubsystem()
{
	FindMyClass(VisualFeedbackActorClass, *VisualFeedbackActor_Path);
}

AShowVisualFeedbackActor* UTacticSubsystem::GetShowVisualFeedbackActor()
{
	if (!ShowVisualFeedbackActor)
	{
		ShowVisualFeedbackActor = GetWorld()->SpawnActor<AShowVisualFeedbackActor>(VisualFeedbackActorClass);
	}
	return ShowVisualFeedbackActor;
}

void UTacticSubsystem::RoundFinish(ABaseCharacter* BaseCharacter)
{
	// Clear path visualization when round finishes
}

void UTacticSubsystem::PostSkillSelectedTimer(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility)
{
	if (BaseCharacter && BaseAbility)
	{
		// 调用 GetPotentialTargets 并传递参数
		bIsInRange = BaseAbility->GetPotentialTargets(
			GetShowVisualFeedbackActor(),
			GetTacticPlayerController()->
			MouseHoveringCursorOverLocation,
			GlobalPotentialTargets,
			BaseCharacter);

		// 处理目标列表...
		// 例如高亮显示可选目标
		for (ABaseCharacter*
		     CharactersInOrder :
		     GetAllCharactersInOrder())
		{
			if (!GlobalPotentialTargets.Contains(CharactersInOrder))
			{
				CharactersInOrder->GetInteractionComp()->
				                   UnSetAsSkillTarget();
			}
		}
		for (ABaseCharacter* Target : GlobalPotentialTargets)
		{
			if (Target && Target->GetInteractionComp())
			{
				Target->GetInteractionComp()->SetAsSkillTarget();
			}
		}
	}
}

void UTacticSubsystem::PostSkillSelected(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility)
{
	//FTimerDelegate TimerDelegate;
	// TimerDelegate.BindUObject(this, &UTacticSubsystem::PostSkillSelectedTimer, BaseCharacter, BaseAbility);
	// GetWorld()->GetTimerManager().SetTimer(SelectedSkillTimerHandle, TimerDelegate, 0.02f, true);
	// 保存参数到成员变量
	GetWorld()->GetTimerManager().SetTimer(SelectedSkillTimerHandle,
	                                       [&,BaseCharacter,BaseAbility]()
	                                       {
		                                       if (BaseCharacter && BaseAbility)
		                                       {
			                                       // 调用 GetPotentialTargets 并传递参数
			                                       bIsInRange = BaseAbility->GetPotentialTargets(
				                                       GetShowVisualFeedbackActor(),
				                                       GetTacticPlayerController()->
				                                       MouseHoveringCursorOverLocation,
				                                       GlobalPotentialTargets,
				                                       BaseCharacter);

			                                       // 处理目标列表...
			                                       // 例如高亮显示可选目标
			                                       for (ABaseCharacter*
			                                            CharactersInOrder :
			                                            GetAllCharactersInOrder())
			                                       {
				                                       if (!GlobalPotentialTargets.Contains(CharactersInOrder))
				                                       {
					                                       CharactersInOrder->GetInteractionComp()->
					                                                          UnSetAsSkillTarget();
				                                       }
			                                       }
			                                       for (ABaseCharacter* Target : GlobalPotentialTargets)
			                                       {
				                                       if (Target && Target->GetInteractionComp())
				                                       {
					                                       Target->GetInteractionComp()->SetAsSkillTarget();
				                                       }
			                                       }
		                                       }
	                                       },
	                                       0.02f, true);
}

// 添加一个函数来清除定时器和高亮
void UTacticSubsystem::CancelSelectedSkill()
{
	GetWorld()->GetTimerManager().ClearTimer(SelectedSkillTimerHandle);

	/*// 清除所有目标的高亮
	{
		TArray<ABaseCharacter*> AllCharacters = GameState->GetAllCharactersInOrder();
		for (ABaseCharacter* Character : AllCharacters)
		{
			if (Character && Character->GetInteractionComp())
			{
				Character->GetInteractionComp()->EndOfHighlightAsTarget();
			}
		}
	}*/
}

void UTacticSubsystem::SortCharactersByActionValues()
{
	AllCharactersInOrder.Sort([](const ABaseCharacter& A, const ABaseCharacter& B) -> bool
	{
		const float ValueA = A.GetBaseCharacterAttributeSet()->GetActionValues();
		const float ValueB = B.GetBaseCharacterAttributeSet()->GetActionValues();
		return ValueA > ValueB;
	});
}

void UTacticSubsystem::AddCharacterToTeamByType(ABaseCharacter* Character)
{
	if (!Character)
		return;
	// 更新行动顺序
	AllCharactersInOrder.AddUnique(Character);
	SortCharactersByActionValues();

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
}

TArray<ABaseCharacter*> UTacticSubsystem::GetAllHostileCharacters(const ABaseCharacter* Character) const
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

TArray<ABaseCharacter*> UTacticSubsystem::GetAllFriendlyCharacters(const ABaseCharacter* Character) const
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

bool UTacticSubsystem::AreAllEnemiesDefeated() const
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

bool UTacticSubsystem::AreAllPlayersDefeated() const
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

void UTacticSubsystem::RemoveCharacterFromTeamByType(ABaseCharacter* Character)
{
	if (!Character)
		return;
	{
		FString
			TempStr = FString::Printf(TEXT("Remove :%s "), *Character->GetName());
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}
	// 从所有队伍中移除该角色
	AllCharactersInOrder.Remove(Character);
	PlayerTeam.Remove(Character);
	EnemyTeam.Remove(Character);
	NeutralTeam.Remove(Character);
}
