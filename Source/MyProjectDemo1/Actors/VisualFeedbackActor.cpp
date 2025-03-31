// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualFeedbackActor.h"

#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Components/PathTracerComponent.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"


void AVisualFeedbackActor::CancelSkill()
{
	SkillPlacementRadiusStaticMeshComponent->SetVisibility(false);
	CircleStaticMeshComponent->SetVisibility(false);
	BoxStaticMeshComponent->SetVisibility(false);
	BoxStaticMeshComponentForCross->SetVisibility(false);
}

void AVisualFeedbackActor::CancelMove()
{
	PathTracerComponent->Deactivate();
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
	
	PathTracerComponent = CreateComponent<UPathTracerComponent>();
	SkillPlacementRadiusStaticMeshComponent = CreateComponent<UStaticMeshComponent>();
	CircleStaticMeshComponent = CreateComponent<UStaticMeshComponent>();
	BoxStaticMeshComponent = CreateComponent<UStaticMeshComponent>();
	BoxStaticMeshComponentForCross = CreateComponent<UStaticMeshComponent>();
}


void AVisualFeedbackActor::ShowVisualFeedbackBySkill(UBaseAbility* InBaseAbility, TArray<ABaseCharacter*>& InPotentialTargets)
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
		break;
	case EAR_Box:
		break;
	case EAR_Sector:
		ShowStaticMesh(CircleStaticMeshComponent, FVector(InBaseAbility->CircleTargetingRange));
		{
			FString TempStr = FString::Printf(TEXT("?fwo1ei"));
			if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
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

float AVisualFeedbackActor::DrawAttackRange(ABaseCharacter* BaseCharacter)
{
	float AttackRange = BaseCharacter->GetBaseCharacterAttributeSet()->GetAttackRange();
	//DrawRangeSize(AttackRange);
	return AttackRange;
}
