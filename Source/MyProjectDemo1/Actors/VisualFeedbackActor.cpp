// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualFeedbackActor.h"

#include "MyProjectDemo1/BlueprintFunctionLibrary/ThisProjectFunctionLibrary.h"
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
	GetPathTracerComp()->Deactivate();
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
                                                     bool bIsValid, ATacticBaseCharacter* OwnerCharacter,
                                                     const FVector& ForwardVector, const bool& bAutomaticMoveBySkill)
{
	FVector AdjustAbilityCenter= AbilityCenter;
	UThisProjectFunctionLibrary::FVectorZToGround(AdjustAbilityCenter);
	SetActorLocation(AdjustAbilityCenter);
	
	FVector TempVector = OwnerCharacter->GetActorLocation();
	SkillPlacementRadiusStaticMeshComponent->
		SetWorldLocation(UThisProjectFunctionLibrary::FVectorZToGround(TempVector));

	//	if (InBaseAbility->bAimWithMouse)
	float Range = (InBaseAbility->GetSkillPlacementRadiusByAimWithMouse() +
		(bAutomaticMoveBySkill?OwnerCharacter->GetBaseCharacterAttributeSet()->GetMoveRange():0)+
		InBaseAbility->CircleOrSectorTargetingRange);
	ShowStaticMesh(SkillPlacementRadiusStaticMeshComponent, FVector(Range));

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
	//FVector Location = InStaticMeshComponent->GetComponentLocation();
	//UThisProjectFunctionLibrary::FVectorZToGround(Location);
	InStaticMeshComponent->SetWorldScale3D(FVector(InScale * CircleRangeAdjustmentValue_FloatValue));
	//InStaticMeshComponent->SetWorldLocation(Location);
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
