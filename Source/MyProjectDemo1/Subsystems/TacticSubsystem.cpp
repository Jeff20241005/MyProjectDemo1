// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticSubsystem.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyProjectDemo1/Actors/VisualFeedbackActor.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/BlueprintFunctionLibrary/ThisProjectFunctionLibrary.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Characters/TacticPlayerCharacter.h"
#include "MyProjectDemo1/Components/InteractionComp.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "MyProjectDemo1/Components/PathTracerComp.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/FilePaths/FilePaths.h"
#include "MyProjectDemo1/Framework/Controllers/TacticPlayerController.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"


void UTacticSubsystem::MyMouseEndCursorOver(ATacticBaseCharacter* BaseCharacter)
{
}

void UTacticSubsystem::MyMouseBeginCursorOver(ATacticBaseCharacter* BaseCharacter)
{
	//if () todo 如果按下技能，则只显示自身的移动范围：  全局就一个圈，我们只调用VisualFeedBackActor就可以了
	{
	}
}

void UTacticSubsystem::CancelSkill()
{
	GetWorld()->GetTimerManager().ClearTimer(SelectedSkillTimerHandle);

	GlobalPotentialTargets.Empty();
	CheckGlobalPotentialTargetsOutline();
}

void UTacticSubsystem::Move(ATacticPlayerController* InTacticPlayerController)
{
	if (CurrentActionCharacter && CurrentActionCharacter->CanMove())
	{
		CurrentActionCharacter->Move(InTacticPlayerController->LastClickLocation);
	}
}

void UTacticSubsystem::PreMove(ATacticPlayerController* InTacticPlayerController, UBaseAbility* InBaseAbility)
{
	bCanMove = true;

	if (!ShowVisualFeedbackActor->GetPathTracerComp()->IsActive())
	{
		ShowVisualFeedbackActor->GetPathTracerComp()->Activate();

		GetWorld()->GetTimerManager().ClearTimer(VisualFeedBackTimeHandle);

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UTacticSubsystem::PreMoveTimer, InTacticPlayerController, InBaseAbility);
		GetWorld()->GetTimerManager().SetTimer(VisualFeedBackTimeHandle, TimerDelegate, 0.04f, true);
	}
}

void UTacticSubsystem::SkillRelease(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility)
{
	//todo bug 可能 GlobalPotentialTargets会为空，因为此变量总是改变着
	//所以可能需要先取消，然后0.2s延迟执行	
	for (ATacticBaseCharacter*
	     TempPotentialTarget : GlobalPotentialTargets)
	{
		{
			FString TempStr = FString::Printf(TEXT("Todo here!"));
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
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

void UTacticSubsystem::DoPostSkillSelectedTimer(ATacticPlayerController* TacticPlayerController,
                                                UBaseAbility* BaseAbility)
{
	if (OnPostSkillSelectedTimer.IsBound())
	{
		OnPostSkillSelectedTimer.Broadcast(TacticPlayerController, BaseAbility);
	}
}

void UTacticSubsystem::PostSkillSelected(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility)
{
	CancelMoveAndSkill();

	if (!SelectedSkillTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(SelectedSkillTimerHandle);

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UTacticSubsystem::DoPostSkillSelectedTimer, TacticPlayerController,
		                          BaseAbility);
		GetWorld()->GetTimerManager().SetTimer(SelectedSkillTimerHandle, TimerDelegate, 0.02f, true);
	}
}

void UTacticSubsystem::PreSkillSelection(UBaseAbility* BaseAbility)
{
	//一次性的，可以不用SetTimer。。
	if (BaseAbility && BaseAbility->GetPotentialTargets(
		this, CurrentActionCharacter->GetActorLocation()))
	{
	}
	CheckGlobalPotentialTargetsOutline();
}

void UTacticSubsystem::CancelMove()
{
	bCanMove = false;
}

void UTacticSubsystem::ChangeAutomaticMoveBySkill(bool bNew)
{
	bEnableAutomaticMoveBySkill = bNew;


	if (!bEnableAutomaticMoveBySkill)
	{
		if (OnCancelMove.IsBound())
		{
			OnCancelMove.Broadcast();
		}
	}
}

void UTacticSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	OnChangeAutomaticMoveBySkill.AddUObject(this, &ThisClass::ChangeAutomaticMoveBySkill);

	OnSwitchToNextCharacterAction.AddUObject(this, &ThisClass::SwitchToNextCharacterAction);
	OnRoundFinish.AddUObject(this, &ThisClass::RoundFinish);

	OnCancelMove.AddUObject(this, &ThisClass::CancelMove);
	OnCancelSkill.AddUObject(this, &ThisClass::CancelSkill);


	OnPreSkillSelection.AddUObject(this, &ThisClass::PreSkillSelection);
	OnPostSkillSelected.AddUObject(this, &ThisClass::PostSkillSelected);
	OnPostSkillSelectedTimer.AddUObject(this, &ThisClass::PostSkillSelectedTimer);
	OnSkillRelease.AddUObject(this, &ThisClass::SkillRelease);

	OnPreMove.AddUObject(this, &ThisClass::PreMove);
	OnMove.AddUObject(this, &ThisClass::Move);


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
	//Spawn VisualFeedback actor
	GetVisualFeedbackActor();

	// 在这里初始化 CurrentActionCharacter
	if (AllCharactersInOrder.Num() > 0)
	{
		CurrentActionCharacter = AllCharactersInOrder[0];

		// 手动触发一次委托，确保 PlayerController 能获取到正确的角色
		if (OnSwitchToNextCharacterAction.IsBound())
		{
			OnSwitchToNextCharacterAction.Broadcast();
		}
	}
}

void UTacticSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	//Spawn VisualFeedback actor
	GetVisualFeedbackActor();

	//todo bug CurrentActionCharacter may not be Init in some situations(加载场景卡顿时。。角色Beginplay加载卡顿) somehow
	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ThisClass::BeginSwitchCharacter, 1.f);
}

void UTacticSubsystem::SwitchToNextCharacterAction()
{
	if (!IsValid(AllCharactersInOrder[0])) return;
	CurrentActionCharacter = AllCharactersInOrder[0];
	if (ATacticPlayerCharacter* PlayerCharacter = Cast<ATacticPlayerCharacter>(CurrentActionCharacter))
	{
		CurrentControlPlayer = PlayerCharacter;
	}
}


void UTacticSubsystem::PreMove_IfHasSkillRadius(ATacticPlayerController* InTacticPlayerController,
                                                float SkillPlacementRadius)
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

	FVector PlayerLocation = CurrentControlPlayer->GetActorLocation();
	float RangeToMove = CurrentControlPlayer->GetBaseCharacterAttributeSet()->GetMoveRange();
	FVector TargetLocation = InTacticPlayerController->MouseHoveringCursorOverLocation;
	if (SkillPlacementRadius)
	{
		FVector AdjustTargetLocation = UThisProjectFunctionLibrary::FVectorZToGround(TargetLocation);
		FVector AdjustOwnerSourceLocation = UThisProjectFunctionLibrary::FVectorZToGround(PlayerLocation);
		const float DistanceToMouse = FVector::Dist2D(AdjustOwnerSourceLocation, AdjustTargetLocation);
		float AdjustMoveDistance = DistanceToMouse - SkillPlacementRadius;
		RangeToMove = RangeToMove < AdjustMoveDistance ? RangeToMove : AdjustMoveDistance;
	}

	UThisProjectFunctionLibrary::ClampMoveRange2D(PlayerLocation, RangeToMove, TargetLocation);


	FVector FinalLocation = UKismetMathLibrary::ClampVectorSize(
		TargetLocation - PlayerLocation,
		0.0f,
		1000.0f) + PlayerLocation;

	if (UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), FinalLocation, projectedLoc, nullptr,
	                                                     UNavigationQueryFilter::StaticClass(),
	                                                     FVector(1000, 1000, 1000)))
	{
		UNavigationPath* NaviValue = UNavigationSystemV1::FindPathToLocationSynchronously(
			GetWorld(), PlayerLocation, projectedLoc);

		if (NaviValue != nullptr)
		{
			MovePoints = NaviValue->PathPoints;
			GetVisualFeedbackActor()->GetPathTracerComp()->DrawPath(MovePoints);
		}
	}
}

void UTacticSubsystem::PreMoveTimer(ATacticPlayerController* InTacticPlayerController, UBaseAbility* InBaseAbility)
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

	if (InBaseAbility)
	{
		PreMove_IfHasSkillRadius(InTacticPlayerController,
		                         InBaseAbility->GetSkillPlacementRadiusByAimWithMouse());
	}
	else
	{
		PreMove_IfHasSkillRadius(InTacticPlayerController);
	}
}


UTacticSubsystem::UTacticSubsystem()
{
	FindMyClass(VisualFeedbackActorClass, *VisualFeedbackActor_Path);
}

AVisualFeedbackActor* UTacticSubsystem::GetVisualFeedbackActor()
{
	if (!ShowVisualFeedbackActor)
	{
		ShowVisualFeedbackActor = GetWorld()->SpawnActor<AVisualFeedbackActor>(VisualFeedbackActorClass);
	}
	return ShowVisualFeedbackActor;
}

void UTacticSubsystem::CancelMoveAndSkill()
{
	if (OnCancelMove.IsBound())
	{
		OnCancelMove.Broadcast();
	}
	if (OnCancelSkill.IsBound())
	{
		OnCancelSkill.Broadcast();
	}
}

void UTacticSubsystem::RoundFinish()
{
	// switch to next character 
	// Clear path visualization when round finishes
}

void UTacticSubsystem::CheckGlobalPotentialTargetsOutline()
{
	for (ATacticBaseCharacter*
	     CharactersInOrder :
	     AllCharactersInOrder)
	{
		if (!GlobalPotentialTargets.Contains(CharactersInOrder))
		{
			CharactersInOrder->GetInteractionComp()->
			                   UnSetAsSkillTarget();
		}
	}
	for (ATacticBaseCharacter* Target : GlobalPotentialTargets)
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
		FVector MouseLocation = InTacticPlayerController->MouseHoveringCursorOverLocation;
		if (bEnableAutomaticMoveBySkill)
		{
			if (BaseAbility->GetPotentialTargets(this, MouseLocation))
			{
				if (OnCancelMove.IsBound())
				{
					OnCancelMove.Broadcast();
				}
			}
			else
			{
				BaseAbility->GetPotentialTargets(this, MouseLocation, true);
				if (OnPreMove.IsBound())
				{
					OnPreMove.Broadcast(InTacticPlayerController, BaseAbility);
				}
			}
		}
		else
		{
			BaseAbility->GetPotentialTargets(this, MouseLocation);
		}
	}

	CheckGlobalPotentialTargetsOutline();
}


void UTacticSubsystem::SortCharactersByActionValues()
{
	AllCharactersInOrder.Sort([](const ATacticBaseCharacter& A, const ATacticBaseCharacter& B) -> bool
	{
		const float ValueA = A.GetBaseCharacterAttributeSet()->GetActionValues();
		const float ValueB = B.GetBaseCharacterAttributeSet()->GetActionValues();
		return ValueA > ValueB;
	});
}

void UTacticSubsystem::AddCharacterToTeamByType(ATacticBaseCharacter* Character)
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

TArray<ATacticBaseCharacter*> UTacticSubsystem::GetAllHostileCharacters(const ATacticBaseCharacter* Character) const
{
	if (!Character)
		return TArray<ATacticBaseCharacter*>();

	TArray<ATacticBaseCharacter*> HostileCharacters;

	// 筛选敌对角色
	for (ATacticBaseCharacter* OtherCharacter : AllCharactersInOrder)
	{
		if (Character->GetTeamComp()->IsHostileTo(OtherCharacter))
		{
			HostileCharacters.Add(OtherCharacter);
		}
	}

	return HostileCharacters;
}

TArray<ATacticBaseCharacter*> UTacticSubsystem::GetAllFriendlyCharacters(const ATacticBaseCharacter* Character) const
{
	if (!Character)
		return TArray<ATacticBaseCharacter*>();

	TArray<ATacticBaseCharacter*> FriendlyCharacters;

	// 筛选友好角色
	for (ATacticBaseCharacter* OtherCharacter : AllCharactersInOrder)
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
	for (const ATacticBaseCharacter* Enemy : EnemyTeam)
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
	for (const ATacticBaseCharacter* Player : PlayerTeam)
	{
		if (Player && Player->GetBaseCharacterAttributeSet()->GetHealth() > 0)
		{
			return false;
		}
	}
	return true;
}

void UTacticSubsystem::RemoveCharacterFromTeamByType(ATacticBaseCharacter* Character)
{
	if (!Character)
		return;

	// 从所有队伍中移除该角色
	AllCharactersInOrder.Remove(Character);
	PlayerTeam.Remove(Character);
	EnemyTeam.Remove(Character);
	NeutralTeam.Remove(Character);
}
