// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbility.h"

#include "Components/CapsuleComponent.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Actors/VisualFeedbackActor.h"
#include "MyProjectDemo1/BlueprintFunctionLibrary/ThisProjectFunctionLibrary.h"
#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/GAS/Effects/BaseEffect.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"

void UBaseAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	BaseCharacterOwner = Cast<ATacticBaseCharacter>(ActorInfo->OwnerActor);

	
	if (!BaseCharacterOwner || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
		return;
	}

	if (!CurrentActorInfo) return;

	// 获取TacticSubsystem获取目标列表
	UTacticSubsystem* TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>();
	if (!TacticSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("ActivateAbility: Failed to get TacticSubsystem"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
		return;
	}

	// 获取目标列表
	const TArray<ATacticBaseCharacter*>& Targets = TacticSubsystem->Cached_GlobalPotentialTargets_SkillReleased;
	if (Targets.IsEmpty())
	{
		FString TempStr = FString::Printf(TEXT("ActivateAbility: No targets available"));
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
		return;
	}

	// 获取源角色的AbilityComp
	UMyAbilityComp* SourceAbilityComp = BaseCharacterOwner->GetMyAbilityComp();
	if (!SourceAbilityComp)
	{
		UE_LOG(LogTemp, Error, TEXT("ActivateAbility: Source character has no AbilityComp"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
		return;
	}

	bool bAppliedAtLeastOne = false;


	//todo do the effect during the animation, we need to set a timer.. add a float value? let's see
	//can we grab 
	// 对每个目标应用效果
	for (ATacticBaseCharacter* Target : Targets)
	{
		if (!Target || !EffectForTargets) continue;

		UMyAbilityComp* TargetAbilityComp = Target->GetMyAbilityComp();
		if (!TargetAbilityComp) continue;

		// 创建效果上下文
		FGameplayEffectContextHandle EffectContext = SourceAbilityComp->MakeEffectContext();
		EffectContext.AddSourceObject(BaseCharacterOwner);

		// 创建效果规格
		FGameplayEffectSpecHandle SpecHandle = SourceAbilityComp->MakeOutgoingSpec(
			EffectForTargets,
			/*todo level test*/GetAbilityLevel(),
			EffectContext
		);

		if (SpecHandle.IsValid())
		{
			// 在这里可以设置自定义参数，比如根据技能等级、角色属性等调整效果强度
			// SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage"), 50.0f);

			// 应用效果到目标
			FActiveGameplayEffectHandle ActiveGEHandle = TargetAbilityComp->ApplyGameplayEffectSpecToSelf(
				*SpecHandle.Data.Get());

			if (ActiveGEHandle.IsValid())
			{
				bAppliedAtLeastOne = true;
				UE_LOG(LogTemp, Log, TEXT("Successfully applied effect to %s"), *Target->GetName());
			}
		}
	}

	if (!bAppliedAtLeastOne)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActivateAbility: Failed to apply effect to any target"));
	}

	// 成功应用效果后，结束技能
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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
	if (bIsPotion)
	{
		PotentialTargets = InTacticSubsystem->GetAllCharactersInOrder();
		return;
	}

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
	//最后。。还是需要和TacticSubsystem交互。
	//总的来说，避免不了和他们耦合，不可能0耦合，但至少分工明确了
	TArray<ATacticBaseCharacter*>& OutTargets = InTacticSubsystem->GlobalPotentialTargets;

	ATacticBaseCharacter* Owner_Caster = BaseCharacterOwner
		                                     ? BaseCharacterOwner
		                                     : (InTacticSubsystem
			                                        ? InTacticSubsystem->CurrentActionBaseCharacter
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
			ForwardVector = Owner_Caster->GetActorLocation();
		}
	}

	// 如果是单目标选择，通知PlayerController更改射线检测参数

	// Get subsystem instead of game state
	TEnumAsByte<EAttackRangeType> EffectType = bIsSingleTarget
		                                           ? TEnumAsByte<EAttackRangeType>(EAR_Circle)
		                                           : SkillRangeType;


	TArray<ATacticBaseCharacter*> PotentialTargets;
	SelectTargetsByTeamAndProperties(InTacticSubsystem, Owner_Caster, PotentialTargets);

	// 根据技能范围类型筛选目标
	for (ATacticBaseCharacter* CheckCharacter : PotentialTargets)
	{
		if (!CheckCharacter) continue;

		bool bInRange = false;

		// 获取角色的位置
		FVector CheckCharacterLocation = UThisProjectFunctionLibrary::FVectorZToGround(
			CheckCharacter->GetActorLocation());

		/*if (!IsLocationInCircleRange(AdjustOwnerSourceLocation, CheckCharacterLocation, TotalRange))
		{
			continue;
		}*/

		// 获取角色胶囊体半径，用于调整检测距离
		float CapsuleRadius = CheckCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();

		// 使用辅助函数计算调整后的检测位置
		FVector CalcLocaitonWithCapsuleComp = CalculateAdjustedCheckLocation(
			CheckCharacterLocation, AbilityCenter, CapsuleRadius);

		switch (EffectType)
		{
		case EAR_Circle:
			if (bIsSingleTarget)
			{
				//InTacticSubsystem->CachedSingleAbilitySelectedTarget = nullptr;
			}
			if (bIsSingleTarget && InTacticSubsystem->HoveredTacticBaseCharacter == CheckCharacter)
			{
				InTacticSubsystem->CachedSingleAbilitySelectedTarget = nullptr;
				bInRange = IsLocationInCircleRange(AdjustOwnerSourceLocation, CheckCharacterLocation,
				                                   GetSkillPlacementRadiusByAimWithMouse()
				                                   + (InTacticSubsystem->bCanMove
					                                      ? Owner_Caster->GetBaseCharacterAttributeSet()->GetMoveRange()
					                                      : 0));
				if (bInRange)
				{
					//todo bug 一直执行这个。一旦执行InTacticSubsystem->SingleAbilitySelectedTarget = CheckCharacter;
					InTacticSubsystem->CachedSingleAbilitySelectedTarget = CheckCharacter;
					AbilityCenter = CheckCharacterLocation;
				}
				else
				{
					InTacticSubsystem->CachedSingleAbilitySelectedTarget = nullptr;
				}
			}
			else
			{
				bInRange = IsLocationInCircleRange(AbilityCenter, CalcLocaitonWithCapsuleComp,
				                                   GetCircleOrSectorTargetingRangeBybIsSingleTarget());
			}
			break;
		case EAR_Box:
			bInRange = IsLocationInBoxRange(AbilityCenter, ForwardVector, CalcLocaitonWithCapsuleComp);
			break;
		case EAR_Sector:
			bInRange = IsLocationInSectorRange(AbilityCenter, ForwardVector, CalcLocaitonWithCapsuleComp);
			break;
		case EAR_Cross:
			bInRange = IsLocationInCrossRange(AbilityCenter, ForwardVector, CalcLocaitonWithCapsuleComp);
			break;
		default: ;
		}


		if (bInRange)
		{
			OutTargets.AddUnique(CheckCharacter);
			if (bIsSingleTarget)
			{
				break;
			}
		}
	}


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

	VisualFeedbackActor->ShowVisualFeedbackBySkill(this, AbilityCenter, !OutTargets.IsEmpty(),
	                                               Owner_Caster, ForwardVector, bAutomaticMoveBySkill);
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

		float DistanceByProperty = Owner_Caster->GetBaseCharacterAttributeSet()->GetMoveRange() +
			GetSkillPlacementRadiusByAimWithMouse();

		// 获取角色的位置
		FVector CharacterLocation = Character->GetActorLocation();

		// 获取角色胶囊体半径，用于调整检测距离
		float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();

		// 使用辅助函数计算调整后的检测位置
		FVector AdjustedCheckLocation = CalculateAdjustedCheckLocation(CharacterLocation,
		                                                               Owner_Caster->GetActorLocation(), CapsuleRadius);

		float SelectDistanceByType = 0;
		switch (SkillRangeType)
		{
		case EAR_Circle:
		case EAR_Sector:
			SelectDistanceByType = GetCircleOrSectorTargetingRangeBybIsSingleTarget() + DistanceByProperty;
			bInRange = IsLocationInCircleRange(Owner_Caster->GetActorLocation(), AdjustedCheckLocation,
			                                   SelectDistanceByType);
			break;

		case EAR_Box:
			// 使用Owner_Caster的前向量
			bInRange = IsLocationInBoxRange(Owner_Caster->GetActorLocation(),
			                                Owner_Caster->GetActorForwardVector(),
			                                AdjustedCheckLocation);
			break;
		case EAR_Cross:
			// 使用Owner_Caster的前向量
			bInRange = IsLocationInCrossRange(Owner_Caster->GetActorLocation(),
			                                  Owner_Caster->GetActorForwardVector(),
			                                  AdjustedCheckLocation);
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

bool UBaseAbility::IsLocationInCircleRange(const FVector& Center, const FVector& CheckedLocation,
                                           float UseCustomRadius) const
{
	// 计算2D距离（忽略高度差异）
	float Distance = FVector::Dist2D(Center, CheckedLocation);

	// 使用的半径：如果 UseCustomRadius 大于 0，则使用它，否则使用 CircleTargetingRange
	float RadiusToUse = (UseCustomRadius > 0.0f) ? UseCustomRadius : GetCircleOrSectorTargetingRangeBybIsSingleTarget();

	// 检查是否在圆形范围内
	return bInfiniteRange || Distance <= RadiusToUse;
}

bool UBaseAbility::IsLocationInBoxRange(const FVector& Center, const FVector& Forward,
                                        const FVector& CheckedLocation) const
{
	// 计算位置相对于中心点的向量
	FVector RelativeLocation = CheckedLocation - Center;

	// 计算右向量（垂直于前向量）
	FVector RightVector = FVector::CrossProduct(Forward, FVector::UpVector);

	// 计算位置在前向和右向上的投影
	float ForwardProjection = FVector::DotProduct(RelativeLocation, Forward);
	float RightProjection = FVector::DotProduct(RelativeLocation, RightVector);

	// 检查是否在矩形范围内
	bool bInForwardRange = FMath::Abs(ForwardProjection) <= BoxLength / 2.0f;
	bool bInRightRange = FMath::Abs(RightProjection) <= BoxWidth / 2.0f;

	return bInfiniteRange || (bInForwardRange && bInRightRange);
}

bool UBaseAbility::IsLocationInSectorRange(const FVector& Center, const FVector& Forward,
                                           const FVector& CheckedLocation) const
{
	// 计算位置相对于中心点的向量
	FVector RelativeLocation = CheckedLocation - Center;

	// 计算2D距离
	float Distance = RelativeLocation.Size2D();

	// 检查距离
	if (!bInfiniteRange && Distance > GetCircleOrSectorTargetingRangeBybIsSingleTarget())
	{
		return false;
	}

	// 计算位置方向与前向量的夹角
	FVector NormalizedLocation = RelativeLocation;
	NormalizedLocation.Normalize();
	float DotProduct = FVector::DotProduct(Forward, NormalizedLocation);

	// 限制点积值在[-1,1]范围内，防止精度问题导致acos出错
	DotProduct = FMath::Clamp(DotProduct, -1.0f, 1.0f);

	float AngleRadians = FMath::Acos(DotProduct);
	float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

	// 检查是否在扇形角度范围内
	return AngleDegrees <= SectorAngle / 2.0f;
}

bool UBaseAbility::IsLocationInCrossRange(const FVector& Center, const FVector& Forward,
                                          const FVector& CheckedLocation) const
{
	// 计算位置相对于中心点的向量
	FVector RelativeLocation = CheckedLocation - Center;

	// 计算右向量（垂直于前向量）
	FVector RightVector = FVector::CrossProduct(Forward, FVector::UpVector);

	// 计算位置在前向和右向上的投影
	float ForwardProjection = FMath::Abs(FVector::DotProduct(RelativeLocation, Forward));
	float RightProjection = FMath::Abs(FVector::DotProduct(RelativeLocation, RightVector));

	// 检查是否在十字形范围内
	bool bInForwardArm = ForwardProjection <= CrossLength && RightProjection <= CrossWidth / 2.0f;
	bool bInRightArm = RightProjection <= CrossLength && ForwardProjection <= CrossWidth / 2.0f;

	return bInfiniteRange || bInForwardArm || bInRightArm;
}

FVector UBaseAbility::CalculateAdjustedCheckLocation(const FVector& CharacterLocation, const FVector& AbilityCenter,
                                                     float CapsuleRadius) const
{
	FVector TempVector = CharacterLocation - AbilityCenter;
	// 计算从源位置到角色位置的方向
	FVector DirectionToCharacter = TempVector.GetSafeNormal();

	//如果距离太小，则使用本来的角色自身坐标，这样倒是保证了，比如扇形后方，距离近的目标，不会被标记
	if (TempVector.Length() < CapsuleRadius)
	{
		return CharacterLocation;
	}

	// 调整检查位置，考虑目标的胶囊体大小
	return CharacterLocation - DirectionToCharacter * CapsuleRadius;
}
