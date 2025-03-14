// Fill out your copyright notice in the Description page of Project Settings.


#include "PassiveAbility.h"

#include "MyProjectDemo1/BlueprintFunctionLibrary/BFL_AbilitySystem.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"

void UPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	//传输到AbilityComp
	if (UMyAbilityComp* MyAbilityComp = Cast<UMyAbilityComp>(
		UBFL_AbilitySystem::GetMyAbilityCompFromActor(GetAvatarActorFromActorInfo())))
	{
		//绑定技能取消回调
		MyAbilityComp->OnDeactivatePassiveAbility.AddUObject(
			this, &UPassiveAbility::OnDeactivatePassiveAbility);
	}
}

void UPassiveAbility::OnDeactivatePassiveAbility(const FGameplayTag& AbilityTag)
{
	if (GetAssetTags().HasTagExact(AbilityTag))
	{
		//todo test automatically remove the delegate of this function within EndAbility?
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
	}
}

void UPassiveAbility::BeginDestroy()
{
	Super::BeginDestroy();
	{
		FString
			TempStr = FString::Printf(TEXT("Destroyed Ability"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}
}
