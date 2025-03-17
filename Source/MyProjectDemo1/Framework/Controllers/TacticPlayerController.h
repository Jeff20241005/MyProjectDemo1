// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerController.h"
#include "TacticPlayerController.generated.h"

class UTacticSubsystem;
class ATacticGameState;
class ABaseCharacter; // Forward declaration instead of including the header

DECLARE_MULTICAST_DELEGATE(FViewportStateChange)

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API ATacticPlayerController : public AMyPlayerController
{
protected:
	void CharacterFocus();
	 virtual void OnTabClick() override;
	virtual void OnRightMouseButtonDown() override;
	UFUNCTION()
	void FreeViewportChange();
	
	FViewportStateChange OnFreeViewport;
	FViewportStateChange OnCharacterFocus;
	bool bIsFreeViewport = false;

public:
	
protected:
	template <class Comp>
	Comp* CreateComponent();

	ATacticPlayerController();
	void Tick(float DeltaSeconds) override;

	void SwitchCharacterAction(ABaseCharacter* BaseCharacter);
	
	void BeginPlay() override;

	virtual void PlayerInputMovement(float Value, EAxis::Type Axis) override;
	virtual void OnLeftMouseButtonDown() override;

	UPROPERTY()
	ATacticGameState* TacticGameState;
	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;


	GENERATED_BODY()
};

template <typename Comp>
Comp* ATacticPlayerController::CreateComponent()
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
