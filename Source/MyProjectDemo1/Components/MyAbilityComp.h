// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MyAbilityComp.generated.h"

enum EAttackRangeType : uint8;
class UBaseAbility;
class UBaseCharacterAttributeSet;
class ABaseCharacter;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UMyAbilityComp*, SourceASC, float,
                                               UnmitigatedDamage, float, MitigatedDamage);

// Delegate of cancel optional skill
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*Optional skill tag*/);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECTDEMO1_API UMyAbilityComp : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Called from DamageExecCalculation. Broadcasts on ReceivedDamage whenever this ASC receives damage.
	virtual void ReceiveDamage(UMyAbilityComp* SourceAbilityComp, float UnmitigatedDamage, float MitigatedDamage);

	//技能相关//
	//Init Abilities and Effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeginPlayAbilities")
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeginPlayAbilities")
	TArray<TSubclassOf<UBaseAbility>> GameplayAbilities;

	//UFUNCTION(BlueprintCallable)
	void AutoGiveAbilitiesAndEffectsAtStart();

	bool bAbilitiesInitialized = false;
	
	FDeactivatePassiveAbility OnDeactivatePassiveAbility;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	ABaseCharacter* Owner_BaseCharacter;

	UMyAbilityComp();
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	FReceivedDamageDelegate ReceivedDamage;


	//UI Stuff
public:
	TArray<ABaseCharacter*> FoundTargets;

	UPROPERTY()
	ABaseCharacter* CurrentSelectTarget;


	FTimerHandle TargetSelectTimerHandle;
};
