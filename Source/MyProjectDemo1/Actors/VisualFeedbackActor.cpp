// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualFeedbackActor.h"

#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
#include "MyProjectDemo1/Components/PathTracerComp.h"
#include "MyProjectDemo1/FilePaths/DefaultPropertyValue.h"
#include "MyProjectDemo1/FilePaths/FilePaths.h"
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

	SkillPlacementRadiusStaticMeshComponent->SetScalarParameterValueOnMaterials(MaterialName_Radius, In_Radius);

	SkillPlacementRadiusStaticMeshComponent->SetRelativeLocation(FVector(0, 0, -0.3f));
}

template <typename T>
bool AVisualFeedbackActor::FindMyObject(T*& YourObject, const TCHAR* Path)
{
	if (ConstructorHelpers::FObjectFinder<T> ObjectFinder(Path); ObjectFinder.Succeeded())
	{
		YourObject = ObjectFinder.Object;
		return true;
	}
	else
	{
		FString TempStr = FString::Printf(TEXT("Not Found! : %s"), Path);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		return false;
	}
}

AVisualFeedbackActor::AVisualFeedbackActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateComponent<USceneComponent>();
	SetRootComponent(SceneComponent);

	SkillPlacementRadiusStaticMeshComponent = CreateComponent<UStaticMeshComponent>();
	CircleStaticMeshComponent = CreateComponent<UStaticMeshComponent>();

	UStaticMesh* ShowUpSM;
	if (FindMyObject(ShowUpSM, *MoveRangeCircleStaticMeshVisualFeedback_Path))
	{
		SkillPlacementRadiusStaticMeshComponent->SetStaticMesh(ShowUpSM);
		CircleStaticMeshComponent->SetStaticMesh(ShowUpSM);
	}

	BoxStaticMeshComponent = CreateComponent<UStaticMeshComponent>();
	BoxStaticMeshComponentForCross = CreateComponent<UStaticMeshComponent>();

	PathTracerComp = CreateComponent<UPathTracerComp>();
}

void AVisualFeedbackActor::ShowVisualFeedbackBySkill(UBaseAbility* InBaseAbility,
                                                     const FVector& AbilityCenter,
                                                     bool bIsValid, const FVector& SourceCharacterLocation,
                                                     const FVector& ForwardVector)
{
	SetActorLocation(AbilityCenter);
	GetSkillPlacementRadiusStaticMeshComponent()->SetWorldLocation(SourceCharacterLocation);

	//	if (InBaseAbility->bAimWithMouse)

	ShowStaticMesh(SkillPlacementRadiusStaticMeshComponent,
	               FVector(InBaseAbility->GetSkillPlacementRadiusByAimWithMouse()));

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


	// 将 ForwardVector 转换为旋转并应用
		if (InBaseAbility->bSkillLookAtMouseHoveringLocation)
		{
			
		}

		FRotator NewRotation = ForwardVector.Rotation();
	// 只保留 Yaw 值，保持视觉反馈在地面上（Pitch 和 Roll 为 0）
		NewRotation.Pitch = 0.0f;
		NewRotation.Roll = 0.0f;
		NewRotation.Yaw -= InBaseAbility->SectorAngle / 2.0f;

		CircleStaticMeshComponent->SetWorldRotation(NewRotation);

		break;
	case EAR_Cross:
		break;
	default: ;
	}
}

void AVisualFeedbackActor::ShowStaticMesh(UStaticMeshComponent* InStaticMeshComponent, FVector InScale)
{
	InStaticMeshComponent->SetVisibility(true);
	InStaticMeshComponent->SetWorldScale3D(InScale * CircleRangeAdjustmentValue_FloatValue);
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
