// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
#include "TacticNPCCharacter.generated.h"

UCLASS()
class MYPROJECTDEMO1_API ATacticNPCCharacter : public ATacticBaseCharacter
{
	GENERATED_BODY()

public:
	ATacticNPCCharacter();
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
