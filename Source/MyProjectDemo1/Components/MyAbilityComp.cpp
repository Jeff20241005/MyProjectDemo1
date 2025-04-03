// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAbilityComp.h"

#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"


UMyAbilityComp::UMyAbilityComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMyAbilityComp::BeginPlay()
{
	Super::BeginPlay();

	Owner_BaseCharacter = Cast<ABaseCharacter>(GetOwner());
	AutoGiveAbilitiesAndEffectsAtStart();
}


void UMyAbilityComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UMyAbilityComp::ReceiveDamage(UMyAbilityComp* SourceAbilityComp, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceAbilityComp, UnmitigatedDamage, MitigatedDamage);
}

void UMyAbilityComp::AutoGiveAbilitiesAndEffectsAtStart()
{
	if (!bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server
		for (const TSubclassOf<UBaseAbility>& StartupAbility : GameplayAbilities)
		{
			if (StartupAbility)
			{
				GiveAbility(FGameplayAbilitySpec(
					StartupAbility,
					1, // Level 1
					-1, //  input ID
					this // Source is the current character
				));
			}
		}
	}

	// Now apply passives
	for (const TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
	{
		if (GameplayEffect)
		{
			FGameplayEffectContextHandle EffectContext = MakeEffectContext();
			EffectContext.AddSourceObject(this);
			//这个对于玩家来说，因为玩家的等级属性比较复杂，只能是buff。
			//对于怪物和NPC，由于只是根据等级改变，永久不变，可以是做成一个excel导入
			FGameplayEffectSpecHandle NewHandle = MakeOutgoingSpec(
				GameplayEffect, Owner_BaseCharacter->GetBaseCharacterAttributeSet()->GetLevel(),
				EffectContext // Level 1 and the effect context
			);

			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle =
					ApplyGameplayEffectSpecToTarget(
						*NewHandle.Data.Get(),
						this);
			}
		}
	}
	bAbilitiesInitialized = true;
}
