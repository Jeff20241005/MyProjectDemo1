// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbility.h"

#include "Components/CapsuleComponent.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Actors/VisualFeedbackActor.h"
#include "MyProjectDemo1/BlueprintFunctionLibrary/ThisProjectFunctionLibrary.h"
#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"

void UBaseAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	BaseCharacterOwner = Cast<ATacticBaseCharacter>(ActorInfo->OwnerActor);
	//	UMyAbilityComp* AbilityComp = BaseCharacterOwner->MyAbilityComp;
	//	UBaseCharacterAttributeSet* BaseCharacterAttributeSet = BaseCharacterOwner->BaseCharacterAttributeSet;

	if (!BaseCharacterOwner || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
	}
	if (!CurrentActorInfo) return;
}

void UBaseAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                              bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UBaseAbility::SelectTargetsByTeamAndProperties(UTacticSubsystem* InTacticSubsystem,
                                                    ATacticBaseCharacter* Owner_Caster,
                                                    TArray<ATacticBaseCharacter*>& PotentialTargets) const
{
	// Get all characters
	TArray<ATacticBaseCharacter*> AllCharacters = InTacticSubsystem->GetAllCharactersInOrder();

	if (bIsNegativeEffect)
	{
		// Negative effects target enemies
		PotentialTargets = InTacticSubsystem->GetAllHostileCharacters(Owner_Caster);
	}
	else
	{
		// Positive effects target allies
		PotentialTargets = InTacticSubsystem->GetAllFriendlyCharacters(Owner_Caster);
	}

	// 如果需要包含自身
	if (bIncludeSelf)
	{
		PotentialTargets.AddUnique(Owner_Caster);
	}
	else
	{
		// 确保不包含自身
		PotentialTargets.Remove(Owner_Caster);
	}
}

bool UBaseAbility::GetPotentialTargets(
	UTacticSubsystem* InTacticSubsystem, const FVector& TargetLocation, bool bAddMovingRange)
{
	TArray<ATacticBaseCharacter*>& OutTargets = InTacticSubsystem->GlobalPotentialTargets;

	ATacticBaseCharacter* Owner_Caster = BaseCharacterOwner
		                                     ? BaseCharacterOwner
		                                     : (InTacticSubsystem
			                                        ? InTacticSubsystem->CurrentActionCharacter
			                                        : nullptr);

	if (!Owner_Caster)
	{
		FString TempStr = FString::Printf(TEXT("if (!Owner_Caster || !InTacticSubsystem)"));
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);

		return false;
	}
	AVisualFeedbackActor* VisualFeedbackActor = InTacticSubsystem->GetVisualFeedbackActor();
	if (!VisualFeedbackActor)
	{
		FString TempStr = FString::Printf(TEXT("if (!VisualFeedbackActor)"));
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);

		return false;
	}

	FVector AdjustTargetLocation = UThisProjectFunctionLibrary::FVectorZToGround(TargetLocation);
	FVector AdjustOwnerSourceLocation = UThisProjectFunctionLibrary::FVectorZToGround(Owner_Caster->GetActorLocation());

	const bool bAutomaticMoveBySkill = InTacticSubsystem->bEnableAutomaticMoveBySkill;
	// 清空输出数组
	OutTargets.Empty();

	float TotalRange =
		GetSkillPlacementRadiusByAimWithMouse()
		+ (bAddMovingRange ? Owner_Caster->GetBaseCharacterAttributeSet()->GetMoveRange() : 0)
		+ (!bAimWithMouse && !bAddMovingRange && bAutomaticMoveBySkill
			   ? Owner_Caster->GetCapsuleComponent()->GetScaledCapsuleRadius()
			   : 0);

	const float DistanceToMouse = FVector::Dist2D(AdjustOwnerSourceLocation, AdjustTargetLocation);
	// 如果使用鼠标指向，检查鼠标位置是否在有效范围内
	if (!bInfiniteRange && DistanceToMouse > TotalRange && SkillRangeType != EAR_Sector)
	{
		if (bAddMovingRange || !bAutomaticMoveBySkill)
		{
			FVector ClampedLocation = AdjustTargetLocation; // 由于是引用，所以这步不能删
			UThisProjectFunctionLibrary::ClampMoveRange2D(AdjustOwnerSourceLocation, TotalRange,
			                                              ClampedLocation);
			//再次检测，让它沿着边缘走
			GetPotentialTargets(InTacticSubsystem, ClampedLocation, true);
		}
		// 鼠标不在施法范围内
		return false;
	}

	// 获取技能释放位置
	FVector ForwardVector;
	FVector AbilityCenter = AdjustTargetLocation;
	if (bSkillLookAtMouseHoveringLocation || (!bAimWithMouse && !bAddMovingRange))
	{
		// 不使用鼠标指向，以施法者为中心
		AbilityCenter = AdjustOwnerSourceLocation;

		if (bSkillLookAtMouseHoveringLocation)
		{
			// 朝向鼠标位置
			ForwardVector = (AdjustTargetLocation - AdjustOwnerSourceLocation).GetSafeNormal();
		}
		else
		{
			// 使用施法者当前朝向
			ForwardVector = Owner_Caster->GetActorForwardVector();
		}
	}

	// Get subsystem instead of game state
	TArray<ATacticBaseCharacter*> PotentialTargets;
	SelectTargetsByTeamAndProperties(InTacticSubsystem, Owner_Caster, PotentialTargets);

	// 根据技能范围类型筛选目标
	for (ATacticBaseCharacter* Character : PotentialTargets)
	{
		if (!Character) continue;

		bool bInRange = false;

		switch (SkillRangeType)
		{
		case EAR_Circle:
			//test 是否直接设置VisualFeedBackActor里面的组件更好？然后蓝图中，让可视化调整为响应的角度。
			//还是说，都一样，蓝图中设置一个跟collision一样大，形状的，都是一样的难。。主要看扇形，测试扇形吧！
			bInRange = IsCharacterInCircleRange(AbilityCenter, Character);
			break;
		case EAR_Box:
			bInRange = IsCharacterInBoxRange(AbilityCenter, ForwardVector, Character);
			break;
		case EAR_Sector:
			bInRange = IsCharacterInSectorRange(AbilityCenter, ForwardVector, Character);
			break;
		case EAR_Cross:
			bInRange = IsCharacterInCrossRange(AbilityCenter, ForwardVector, Character);
			break;
		default: ;
		}

		if (bInRange)
		{
			OutTargets.AddUnique(Character);
		}
	}
	// 更新视觉反馈
	VisualFeedbackActor->ShowVisualFeedbackBySkill(this, OutTargets);
	VisualFeedbackActor->SetActorLocation(AbilityCenter);
	if (bSkillLookAtMouseHoveringLocation)
	{
		// 将 ForwardVector 转换为旋转并应用
		FRotator NewRotation = ForwardVector.Rotation();
		// 只保留 Yaw 值，保持视觉反馈在地面上（Pitch 和 Roll 为 0）
		NewRotation.Pitch = 0.0f;
		NewRotation.Roll = 0.0f;

		// UE5中，顺时针旋转对应Yaw值，所以加半个扇形角度
		if (SkillRangeType == EAR_Sector)
		{
			NewRotation.Yaw += SectorAngle / 2.0f;
		}
		VisualFeedbackActor->SetActorRotation(NewRotation);
	}
	return true;
}

TArray<ATacticBaseCharacter*> UBaseAbility::GetTargetsInMaxRange(ATacticBaseCharacter* InOwner,
                                                                 UTacticSubsystem* InTacticSubsystem)
{
	TArray<ATacticBaseCharacter*> PotentialTargets;
	ATacticBaseCharacter* Owner_Caster = BaseCharacterOwner ? BaseCharacterOwner : InOwner;
	if (!Owner_Caster)
	{
		{
			FString TempStr = FString::Printf(TEXT("!Owner_Caster"));
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
		return PotentialTargets;
	}

	SelectTargetsByTeamAndProperties(InTacticSubsystem, Owner_Caster, PotentialTargets);
	if (PotentialTargets.IsEmpty())return PotentialTargets;

	// 根据技能范围类型筛选目标
	for (ATacticBaseCharacter* Character : PotentialTargets)
	{
		if (!Character) continue;
		bool bInRange = false;

		float SelectDistanceByType = 0;
		float DistanceByProperty = Owner_Caster->GetBaseCharacterAttributeSet()->GetMoveRange() +
			GetSkillPlacementRadiusByAimWithMouse();

		switch (SkillRangeType)
		{
		case EAR_Circle:
		case EAR_Sector:
			SelectDistanceByType = CircleTargetingRange + DistanceByProperty;
			bInRange = IsCharacterInCircleRange(Owner_Caster->GetActorLocation(), Character, SelectDistanceByType);
			break;

		case EAR_Box:
			//todo bInRange = IsCharacterInBoxRange(Owner_Caster->GetActorLocation(), ForwardVector, Character);
			break;
		case EAR_Cross:
			//todo bInRange = IsCharacterInCrossRange(Owner_Caster->GetActorLocation(), ForwardVector, Character);
			break;
		default: ;
		}
		if (!bInRange)
		{
			PotentialTargets.Remove(Character);
		}
	}
	return PotentialTargets;
}

bool UBaseAbility::IsCharacterInCircleRange(const FVector& Center, ATacticBaseCharacter* Character,
                                            float UseCustomRadius) const
{
	if (!Character) return false;

	// 计算2D距离（忽略高度差异）
	float Distance = FVector::Dist2D(Center, Character->GetActorLocation());


	// 使用的半径：如果 UseCustomRadius 大于 0，则使用它，否则使用 CircleTargetingRange
	float RadiusToUse = (UseCustomRadius > 0.0f) ? UseCustomRadius : CircleTargetingRange;

	// 检查是否在圆形范围内
	return bInfiniteRange || Distance <= RadiusToUse;
}

bool UBaseAbility::IsCharacterInBoxRange(const FVector& Center, const FVector& Forward,
                                         ATacticBaseCharacter* Character) const
{
	if (!Character) return false;

	// 获取角色位置
	FVector CharacterLocation = Character->GetActorLocation();

	// 计算角色相对于中心点的位置
	FVector RelativeLocation = CharacterLocation - Center;

	// 计算右向量（垂直于前向量）
	FVector RightVector = FVector::CrossProduct(Forward, FVector::UpVector);

	// 计算角色在前向和右向上的投影
	float ForwardProjection = FVector::DotProduct(RelativeLocation, Forward);
	float RightProjection = FVector::DotProduct(RelativeLocation, RightVector);

	// 检查是否在矩形范围内
	bool bInForwardRange = FMath::Abs(ForwardProjection) <= BoxLength / 2.0f;
	bool bInRightRange = FMath::Abs(RightProjection) <= BoxWidth / 2.0f;

	return bInfiniteRange || (bInForwardRange && bInRightRange);
}

bool UBaseAbility::IsCharacterInSectorRange(const FVector& Center, const FVector& Forward,
                                            ATacticBaseCharacter* Character) const
{
	if (!Character) return false;

	// 获取角色位置
	FVector CharacterLocation = Character->GetActorLocation();

	// 计算角色相对于中心点的位置
	FVector RelativeLocation = CharacterLocation - Center;

	// 计算2D距离
	float Distance = RelativeLocation.Size2D();

	// 检查距离
	if (!bInfiniteRange && Distance > CircleTargetingRange)
	{
		return false;
	}

	// 计算角色方向与前向量的夹角
	RelativeLocation.Normalize();
	float DotProduct = FVector::DotProduct(Forward, RelativeLocation);
	float AngleRadians = FMath::Acos(DotProduct);
	float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

	// 检查是否在扇形角度范围内
	return AngleDegrees <= SectorAngle / 2.0f;
}

bool UBaseAbility::IsCharacterInCrossRange(const FVector& Center, const FVector& Forward,
                                           ATacticBaseCharacter* Character) const
{
	if (!Character) return false;

	// 获取角色位置
	FVector CharacterLocation = Character->GetActorLocation();

	// 计算角色相对于中心点的位置
	FVector RelativeLocation = CharacterLocation - Center;

	// 计算右向量（垂直于前向量）
	FVector RightVector = FVector::CrossProduct(Forward, FVector::UpVector);

	// 计算角色在前向和右向上的投影
	float ForwardProjection = FMath::Abs(FVector::DotProduct(RelativeLocation, Forward));
	float RightProjection = FMath::Abs(FVector::DotProduct(RelativeLocation, RightVector));

	// 检查是否在十字形范围内
	bool bInForwardArm = ForwardProjection <= CrossLength && RightProjection <= CrossWidth / 2.0f;
	bool bInRightArm = RightProjection <= CrossLength && ForwardProjection <= CrossWidth / 2.0f;

	return bInfiniteRange || bInForwardArm || bInRightArm;
}
