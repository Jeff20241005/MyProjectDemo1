// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticSubsystem.h"

#include "EngineUtils.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyProjectDemo1/Actors/VisualFeedbackActor.h"
#include "MyProjectDemo1/BlueprintFunctionLibrary/ThisProjectFunctionLibrary.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Characters/TacticPlayerCharacter.h"
#include "MyProjectDemo1/Components/InteractionComp.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "MyProjectDemo1/Components/PathTracerComp.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/FilePaths/DefaultPropertyValue.h"
#include "MyProjectDemo1/FilePaths/FilePaths.h"
#include "MyProjectDemo1/Framework/Controllers/TacticPlayerController.h"
#include "MyProjectDemo1/Framework/HUD/TacticHUD.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"
#include "NavMesh/NavMeshBoundsVolume.h"


void UTacticSubsystem::MyMouseEndCursorOver(ATacticBaseCharacter* TacticBaseCharacter)
{
	HoveredTacticBaseCharacter = nullptr;
	if (!CurrentSelectedBaseAbility && !bIsAttemptToMove)
	{
		TacticBaseCharacter->CloseMoveRange();
	}
}

void UTacticSubsystem::MyMouseBeginCursorOver(ATacticBaseCharacter* TacticBaseCharacter)
{
	HoveredTacticBaseCharacter = TacticBaseCharacter;

	if (!CurrentSelectedBaseAbility && !bIsAttemptToMove)
	{
		TacticBaseCharacter->ShowMoveRange();
	}
}

void UTacticSubsystem::CancelSkill()
{
	if (!CurrentActionBaseCharacter) { return; }
	GetWorld()->GetTimerManager().ClearTimer(SelectedSkillTimerHandle);

	GlobalPotentialTargets.Empty();
	CheckGlobalPotentialTargetsOutline();

	CurrentActionBaseCharacter->CloseMoveRange();

	CurrentSelectedBaseAbility = nullptr;
}

void UTacticSubsystem::Move(ATacticPlayerController* InTacticPlayerController)
{
	if (CurrentActionBaseCharacter && CurrentActionBaseCharacter->CanMove() && bIsAttemptToMove)
	{
		if (bCanMove)
		{
			if (!bIsOverlappingWithCharacter)
			{
				CurrentActionBaseCharacter->TacticMove(CachedPremoveFinalLocation);
				SetbCanMove(false);
				CancelMoveAndSkill();
			}
			else
			{
				{
					FString TempStr = FString::Printf(TEXT("角色不能移动到此位置"));
					if (GEngine)
						GEngine->
							AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
					UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
				}
			}
		}
	}
	else
	{
		{
			FString TempStr = FString::Printf(TEXT("行动点不足：TODO UI display"));
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
	}
}

void UTacticSubsystem::PreMove(ATacticPlayerController* InTacticPlayerController, UBaseAbility* InBaseAbility)
{
	bIsAttemptToMove = true;

	if (CurrentActionBaseCharacter)
	{
		CurrentActionBaseCharacter->ShowMoveRange();
	}

	if (!ShowVisualFeedbackActor->GetPathTracerComp()->IsActive())
	{
		ShowVisualFeedbackActor->GetPathTracerComp()->Activate();

		GetWorld()->GetTimerManager().ClearTimer(PreMoveTimerTimeHandle);

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UTacticSubsystem::PreMoveTimer, InTacticPlayerController, InBaseAbility);
		GetWorld()->GetTimerManager().SetTimer(PreMoveTimerTimeHandle, TimerDelegate, 0.04f, true);
	}
}

void UTacticSubsystem::ReleaseSkillActiveAbility(UMyAbilityComp* MyAbilityComp, UBaseAbility* BaseAbility)
{
	if (MyAbilityComp->TryActivateAbilityByClass(BaseAbility->GetClass(), true))
	{
		CachedMyAbilityComp = nullptr;
		CachedBaseAbility = nullptr;
		Cached_GlobalPotentialTargets_SkillReleased.Empty();
		// FTimerHandle TempHandle;
		//GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ThisClass::CheckAllCharacterIsDeath, 0.2f);

		UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		NavSystem->Build();
		//UpdateNavigationMesh();
	}
	else
	{
		FString TempStr = FString::Printf(TEXT("技能释放失败: 未找到技能类"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(1.5, 1.5));
	}
}

void UTacticSubsystem::CheckAllCharacterIsDeath()
{
	for (ATacticBaseCharacter*& CharactersInOrder : AllCharactersInOrder)
	{
		if (CharactersInOrder->GetBaseCharacterAttributeSet()->GetHealth() <= 0)
		{
			RemoveCharacterFromTeamByType(CharactersInOrder);
			CharactersInOrder->Destroy();
		}
	}
}

void UTacticSubsystem::SkillRelease(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility)
{
	if (!BaseAbility || !CurrentActionBaseCharacter) { return; }
	CachedMyAbilityComp = CurrentActionBaseCharacter->GetMyAbilityComp();
	if (!CachedMyAbilityComp) return;

	Cached_GlobalPotentialTargets_SkillReleased = GlobalPotentialTargets;
	CachedBaseAbility = BaseAbility;

	// 如果没有目标，显示提示信息
	if (GlobalPotentialTargets.IsEmpty())
	{
		FString TempStr = FString::Printf(TEXT("技能释放失败: 没有选择目标//todo UI display"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TempStr, true, FVector2D(1.5, 1.5));
		return;
	}
	else if (bIsAttemptToMove && bCanMove)
	{
		OnMove.Broadcast(TacticPlayerController);
		/*
		float CharacterMovementSpeed = CurrentActionBaseCharacter->GetCharacterMovement()->MaxWalkSpeed;
		float DelaySkillTime = CachedActualPathLength / CharacterMovementSpeed +
			SkillAfterMoveDelay_FloatValue;

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UTacticSubsystem::ReleaseSkillActiveAbility, AbilityComp, BaseAbility);
		FTimerHandle CustomTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(CustomTimerHandle, TimerDelegate, DelaySkillTime, false);
		*/
	}
	else
	{
		TryReleaseSkillAfterMove();
		CancelMoveAndSkill();
	}
}

void UTacticSubsystem::TryReleaseSkillAfterMove()
{
	if (CachedMyAbilityComp && CachedBaseAbility)
	{
		ReleaseSkillActiveAbility(CachedMyAbilityComp, CachedBaseAbility);
	}
}


void UTacticSubsystem::DoSkillSelectedTimer(ATacticPlayerController* TacticPlayerController,
                                            UBaseAbility* BaseAbility)
{
	OnSkillSelectedTimer.Broadcast(TacticPlayerController, BaseAbility);
}

void UTacticSubsystem::SkillSelected(ATacticPlayerController* InTacticPlayerController, UBaseAbility* InBaseAbility)
{
	if (!CurrentActionBaseCharacter) { return; }

	CancelMoveAndSkill();
	CurrentSelectedBaseAbility = InBaseAbility;

	if (!SelectedSkillTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(SelectedSkillTimerHandle);

		if (bEnableAutomaticMoveBySkill)
		{
			OnPreMove.Broadcast(InTacticPlayerController, InBaseAbility);
		}

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UTacticSubsystem::DoSkillSelectedTimer, InTacticPlayerController,
		                          InBaseAbility);
		GetWorld()->GetTimerManager().SetTimer(SelectedSkillTimerHandle, TimerDelegate, 0.02f, true);
	}
}

void UTacticSubsystem::PreSkillSelection(UBaseAbility* BaseAbility)
{
	if (!CurrentActionBaseCharacter) { return; }
	//todo bIsSkill = true;如果UI移出去preSkill。。那就是False

	FVector TempLocation = CurrentActionBaseCharacter->GetActorLocation();
	//一次性的，可以不用SetTimer。。
	if (BaseAbility && BaseAbility->GetPotentialTargets(
		this, TempLocation))
	{
	}
	CheckGlobalPotentialTargetsOutline();
}

void UTacticSubsystem::CancelMove()
{
	if (!CurrentActionBaseCharacter) { return; }
	GetWorld()->GetTimerManager().ClearTimer(PreMoveTimerTimeHandle);
	bIsAttemptToMove = false;
	if (CurrentActionBaseCharacter && !CurrentSelectedBaseAbility)
	{
		CurrentActionBaseCharacter->CloseMoveRange();
	}
}


void UTacticSubsystem::CheckHasEnoughResourceToReleseSkill(UBaseAbility* BaseAbility, bool& CanRelease)
{
	ATacticPlayerController* TacticPlayerController =
		GetWorld()->GetFirstPlayerController<ATacticPlayerController>();
	ATacticHUD* TacticHUD = Cast<ATacticHUD>(TacticPlayerController->GetHUD());

	if (CurrentActionBaseCharacter && BaseAbility &&
		TacticHUD->CheckHasEnoughResources(CurrentActionBaseCharacter, BaseAbility))
	{
		CanRelease = true;
		return;
	}
	CanRelease = false;
}

void UTacticSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnSwitchToNextCharacterAction.AddUObject(this, &ThisClass::SwitchToNextCharacterAction);
	OnRoundFinish.AddUObject(this, &ThisClass::RoundFinish);

	OnCancelMove.AddUObject(this, &ThisClass::CancelMove);
	OnCancelSkill.AddUObject(this, &ThisClass::CancelSkill);

	OnPreSkillSelection.AddUObject(this, &ThisClass::PreSkillSelection);
	OnCheckHasEnoughResourceToReleseSkill.AddUObject(this, &ThisClass::CheckHasEnoughResourceToReleseSkill);

	OnSkillSelected.AddUObject(this, &ThisClass::SkillSelected);
	OnSkillSelectedTimer.AddUObject(this, &ThisClass::SkillSelectedTimer);
	OnSkillRelease.AddUObject(this, &ThisClass::SkillRelease);

	OnPreMove.AddUObject(this, &ThisClass::PreMove);
	OnMove.AddUObject(this, &ThisClass::Move);

	OnMyMouseBeginCursorOver.AddUObject(this, &ThisClass::MyMouseBeginCursorOver);
	OnMyMouseEndCursorOver.AddUObject(this, &ThisClass::MyMouseEndCursorOver);
}

void UTacticSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UTacticSubsystem::BeginSwitchCharacter()
{
	//Spawn VisualFeedback actor
	GetVisualFeedbackActor();

	// 在这里初始化 CurrentActionCharacter
	if (AllCharactersInOrder.Num() > 0)
	{
		CurrentActionBaseCharacter = AllCharactersInOrder[0];

		// 手动触发一次委托，确保 PlayerController 能获取到正确的角色
		OnSwitchToNextCharacterAction.Broadcast();
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

void UTacticSubsystem::SetbCanMove(bool bNewValue)
{
	bCanMove = bNewValue;

	if (!CurrentActionBaseCharacter) { return; }
	bEnableAutomaticMoveBySkill = bCanMove;
	if (!bEnableAutomaticMoveBySkill && !bIsAttemptToMove && !bIsOverlappingWithCharacter)
	{
		OnCancelMove.Broadcast();
	}
}

void UTacticSubsystem::SwitchToNextCharacterAction()
{
	if (CurrentActionBaseCharacter /*重置一些属性*/)
	{
		CurrentActionBaseCharacter->SetNavCubeCompScaleToActive();
		CurrentActionBaseCharacter->HideHeadIndicator();
	}

	if (!IsValid(AllCharactersInOrder[0])) return;
	CurrentActionBaseCharacter = AllCharactersInOrder[0];
	SetbCanMove(true);

	CurrentActionBaseCharacter->ShowRotationHandleIndicator();

	CurrentActionBaseCharacter->SetNavCubeCompScaleToZero();
}

ATacticPlayerCharacter* UTacticSubsystem::TryGetActionPlayer() const
{
	if (ATacticPlayerCharacter* PlayerCharacter = Cast<ATacticPlayerCharacter>(CurrentActionBaseCharacter))
	{
		return PlayerCharacter;
	}
	return nullptr;
}

void UTacticSubsystem::PreMove_IfHasSkillRadius(ATacticPlayerController* InTacticPlayerController,
                                                float SkillPlacementRadius)
{
	if (!CurrentActionBaseCharacter) { return; }

	if (GlobalPotentialTargets.IsEmpty()&&SkillPlacementRadius>0)
	{
		if (ShowVisualFeedbackActor && !ShowVisualFeedbackActor->GetPathTracerComp()->IsPathClear())
		{
			ShowVisualFeedbackActor->GetPathTracerComp()->ClearThePath();
			ShowVisualFeedbackActor->GetPathTracerComp()->SetMarkerInvalid();
		}
		return;
	}
	
	FVector projectedLoc;

	//---角色移动范围---
	FVector PlayerLocation = CurrentActionBaseCharacter->GetActorLocation();
	float RangeToMove = CurrentActionBaseCharacter->GetBaseCharacterAttributeSet()->GetMoveRange();
	FVector MoveFinalLocation = InTacticPlayerController->CustomHoveredLocation;
	
	if (CurrentSelectedBaseAbility && CurrentSelectedBaseAbility->bIsSingleTarget &&
		CachedSingleAbilitySelectedTarget)
	{
		MoveFinalLocation = UThisProjectFunctionLibrary::FVectorZToGround(
			CachedSingleAbilitySelectedTarget->GetActorLocation());
	}
	else
	{
		MoveFinalLocation = InTacticPlayerController->CustomHoveredLocation;
	}
	if (CurrentSelectedBaseAbility && CurrentSelectedBaseAbility->bIsSingleTarget && !CachedSingleAbilitySelectedTarget)
	{
		{
			FString TempStr = FString::Printf(TEXT("Nice"));
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
		return;
	}

	if (SkillPlacementRadius)
	{
		FVector AdjustTargetLocation = UThisProjectFunctionLibrary::FVectorZToGround(MoveFinalLocation);
		FVector AdjustOwnerSourceLocation = UThisProjectFunctionLibrary::FVectorZToGround(PlayerLocation);
		const float DistanceToMouse = FVector::Dist2D(AdjustOwnerSourceLocation, AdjustTargetLocation);
		float AdjustMoveDistance = DistanceToMouse - SkillPlacementRadius;
		RangeToMove = RangeToMove < AdjustMoveDistance ? RangeToMove : AdjustMoveDistance;
	}

	UThisProjectFunctionLibrary::ClampMoveRange2D(PlayerLocation, RangeToMove, MoveFinalLocation);

	CachedPremoveFinalLocation = MoveFinalLocation;

	// 检查目标位置是否在任何角色的胶囊体内
	bIsOverlappingWithCharacter = false;
	for (ATacticBaseCharacter* CharactersInOrder : AllCharactersInOrder)
	{
		// 跳过当前行动的角色
		if (CharactersInOrder == CurrentActionBaseCharacter)
			continue;

		// 获取角色胶囊体信息
		UCapsuleComponent* CapsuleComp = CharactersInOrder->GetCapsuleComponent();
		if (!CapsuleComp)
			continue;

		float CapsuleRadius = CapsuleComp->GetScaledCapsuleRadius();
		float CapsuleHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();
		FVector CharacterLoc = CharactersInOrder->GetActorLocation();

		// 简单的2D距离检查
		float Dist2D = FVector::Dist2D(MoveFinalLocation, CharacterLoc);

		// 如果距离小于胶囊体半径，认为是重叠
		if (Dist2D < CapsuleRadius + PlusOverlappingWithCharacterEdge_FloatValue) // 添加一些边距
		{
			bIsOverlappingWithCharacter = true;
			break;
		}
	}

	// 根据重叠状态设置是否可移动
	//SetbCanMove(!bIsOverlappingWithCharacter);

	// 如果不可移动，隐藏路径
	if (bIsOverlappingWithCharacter)
	{
		if (ShowVisualFeedbackActor && !ShowVisualFeedbackActor->GetPathTracerComp()->IsPathClear())
		{
			ShowVisualFeedbackActor->GetPathTracerComp()->ClearThePath();
			ShowVisualFeedbackActor->GetPathTracerComp()->SetMarkerInvalid();
		}
		return;
	}
	else
	{
		if (ShowVisualFeedbackActor && ShowVisualFeedbackActor->GetPathTracerComp()->IsPathClear())
		{
			ShowVisualFeedbackActor->GetPathTracerComp()->RegeneratePath();
			ShowVisualFeedbackActor->GetPathTracerComp()->SetMarkerValid();
		}
	}


	// 正常情况下生成并显示路径
	if (UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), MoveFinalLocation, projectedLoc, nullptr,
	                                                     UNavigationQueryFilter::StaticClass(),
	                                                     FVector(1000, 1000, 1000)))
	{
		UNavigationPath* NaviValue = UNavigationSystemV1::FindPathToLocationSynchronously(
			GetWorld(), PlayerLocation, projectedLoc);

		if (NaviValue != nullptr)
		{
			CachedMovePoints = NaviValue->PathPoints;
			GetVisualFeedbackActor()->GetPathTracerComp()->DrawPath(CachedMovePoints);
		}
	}
}

void UTacticSubsystem::UpdateNavigationMesh()
{
	UNavigationSystemV1* NavigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	UWorld* World = GetWorld();
	World->SetNavigationSystem(nullptr);

	AWorldSettings* WorldSettings = World->GetWorldSettings();
	if (WorldSettings)
	{
		UNavigationSystemConfig* NavigationSystemConfig = WorldSettings->GetNavigationSystemConfig();
		FNavigationSystem::AddNavigationSystemToWorld(*World, FNavigationSystemRunMode::GameMode,
		                                              NavigationSystemConfig,
		                                              /*bInitializeForWorld*/true);

		UNavigationSystemV1* NavigationSystemV1Temp = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		for (TActorIterator<ANavMeshBoundsVolume> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
		{
			ANavMeshBoundsVolume* NavMeshBoundsVolume = *ActorIterator;

			NavMeshBoundsVolume->RebuildNavigationData();
			NavigationSystemV1->OnNavigationBoundsUpdated(NavMeshBoundsVolume);
		}

		for (TActorIterator<ATacticBaseCharacter> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
		{
			ATacticBaseCharacter* NavigationInvoker = *ActorIterator;
			//NavigationInvoker->enablenavigation();
		}
	}
	else
	{
		{
			FString TempStr = FString::Printf(TEXT("WorldSetting"));
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
	}
}

void UTacticSubsystem::PreMoveTimer(ATacticPlayerController* InTacticPlayerController, UBaseAbility* InBaseAbility)
{
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
	OnCancelMove.Broadcast();
	OnCancelSkill.Broadcast();
}

void UTacticSubsystem::TryReleaseSkillOrMove(ATacticPlayerController* InTacticPlayerController)
{
	if (CurrentSelectedBaseAbility)
	{
		bool CanRelease;
		OnCheckHasEnoughResourceToReleseSkill.Broadcast(CurrentSelectedBaseAbility, CanRelease);
		if (CanRelease)
		{
			OnSkillRelease.Broadcast(InTacticPlayerController, CurrentSelectedBaseAbility);
		}
	}
	else if (bIsAttemptToMove)
	{
		if (bCanMove)
		{
			OnMove.Broadcast(InTacticPlayerController);
		}
		else
		{
			{
				FString TempStr = FString::Printf(TEXT("回合已经移动"));
				if (GEngine)
					GEngine->
						AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
				UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
			}
		}
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
			CharactersInOrder->GetInteractionComp()->UnSetAsSkillTarget();
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

void UTacticSubsystem::SkillSelectedTimer(ATacticPlayerController* InTacticPlayerController,
                                          UBaseAbility* BaseAbility)
{
	if (InTacticPlayerController && BaseAbility && CurrentActionBaseCharacter)
	{
		FVector MouseLocation = InTacticPlayerController->CustomHoveredLocation;
		if (bEnableAutomaticMoveBySkill)
		{
			if (BaseAbility->GetPotentialTargets(this, MouseLocation))
			{
				OnCancelMove.Broadcast();
			}
			else
			{
				OnPreMove.Broadcast(InTacticPlayerController, BaseAbility);
				BaseAbility->GetPotentialTargets(this, MouseLocation, true);
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
