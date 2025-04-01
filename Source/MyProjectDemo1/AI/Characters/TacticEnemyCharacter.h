// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
#include "TacticEnemyCharacter.generated.h"

class UBehaviorTree;

UCLASS()
class MYPROJECTDEMO1_API ATacticEnemyCharacter : public ATacticBaseCharacter
{
	GENERATED_BODY()

public:
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;
public:
	ATacticEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UBehaviorTree* GetBehaviorTree()
	{
		return BehaviorTree;
	};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UBehaviorTree* BehaviorTree;
};
