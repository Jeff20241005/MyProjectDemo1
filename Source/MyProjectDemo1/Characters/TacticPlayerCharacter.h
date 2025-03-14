// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
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
	// Sets default values for this character's properties
	ATacticPlayerCharacter();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	//UActorComponent_PathTracer* PathTracer;

	void SetupCharacterDataFromBP();
	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;
};
