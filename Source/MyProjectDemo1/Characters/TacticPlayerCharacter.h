// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "TacticPlayerCharacter.generated.h"

class UTacticInteractionComp;
class ATacticPlayerController;
class UPathTracerComponent;
class UWidgetComponent;

UCLASS()
class MYPROJECTDEMO1_API ATacticPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()

public:
	ATacticPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	//UActorComponent_PathTracer* PathTracer;
};
