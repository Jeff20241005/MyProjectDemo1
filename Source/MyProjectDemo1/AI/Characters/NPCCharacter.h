// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIBaseCharacter.h"
#include "NPCCharacter.generated.h"

UCLASS()
class MYPROJECTDEMO1_API ANPCCharacter : public AAIBaseCharacter
{
	GENERATED_BODY()

public:
	ANPCCharacter();
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
