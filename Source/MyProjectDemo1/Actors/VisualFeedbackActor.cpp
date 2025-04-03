// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualFeedbackActor.h"

#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
#include "MyProjectDemo1/Components/PathTracerComp.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"


void AVisualFeedbackActor::CancelSkill_SetAllVisibilitiesToFalse()
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

void AVisualFeedbackActor::ResetCircleStaticMeshComponentWithVariables(bool InbHasValidTargets,
                                                                       float InAngleValueOfSphereSM)
{
	if (CircleStaticMeshComponent)
	{
		CircleStaticMeshComponent->SetScalarParameterValueOnMaterials(MaterialName_Angle, InAngleValueOfSphereSM);

		// 根据是否有有效目标设置颜色
		if (InbHasValidTargets)
		{
			CircleStaticMeshComponent->SetVectorParameterValueOnMaterials(
				MaterialName_Color, DefaultColorValueOfSphereSM);
		}
		else
		{
			CircleStaticMeshComponent->SetVectorParameterValueOnMaterials(
				MaterialName_Color, UnselectableColorValueOfSphereSM);
		}
	}
}

void AVisualFeedbackActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//auto Material= SphereStaticMeshComponent /*but it may use on this*/->CreateDynamicMaterialInstance(0);

	CircleStaticMeshComponent->SetWorldScale3D(FVector(TestScale));
	ResetCircleStaticMeshComponentWithVariables(bTestIsValidColor, TestAngleValueOfSphereSM);
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
                                                     TArray<ATacticBaseCharacter*>& InPotentialTargets,
                                                     bool bIsValid)
{
	if (InBaseAbility->bAimWithMouse)
	{
		ShowStaticMesh(SkillPlacementRadiusStaticMeshComponent,
		               FVector(InBaseAbility->GetSkillPlacementRadiusByAimWithMouse()));
	}

	switch (InBaseAbility->SkillRangeType)
	{
	case EAR_Circle:
		ShowStaticMesh(CircleStaticMeshComponent, FVector(InBaseAbility->CircleOrSectorTargetingRange));
		ResetCircleStaticMeshComponentWithVariables(bIsValid, 1.f);
		break;
	case EAR_Box:
		break;
	case EAR_Sector:
		ShowStaticMesh(CircleStaticMeshComponent, FVector(InBaseAbility->CircleOrSectorTargetingRange));
		ResetCircleStaticMeshComponentWithVariables(bIsValid, InBaseAbility->SectorAngle / 360.f);
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
	Super::BeginPlay();
	TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>();
	TacticSubsystem->OnCancelMove.AddUObject(this, &ThisClass::CancelMove);
	TacticSubsystem->OnCancelSkill.AddUObject(this, &ThisClass::CancelSkill_SetAllVisibilitiesToFalse);

	CancelSkill_SetAllVisibilitiesToFalse();
}

float AVisualFeedbackActor::DrawAttackRange(ATacticBaseCharacter* BaseCharacter)
{
	float AttackRange = BaseCharacter->GetBaseCharacterAttributeSet()->GetAttackRange();
	//DrawRangeSize(AttackRange);
	return AttackRange;
}
