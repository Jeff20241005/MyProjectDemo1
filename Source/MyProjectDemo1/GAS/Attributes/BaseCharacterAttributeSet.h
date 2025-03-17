// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseCharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#define GET_NUMERIC_VALUE(AttributeSetName, PropertyName) \
AttributeSetName->Get##PropertyName##Attribute().GetNumericValue(AttributeSetName)

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UBaseCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBaseCharacterAttributeSet();


	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(EditAnywhere, Category = "Base")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, MaxHealth)
	UPROPERTY(EditAnywhere)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ThisClass, Health)

	UPROPERTY(EditAnywhere, Category = "Base")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, MaxMana)
	UPROPERTY(EditAnywhere)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(ThisClass, Mana)
	
	UPROPERTY(EditAnywhere, Category = "Base")
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, MaxEnergy)
	UPROPERTY(EditAnywhere)
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(ThisClass, Energy)
	
	UPROPERTY(EditAnywhere, Category = "Base")
	FGameplayAttributeData ActionRecovery;
	ATTRIBUTE_ACCESSORS(UBaseCharacterAttributeSet, ActionRecovery)
	UPROPERTY(EditAnywhere)
	FGameplayAttributeData ActionValues;
	ATTRIBUTE_ACCESSORS(ThisClass, ActionValues)

	UPROPERTY(EditAnywhere, Category = "Base")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(ThisClass, Damage)

	
	UPROPERTY(EditAnywhere, Category = "Base")
	FGameplayAttributeData DamageReduction;
	ATTRIBUTE_ACCESSORS(ThisClass, DamageReduction)
	
	UPROPERTY(EditAnywhere, Category = "Base")
	FGameplayAttributeData Defence;
	ATTRIBUTE_ACCESSORS(ThisClass, Defence)

	UPROPERTY(EditAnywhere, Category = "Base")
	FGameplayAttributeData AttackRange;
	ATTRIBUTE_ACCESSORS(ThisClass, AttackRange)

	UPROPERTY(EditAnywhere, Category = "Base")
	FGameplayAttributeData MoveRange;
	ATTRIBUTE_ACCESSORS(ThisClass, MoveRange)
};
