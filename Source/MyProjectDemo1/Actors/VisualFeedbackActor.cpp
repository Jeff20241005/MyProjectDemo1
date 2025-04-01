// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualFeedbackActor.h"

#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
#include "MyProjectDemo1/Components/PathTracerComp.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"


void AVisualFeedbackActor::CancelSkill()
{
	SkillPlacementRadiusStaticMeshComponent->SetVisibility(false);
	CircleStaticMeshComponent->SetVisibility(false);
	BoxStaticMeshComponent->SetVisibility(false);
	BoxStaticMeshComponentForCross->SetVisibility(false);
}

void AVisualFeedbackActor::CancelMove()
{
	PathTracerComp->Deactivate();
}

void AVisualFeedbackActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//auto Material= SphereStaticMeshComponent /*but it may use on this*/->CreateDynamicMaterialInstance(0);
	if (CircleStaticMeshComponent)
	{
		CircleStaticMeshComponent->SetScalarParameterValueOnMaterials(MaterialName, AngleOfSphereSM);
	}
}

AVisualFeedbackActor::AVisualFeedbackActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateComponent<USceneComponent>();
	SetRootComponent(SceneComponent);

	PathTracerComp = CreateComponent<UPathTracerComp>();
	SkillPlacementRadiusStaticMeshComponent = CreateComponent<UStaticMeshComponent>();
	CircleStaticMeshComponent = CreateComponent<UStaticMeshComponent>();
	BoxStaticMeshComponent = CreateComponent<UStaticMeshComponent>();
	BoxStaticMeshComponentForCross = CreateComponent<UStaticMeshComponent>();
}


void AVisualFeedbackActor::ShowVisualFeedbackBySkill(UBaseAbility* InBaseAbility,
                                                     TArray<ATacticBaseCharacter*>& InPotentialTargets)
{
	if (InPotentialTargets.IsEmpty())
	{
		//Turn Red
	}
	if (InBaseAbility->bAimWithMouse)
	{
		ShowStaticMesh(SkillPlacementRadiusStaticMeshComponent, FVector(InBaseAbility->SkillPlacementRadius));
	}
	switch (InBaseAbility->SkillRangeType)
	{
	case EAR_Circle:
		ShowStaticMesh(CircleStaticMeshComponent, FVector(InBaseAbility->CircleTargetingRange));
		CircleStaticMeshComponent->SetScalarParameterValueOnMaterials(MaterialName, 1.f);
		break;
	case EAR_Box:
		break;
	case EAR_Sector:
		ShowStaticMesh(CircleStaticMeshComponent, FVector(InBaseAbility->CircleTargetingRange));
		CircleStaticMeshComponent->SetScalarParameterValueOnMaterials(MaterialName, InBaseAbility->SectorAngle / 360.f);
		break;
	case EAR_Cross:
		break;
	default: ;
	}
}


void AVisualFeedbackActor::ShowStaticMesh(UStaticMeshComponent* InStaticMeshComponent, FVector InScale)
{
	InStaticMeshComponent->SetVisibility(true);
	InStaticMeshComponent->SetWorldScale3D(InScale * AdjustedSkillProperty);
}

void AVisualFeedbackActor::BeginPlay()
{
	TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>();
	TacticSubsystem->OnCancelMove.AddUObject(this, &ThisClass::CancelMove);
	TacticSubsystem->OnCancelSkill.AddUObject(this, &ThisClass::CancelSkill);
	Super::BeginPlay();
}

float AVisualFeedbackActor::DrawAttackRange(ATacticBaseCharacter* BaseCharacter)
{
	float AttackRange = BaseCharacter->GetBaseCharacterAttributeSet()->GetAttackRange();
	//DrawRangeSize(AttackRange);
	return AttackRange;
}
