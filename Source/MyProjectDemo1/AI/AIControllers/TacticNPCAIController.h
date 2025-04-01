// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProjectDemo1/AI/Characters/TacticNPCCharacter.h"
#include "TacticNPCAIController.generated.h"

UCLASS()
class MYPROJECTDEMO1_API ATacticNPCAIController : public ATacticNPCCharacter
{
	GENERATED_BODY()

public:
	ATacticNPCAIController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
