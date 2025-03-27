// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualFeedbackActor.h"

#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Components/PathTracerComponent.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"


void AVisualFeedbackActor::CloseVisualFeedback(UBaseAbility* InBaseAbility)
{
}

AVisualFeedbackActor::AVisualFeedbackActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PathTracerComponent = CreateComponent<UPathTracerComponent>();

	SkillPlacementRadiusStaticMeshComponent = CreateComponent<UStaticMeshComponent>();
	SkillPlacementRadiusStaticMeshComponent->SetVisibility(false);


	SphereStaticMeshComponent = CreateComponent<UStaticMeshComponent>();


	BoxStaticMeshComponent = CreateComponent<UStaticMeshComponent>();
}


void AVisualFeedbackActor::ShowVisualFeedback(UBaseAbility* InBaseAbility)
{
	BaseAbility = InBaseAbility;

	switch (BaseAbility->SkillRangeType)
	{
	case EAR_Circle:

		break;
	case EAR_Box:

		break;
	case EAR_Sector:

		break;
	case EAR_Cross:

		break;
	default: ;
	}
}

void AVisualFeedbackActor::OnMouseCursorOver()
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

void AVisualFeedbackActor::BeginPlay()
{
	Super::BeginPlay();
}

void AVisualFeedbackActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AVisualFeedbackActor::DrawAttackRange(ABaseCharacter* BaseCharacter)
{
	float AttackRange = BaseCharacter->GetBaseCharacterAttributeSet()->GetAttackRange();
	//DrawRangeSize(AttackRange);

	return AttackRange;
}
