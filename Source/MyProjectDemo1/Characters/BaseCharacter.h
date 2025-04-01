// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UTacticSubsystem;
class UPathTracerComp;
class UTeamComp;
class UWidgetComponent;
class ABasePlayerController;
class ABaseAIController;
class UBaseAbility;
class UGameplayEffect;
class UMyAbilityComp;
class UBaseCharacterAttributeSet;
class UInteractionComp;
class ABaseCharacter;


UCLASS()
class MYPROJECTDEMO1_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags,
	                  ABaseCharacter* InstigatorCharacter, AActor* DamageCauser);
	void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	ABaseAIController* BaseAIController;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMyAbilityComp* GetMyAbilityComp() const { return MyAbilityComp; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UBaseCharacterAttributeSet* GetBaseCharacterAttributeSet() const { return BaseCharacterAttributeSet; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInteractionComp* GetInteractionComp() const { return InteractionComp; }


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	USpringArmComponent* SpringArmComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JFSetting)
	UCameraComponent* CameraComponent;

	//custom settings
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UMyAbilityComp* MyAbilityComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UBaseCharacterAttributeSet* BaseCharacterAttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UInteractionComp* InteractionComp;


	//todo move to parent
	// default Setting默认//
	// we may want to modify functions (TakeDamage->OnDamaged) to 
	UFUNCTION(BlueprintNativeEvent)
	void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags,
	               ABaseCharacter* InstigatorCharacter, AActor* DamageCauser);

	//todo move to parent
	/**
	 * Called when health is changed, either from healing or from being damaged
	 * For damage this is called in addition to OnDamaged/OnKilled
	 *
	 * @param DeltaValue Change in health value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintNativeEvent)
	void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	//todo move to parent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	ABasePlayerController* MyPlayerController;

	//	UDataTable* AttributeDT;
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;

protected:

	ABaseCharacter();
	virtual void BeginPlay() override;

	template <class Comp>
	Comp* CreateComponent();
};

template <typename Comp>
Comp* ABaseCharacter::CreateComponent()
{
	TArray<UActorComponent*> ExistingComponents;
	GetComponents(ExistingComponents);
	int32 CompCount = 0;
	for (UActorComponent* CompInstance : ExistingComponents)
	{
		if (CompInstance->IsA<Comp>())
		{
			CompCount++;
		}
	}

	FName CompName = FName(*(Comp::StaticClass()->GetName() + TEXT("_") + FString::FromInt(CompCount)));

	Comp* TheComp = CreateDefaultSubobject<Comp>(CompName);

	if (USceneComponent* TempSTempSceneComponent = Cast<USceneComponent>(TheComp))
	{
		if (TempSTempSceneComponent)
		{
			TempSTempSceneComponent->SetupAttachment(RootComponent);
		}
	}
	return TheComp;
}
