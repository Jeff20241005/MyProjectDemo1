// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "TacticBaseCharacter.generated.h"

UCLASS()
class MYPROJECTDEMO1_API ATacticBaseCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	void DrawRangeSize();
	void CloseWidget();

	void Move(FVector MoveLocation);
	bool CanMove();

protected:
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;
	virtual void NotifyActorEndCursorOver() override;

	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;

	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UWidgetComponent* HealthWidgetComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UWidgetComponent* MoveRangeWidgetComp;

	ATacticBaseCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
