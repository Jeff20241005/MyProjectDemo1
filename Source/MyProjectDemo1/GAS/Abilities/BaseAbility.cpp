// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbility.h"

#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Actors/VisualFeedbackActor.h"
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

bool UBaseAbility::GetPotentialTargets(
	TArray<ABaseCharacter*>& OutTargets,
	ABaseCharacter* SourceCharacter, UTacticSubsystem* InTacticSubsystem, const FVector& TargetLocation)
{
	ABaseCharacter* Owner_Caster = BaseCharacterOwner ? BaseCharacterOwner : SourceCharacter;

	if (!Owner_Caster || !InTacticSubsystem)
	{
		FString TempStr = FString::Printf(TEXT("if (!Owner_Caster || !InTacticSubsystem)"));
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);

		return false;
	}
	AVisualFeedbackActor* VisualFeedbackActor = InTacticSubsystem->GetShowVisualFeedbackActor();
	if (!VisualFeedbackActor)
	{
		FString TempStr = FString::Printf(TEXT("if (!VisualFeedbackActor)"));
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);

		return false;
	}
	// 清空输出数组
	OutTargets.Empty();


	// 获取技能释放位置
	FVector AbilityCenter;
	FVector ForwardVector;

	if (bAimWithMouse)
	{
		// 如果使用鼠标指向，检查鼠标位置是否在有效范围内

		if (const float DistanceToMouse = FVector::Dist2D(Owner_Caster->GetActorLocation(), TargetLocation);
			!bInfiniteRange && DistanceToMouse > SkillPlacementRadius)
		{
			// 更新视觉反馈
			//VisualFeedbackActor->CloseVisualFeedback(this);


			// 鼠标是否没有超出施法范围
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
	VisualFeedbackActor->ShowVisualFeedback(this);

	// Get subsystem instead of game state

	// Get all characters
	TArray<ABaseCharacter*> AllCharacters = InTacticSubsystem->GetAllCharactersInOrder();

	// According to whether the skill is negative effect, get hostile or friendly characters
	TArray<ABaseCharacter*> PotentialTargets;
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
			OutTargets.AddUnique(Character);
		}
	}
	for (ABaseCharacter*
	     OutTarget : OutTargets)
	{
		{
			FString
				TempStr = FString::Printf(TEXT("%s"), *OutTarget->GetName());
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true,
				                                 FVector2D(2, 2));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
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
	return bInfiniteRange || Distance <= CircleTargetingRadius;
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
	if (!bInfiniteRange && Distance > CircleTargetingRadius)
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
