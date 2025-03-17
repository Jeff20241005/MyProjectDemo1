// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowVisualFeedbackActor.h"

#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Components/PathTracerComponent.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"


AShowVisualFeedbackActor::AShowVisualFeedbackActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PathTracerComponent = CreateComponent<UPathTracerComponent>();
}


void AShowVisualFeedbackActor::OnMouseCursorOver()
{
	/*	if (!InteractionComp->bIsSelected)
	{
		HealthComponent->SetVisibility(true);
		if (!InteractionComp->bIsMoved && !bIsAttackRange)
		{
			//---显示移动范围等
			DrawMoveRange();
		}
	}
	*/
}

void AShowVisualFeedbackActor::BeginPlay()
{

	Super::BeginPlay();
}

void AShowVisualFeedbackActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


float AShowVisualFeedbackActor::DrawAttackRange(ABaseCharacter* BaseCharacter)
{
	float AttackRange = BaseCharacter->GetBaseCharacterAttributeSet()->GetAttackRange();
	//DrawRangeSize(AttackRange);

	return AttackRange;
}
