// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticBaseCharacter.h"

#include "Components/WidgetComponent.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"


void ATacticBaseCharacter::Move(FVector MoveLocation)
{
	if (GetMyAbilityComp() && CanMove())
	{
		float MoveRange = BaseCharacterAttributeSet->GetMoveRange();
		BaseAIController->MoveToLocationWithPathFinding(
			MoveLocation, false, MoveRange);

		// 修改ActionValues属性值
		BaseCharacterAttributeSet->SetActionValues(BaseCharacterAttributeSet->GetActionValues() - 1.0f);
	}
}

bool ATacticBaseCharacter::CanMove()
{
	return BaseCharacterAttributeSet->GetActionValues() >= 1;
}

void ATacticBaseCharacter::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	// 添加空指针检查
	if (TacticSubsystem)
	{
		TacticSubsystem->OnMyMouseBeginCursorOver.Broadcast(this);
	}
}

void ATacticBaseCharacter::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	// 添加空指针检查
	if (TacticSubsystem && TacticSubsystem->OnMyMouseEndCursorOver.IsBound())
	{
		TacticSubsystem->OnMyMouseEndCursorOver.Broadcast(this);
	}
}


void ATacticBaseCharacter::DrawRangeSize()
{
	MoveRangeWidgetComp->SetDrawSize(FVector2D(GetBaseCharacterAttributeSet()->GetMoveRange()));
	MoveRangeWidgetComp->SetVisibility(true);
}


void ATacticBaseCharacter::CloseWidget()
{
	MoveRangeWidgetComp->SetVisibility(false);
}

ATacticBaseCharacter::ATacticBaseCharacter()
{
	TeamComp = CreateComponent<UTeamComp>();

	//PrimaryActorTick.bCanEverTick = true;

	HealthWidgetComp = CreateComponent<UWidgetComponent>();
	HealthWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthWidgetComp->SetVisibility(false);


	MoveRangeWidgetComp = CreateComponent<UWidgetComponent>();
	MoveRangeWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MoveRangeWidgetComp->SetVisibility(false);
	MoveRangeWidgetComp->SetRelativeLocation(FVector(0, 0, -85));
	MoveRangeWidgetComp->SetRelativeRotation(FRotator(90, 0, 0));
}

void ATacticBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>();
}

void ATacticBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
