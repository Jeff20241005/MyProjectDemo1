// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"

UBaseCharacterAttributeSet::UBaseCharacterAttributeSet()
{
}

bool UBaseCharacterAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	//buff or debuff detection
	//check if we can trigger the effect from skill
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		PreHealth = GetHealth();
	}
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
	//float Magnitude = Data.EvaluatedData.Magnitude;

	//handle health change
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		float CurrentHealth = GetHealth();
		float ActualDelta = CurrentHealth - PreHealth;

		if (TargetCharacter)
		{
			FString DebugMsg = FString::Printf(TEXT("Health Changed: Original=%.1f, Current=%.1f, Delta=%.1f, Op=%d"),
			                                   PreHealth, CurrentHealth, ActualDelta,
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

ABaseCharacter* UBaseCharacterAttributeSet::GetOwnerBaseCharacter()
{
	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ABaseCharacter>(GetOwningActor());
	}
	return OwnerCharacter;
}

void UBaseCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

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

	if (Attribute == GetActionValuesAttribute() && GetOwnerBaseCharacter()->GetMyAbilityComp()->bAbilitiesInitialized)
	{
		if (UTacticSubsystem* TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>())
		{
			TacticSubsystem->SortCharactersByActionValues();
		}
	}

	// Clamp health between 0 and max health
	if (Attribute == GetHealthAttribute())
	{
		if (ATacticBaseCharacter* TacticBaseCharacter = Cast<ATacticBaseCharacter>(GetOwnerBaseCharacter()))
		{
			TacticBaseCharacter->SetWidgetHealth(NewValue, GetMaxHealth());
			if (NewValue <= 0)
			{
				GetOwnerBaseCharacter()->Destroy();
			}
		}
		//NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}
