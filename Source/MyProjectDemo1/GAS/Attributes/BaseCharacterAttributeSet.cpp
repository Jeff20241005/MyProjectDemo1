// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Framework/Controllers/MyPlayerController.h"
#include "MyProjectDemo1/Framework/GameStates/TacticGameState.h"

UBaseCharacterAttributeSet::UBaseCharacterAttributeSet()
{
}


bool UBaseCharacterAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	//buff or debuff detection
	//check if we can trigger the effect from skill
	return Super::PreGameplayEffectExecute(Data);
}

void UBaseCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	//Reduce Action point
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source_AbilitySystemComponent = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	ABaseCharacter* TargetCharacter{nullptr};
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* TargetActor{nullptr};
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetCharacter = Cast<ABaseCharacter>(TargetActor);
	}

	//handle health change
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		float CurrentHealth = GetHealth(); // 已经修改后的值
		float Magnitude = Data.EvaluatedData.Magnitude;
		float OriginalHealth = 0.0f;
		float ActualDelta = 0.0f;

		// 根据不同的修改类型计算原始值和变化值
		switch (Data.EvaluatedData.ModifierOp)
		{
		case EGameplayModOp::AddBase:
		case EGameplayModOp::AddFinal:
			OriginalHealth = CurrentHealth - Magnitude;
			ActualDelta = Magnitude;
			break;
		case EGameplayModOp::MultiplyAdditive:
		case EGameplayModOp::MultiplyCompound:
			OriginalHealth = CurrentHealth / Magnitude;
			ActualDelta = CurrentHealth - OriginalHealth;
			break;
		case EGameplayModOp::DivideAdditive:
			OriginalHealth = CurrentHealth * Magnitude;
			ActualDelta = CurrentHealth - OriginalHealth;
			break;
		case EGameplayModOp::Override:
			break;
		default: ;
		}

		if (TargetCharacter)
		{
			// Debug输出
			FString DebugMsg = FString::Printf(TEXT("Health Changed: Original=%.1f, Current=%.1f, Delta=%.1f, Op=%d"),
			                                   OriginalHealth, CurrentHealth, ActualDelta,
			                                   static_cast<int>(Data.EvaluatedData.ModifierOp));
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, DebugMsg);

			TargetCharacter->HandleHealthChanged(ActualDelta, SourceTags);

			if (ActualDelta < 0)
			{
				//TargetCharacter->HandleDamage(ActualDelta, whyHitResult, SourceTags, BaseCharacter, SourceActor);
			}
		}
	}
}

void UBaseCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	//ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwningActor());

	// Clamp health between 0 and max health
	if (Attribute == GetHealthAttribute())
	{
		//NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	if (Attribute == GetMaxHealthAttribute())
	{
	}

	if (Attribute == GetActionValuesAttribute())
	{
		
	}
}

void UBaseCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue,
                                                     float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetActionValuesAttribute())
	{
		if (ATacticGameState* TacticGameState = Cast<ATacticGameState>(UGameplayStatics::GetGameState(GetWorld())))
		{
			TacticGameState->SortCharactersByActionValues();
			{
				FString
					TempStr = FString::Printf(TEXT("ActionValue: %f"),GetActionValues());
				if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true,
				                                              FVector2D(2, 2));
				UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
			}

		}
	}
}
