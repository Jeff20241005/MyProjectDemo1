// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbility.h"

#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Actors/ShowVisualFeedbackActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"

void UBaseAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	BaseCharacterOwner = Cast<ABaseCharacter>(ActorInfo->OwnerActor);
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

bool UBaseAbility::GetPotentialTargets(AShowVisualFeedbackActor* VisualFeedbackActor,
                                       const FVector& TargetLocation,
                                       TArray<ABaseCharacter*>& OutTargets,
                                       ABaseCharacter* SourceCharacter)
{
	// 清空输出数组
	OutTargets.Empty();

	// 获取施法者
	if (!BaseCharacterOwner && !SourceCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetPotentialTargets: No source character provided!"));
		return false;
	}

	ABaseCharacter* Owner_Caster = BaseCharacterOwner ? BaseCharacterOwner : SourceCharacter;

	// 获取技能释放位置
	FVector AbilityCenter;
	FVector ForwardVector;

	if (bAimWithMouse)
	{
		// 如果使用鼠标指向，检查鼠标位置是否在有效范围内
		float DistanceToMouse = FVector::Dist2D(Owner_Caster->GetActorLocation(), TargetLocation);

		if (!bInfiniteRange && DistanceToMouse > SkillPlacementRadius)
		{
			// 鼠标超出范围，返回false
			return false;
		}

		AbilityCenter = TargetLocation;
		//ForwardVector = (MouseLocation - Owner_Caster->GetActorLocation()).GetSafeNormal();
	}
	else
	{
		// 不使用鼠标指向，以施法者为中心
		AbilityCenter = Owner_Caster->GetActorLocation();

		if (bSkillLookAtMouseHoveringLocation)
		{
			// 朝向鼠标位置
			ForwardVector = (TargetLocation - Owner_Caster->GetActorLocation()).GetSafeNormal();
		}
		else
		{
			// 使用施法者当前朝向
			ForwardVector = Owner_Caster->GetActorForwardVector();
		}
	}

	// 更新视觉反馈
	if (VisualFeedbackActor)
	{
		// 根据技能类型设置视觉反馈
		switch (SkillRangeType)
		{
		case EAR_Circle:
			
			//todo
			// 设置圆形范围显示
			// 这里需要根据你的VisualFeedbackActor实现来调用相应方法
			break;
		case EAR_Box:
			// 设置矩形范围显示
			break;
		case EAR_Sector:
			// 设置扇形范围显示
			break;
		case EAR_Cross:
			// 设置十字形范围显示
			break;
		default: ;
		}
	}

	// Get subsystem instead of game state
	if (!TacticSubsystem)
	{
		TacticSubsystem = VisualFeedbackActor->GetWorld()->GetGameInstance()->GetSubsystem<UTacticSubsystem>();
	}

	// Get all characters
	TArray<ABaseCharacter*> AllCharacters = TacticSubsystem->GetAllCharactersInOrder();

	// According to whether the skill is negative effect, get hostile or friendly characters
	TArray<ABaseCharacter*> PotentialTargets;
	if (bIsNegativeEffect)
	{
		// Negative effects target enemies
		PotentialTargets = TacticSubsystem->GetAllHostileCharacters(Owner_Caster);
	}
	else
	{
		// Positive effects target allies
		PotentialTargets = TacticSubsystem->GetAllFriendlyCharacters(Owner_Caster);
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

	// 根据技能范围类型筛选目标
	for (ABaseCharacter* Character : PotentialTargets)
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
			OutTargets.Add(Character);
		}
	}

	return true;
}

bool UBaseAbility::IsCharacterInCircleRange(const FVector& Center, ABaseCharacter* Character) const
{
	if (!Character) return false;

	// 计算2D距离（忽略高度差异）
	float Distance = FVector::Dist2D(Center, Character->GetActorLocation());

	// 检查是否在圆形范围内
	return bInfiniteRange || Distance <= AbilityTargetingRange;
}

bool UBaseAbility::IsCharacterInBoxRange(const FVector& Center, const FVector& Forward, ABaseCharacter* Character) const
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
                                            ABaseCharacter* Character) const
{
	if (!Character) return false;

	// 获取角色位置
	FVector CharacterLocation = Character->GetActorLocation();

	// 计算角色相对于中心点的位置
	FVector RelativeLocation = CharacterLocation - Center;

	// 计算2D距离
	float Distance = RelativeLocation.Size2D();

	// 检查距离
	if (!bInfiniteRange && Distance > AbilityTargetingRange)
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
                                           ABaseCharacter* Character) const
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
