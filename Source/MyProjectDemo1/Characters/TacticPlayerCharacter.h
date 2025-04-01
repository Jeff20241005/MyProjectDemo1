// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TacticBaseCharacter.h"
#include "TacticPlayerCharacter.generated.h"


UCLASS()
class MYPROJECTDEMO1_API ATacticPlayerCharacter : public ATacticBaseCharacter
{
	GENERATED_BODY()

public:
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;
	ATacticPlayerCharacter();

	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
