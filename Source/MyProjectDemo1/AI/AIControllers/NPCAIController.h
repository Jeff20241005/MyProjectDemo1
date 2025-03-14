// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProjectDemo1/AI/Characters/NPCCharacter.h"
#include "NPCAIController.generated.h"

UCLASS()
class MYPROJECTDEMO1_API ANPCAIController : public ANPCCharacter
{
	GENERATED_BODY()

public:
	ANPCAIController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
