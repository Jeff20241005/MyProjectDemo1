// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbility.h"

#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"


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