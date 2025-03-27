// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticSubsystem.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyProjectDemo1/Actors/VisualFeedbackActor.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Characters/PlayerCharacter.h"
#include "MyProjectDemo1/Characters/TacticPlayerCharacter.h"
#include "MyProjectDemo1/Components/InteractionComp.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "MyProjectDemo1/Components/PathTracerComponent.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/FilePaths/FilePaths.h"
#include "MyProjectDemo1/Framework/Controllers/TacticPlayerController.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"


void UTacticSubsystem::MyMouseEndCursorOver(ABaseCharacter* BaseCharacter)
{
}

void UTacticSubsystem::MyMouseBeginCursorOver(ABaseCharacter* BaseCharacter)
{
	//if () todo 如果按下技能，则只显示自身的移动范围：  全局就一个圈，我们只调用VisualFeedBackActor就可以了
	{
	}
}

void UTacticSubsystem::CancelMoveAndSkillThenClearVisualFeedback()
{
	bCanMove = false;
	GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);
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

void UTacticSubsystem::Move(ATacticPlayerController* InTacticPlayerController)
{
	if (bCanMove && CurrentActionCharacter)
	{
		UBaseCharacterAttributeSet* BaseCharacterAttributeSet = CurrentActionCharacter->GetBaseCharacterAttributeSet();
		float CurrentActionValues = BaseCharacterAttributeSet->GetActionValues();
		if (CurrentActionCharacter->GetMyAbilityComp() && CurrentActionValues >= 1)
		{
			float MoveRange = BaseCharacterAttributeSet->GetMoveRange();
			CurrentActionCharacter->BaseAIController->MoveToLocationWithPathFinding(
				InTacticPlayerController->LastClickLocation, false, MoveRange);
			// 修改ActionValues属性值
			BaseCharacterAttributeSet->SetActionValues(CurrentActionValues - 1.0f);
		}
	}
}

void UTacticSubsystem::CharacterPreMove(ATacticPlayerController* TacticPlayerController)
{
	bCanMove = true;
	GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UTacticSubsystem::PreMove,
	                          TacticPlayerController);
	GetWorld()->GetTimerManager().SetTimer(VisualFeedBackTimeHandle, TimerDelegate, 0.04f, true, 0.1f);
}

void UTacticSubsystem::SkillRelease(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility)
{
	//todo bug 可能 GlobalPotentialTargets会为空，因为此变量总是改变着
	//所以可能需要先取消，然后0.2s延迟执行	
	for (ABaseCharacter*
	     TempPotentialTarget : GlobalPotentialTargets)
	{
		//BaseAbility->BaseEffect.GetDefaultObject();

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

void UTacticSubsystem::PostSkillSelected(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility)
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UTacticSubsystem::PostSkillSelectedTimer, TacticPlayerController, BaseAbility);
	GetWorld()->GetTimerManager().SetTimer(SelectedSkillTimerHandle, TimerDelegate, 0.02f, true);
}

void UTacticSubsystem::PreSkillSelection(UBaseAbility* BaseAbility)
{
	//也可能可以直接调用VisualFeedbackActor的移动组件。让其变成合适的大小。圈内的Targets高亮
	if (BaseAbility && BaseAbility->GetPotentialTargets(
		GlobalPotentialTargets,
		CurrentActionCharacter, this, CurrentActionCharacter->GetActorLocation()))
	{
	}
}


void UTacticSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnSwitchToNextCharacterAction.AddUObject(this, &ThisClass::SwitchToNextCharacterAction);
	OnRoundFinish.AddUObject(this, &ThisClass::RoundFinish);

	OnPreSkillSelection.AddUObject(this, &ThisClass::PreSkillSelection);
	OnPostSkillSelected.AddUObject(this, &ThisClass::PostSkillSelected);
	OnSkillRelease.AddUObject(this, &ThisClass::SkillRelease);

	OnPreMove.AddUObject(this, &ThisClass::CharacterPreMove);
	OnMove.AddUObject(this, &ThisClass::Move);
	OnCancelMoveAndSkill.AddUObject(this, &ThisClass::CancelMoveAndSkillThenClearVisualFeedback);

	OnMyMouseBeginCursorOver.AddUObject(this, &ThisClass::MyMouseBeginCursorOver);
	OnMyMouseEndCursorOver.AddUObject(this, &ThisClass::MyMouseEndCursorOver);
}

void UTacticSubsystem::Deinitialize()
{
	// Clean up path visualization components
	Super::Deinitialize();
}

void UTacticSubsystem::BeginSwitchCharacter()
{
	// 在这里初始化 CurrentActionCharacter
	if (AllCharactersInOrder.Num() > 0)
	{
		CurrentActionCharacter = AllCharactersInOrder[0];

		// 手动触发一次委托，确保 PlayerController 能获取到正确的角色
		OnSwitchToNextCharacterAction.Broadcast();
	}
}

void UTacticSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	//todo bug CurrentActionCharacter may not be Init in some situations(加载场景卡顿时。。角色Beginplay加载卡顿) somehow
	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ThisClass::BeginSwitchCharacter, 1.f);
}

void UTacticSubsystem::PostInitialize()
{
	Super::PostInitialize();

	// 在这里初始化 CurrentActionCharacter
	if (AllCharactersInOrder.Num() > 0)
	{
		CurrentActionCharacter = AllCharactersInOrder[0];

		// 手动触发一次委托，确保 PlayerController 能获取到正确的角色
		OnSwitchToNextCharacterAction.Broadcast();
	}
}


void UTacticSubsystem::SwitchToNextCharacterAction()
{
	if (!IsValid(AllCharactersInOrder[0])) return;
	CurrentActionCharacter = AllCharactersInOrder[0];
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(CurrentActionCharacter))
	{
		CurrentControlPlayer = PlayerCharacter;
	}
}

void UTacticSubsystem::PreMove(ATacticPlayerController* InTacticPlayerController)
{
	if (!CurrentControlPlayer)
	{
		{
			FString
				TempStr = FString::Printf(TEXT("!CurrentControlPlayer"));
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
		return;
	}

	FVector projectedLoc;
	//---角色移动范围---
	FVector FinalLocation = UKismetMathLibrary::ClampVectorSize(
		InTacticPlayerController->MouseHoveringCursorOverLocation - CurrentControlPlayer->GetActorLocation(),
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


UTacticSubsystem::UTacticSubsystem()
{
	FindMyClass(VisualFeedbackActorClass, *VisualFeedbackActor_Path);
}

AVisualFeedbackActor* UTacticSubsystem::GetShowVisualFeedbackActor()
{
	if (!ShowVisualFeedbackActor)
	{
		ShowVisualFeedbackActor = GetWorld()->SpawnActor<AVisualFeedbackActor>(VisualFeedbackActorClass);
	}
	return ShowVisualFeedbackActor;
}

void UTacticSubsystem::RoundFinish()
{
	// switch to next character 
	// Clear path visualization when round finishes
}

void UTacticSubsystem::ShowGlobalTargetsOutline()
{
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

void UTacticSubsystem::PostSkillSelectedTimer(ATacticPlayerController* InTacticPlayerController,
                                              UBaseAbility* BaseAbility)
{
	if (InTacticPlayerController && BaseAbility && CurrentActionCharacter)
	{
		// 调用并传递参数
		if (BaseAbility->GetPotentialTargets(
			GlobalPotentialTargets,
			CurrentActionCharacter, this,
			InTacticPlayerController->MouseHoveringCursorOverLocation
			/*todo bug test CurrentActionCharacter->GetActorLocation()*/))
		{
			
		}
		// 进行鼠标指针检测
		else if (BaseAbility->GetPotentialTargets(
			GlobalPotentialTargets,
			CurrentActionCharacter, this, InTacticPlayerController->MouseHoveringCursorOverLocation))
		{
			{
				FString TempStr = FString::Printf(TEXT("进行鼠标指针检测"));
				if (GEngine)
					GEngine->
						AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
				UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
			}
			//OnPreMove.Broadcast(BaseCharacter)
			//todo move broadcast
		}
		//超出最远距离检测的实现, 所有技能变红
		else
		{
		}
	}

	ShowGlobalTargetsOutline();
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

	// 从所有队伍中移除该角色
	AllCharactersInOrder.Remove(Character);
	PlayerTeam.Remove(Character);
	EnemyTeam.Remove(Character);
	NeutralTeam.Remove(Character);
}
