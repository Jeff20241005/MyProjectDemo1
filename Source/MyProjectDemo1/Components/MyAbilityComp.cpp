// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAbilityComp.h"

#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Framework/GameStates/TacticGameState.h"
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

			FGameplayEffectSpecHandle NewHandle = MakeOutgoingSpec(
				GameplayEffect, 1, EffectContext // Level 1 and the effect context
			);

			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle =
					ApplyGameplayEffectSpecToTarget(
						*NewHandle.Data.Get(),
						this
					);
			}
		}
	}
	bAbilitiesInitialized = true;
}