// Fill out your copyright notice in the Description page of Project Settings.


#include "PathTracerComponent.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UPathTracerComponent::UPathTracerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	PathMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PathMeshComponent"));
}


// Called when the game starts
void UPathTracerComponent::DrawPath(TArray<FVector> PathPoints_p)
{
	if (PathPoints_p.Num() > 1)
	{
		PathPoints = PathPoints_p;
		GeneratePaths();
	}
}

void UPathTracerComponent::GeneratePaths()
{
	if (!bIsSetupParameter)
	{
		bIsSetupParameter = true;
		SetupParameters();
	}

	MoveMarkers();
	ResetSegments();
	GenerateRoundCornersPath();
}

void UPathTracerComponent::SetupParameters()
{
	SetupMaterials();
	//SetupMarkers();
}

void UPathTracerComponent::MoveMarkers()
{
	FRotator TempRotator;

	if (IsValid(TargetPointMarker))
	{
		TargetPointMarker->SetWorldLocation(GetLastPathPoint());
		TempRotator = FRotator::ZeroRotator;

		if (PathPoints.Num() > 1)
		{
			TempRotator = UKismetMathLibrary::FindLookAtRotation(PathPoints[PathPoints.Num() - 2], GetLastPathPoint());
			TempRotator = FRotator(0, TempRotator.Yaw + 90.0f, 0);
		}

		TargetPointMarker->SetWorldRotation(TempRotator);
	}

	if (IsValid(CharacterMarker))
	{
		CharacterMarker->SetWorldLocation(PathPoints[0]);
		TempRotator = FRotator::ZeroRotator;

		if (PathPoints.Num() > 1)
		{
			TempRotator = UKismetMathLibrary::FindLookAtRotation(PathPoints[0], PathPoints[1]);
			TempRotator = FRotator(0, TempRotator.Yaw + 90.0f, 0);
		}

		CharacterMarker->SetWorldRotation(TempRotator);
	}
}

void UPathTracerComponent::ResetSegments()
{
	if (!SplineMeshes.IsEmpty() && SplineMeshes.IsValidIndex(0))
	{
		for (USplineMeshComponent* SplineMesh : SplineMeshes)
		{
			if (IsValid(SplineMesh))
			{
				//SplineMesh->GetVisibleFlag()
				SplineMesh->SetVisibility(false);
			}
		}
	}
	MainMeshAmount = SplineMeshAmount = DynamicMeshAmount = 0;
	OffsetUV = 0;
}

void UPathTracerComponent::GenerateRoundCornersPath()
{
	int32 lastIndex = PathPoints.Num() - 1;
	int32 curIndex;

	for (int i = 0; i < lastIndex; i++)
	{
		curIndex = i;

		FVector curPoint = PathPoints[curIndex];
		FVector nextPoint = PathPoints[curIndex + 1];
		FVector nextNextPoint = PathPoints[curIndex + ((curIndex == lastIndex) ? 2 : 1)];

		FVector tempFVector = nextPoint - curPoint;
		UKismetMathLibrary::Vector_Normalize(tempFVector);
		FVector curSegment = tempFVector;

		tempFVector = nextNextPoint - nextPoint;
		UKismetMathLibrary::Vector_Normalize(tempFVector);
		FVector nextSegment = tempFVector;

		float curSegmentLength = curSegment.Length();
		float nextSegmentLength = nextSegment.Length();

		FVector SegmentsStart = curPoint + (curSegment * (curIndex == 0 ? 0 : CalculateIndent(curSegmentLength)));
		FVector SegmentsEnd = curPoint + (curSegment * (curSegmentLength - (curIndex == lastIndex
			                                                                    ? 0
			                                                                    : CalculateIndent(curSegmentLength))));
		SetupPathSegments(SegmentsStart, SegmentsEnd, true, curSegment, curSegment);

		if (curIndex != lastIndex)
		{
			SegmentsStart = nextPoint + (nextSegment * CalculateIndent(nextSegmentLength));
			SegmentsEnd = curPoint + (curSegment * (curSegmentLength - CalculateIndent(curSegmentLength)));
			FVector SegmentStartTangent = nextSegment * SegmentIndent * TangentsLength * -1;
			FVector SegmentEndTangent = curSegment * SegmentIndent * TangentsLength * -1;

			SetupPathSegments(SegmentsStart, SegmentsEnd, false, SegmentStartTangent, SegmentEndTangent);
		}
	}
}

void UPathTracerComponent::SetupMaterials()
{
	if (IsValid(PathMaterial))
	{
		if (!IsValid(PathMeshComponent)) return;
		PathMeshComponent->SetStaticMesh(PathMesh);
		PathMeshComponent->SetMaterial(0, PathMaterial);
		PathDynamicMaterial = PathMeshComponent->CreateDynamicMaterialInstance(0);

		PathDynamicMaterial->SetVectorParameterValue(DynamicMaterial_NameOfColor, DynamicMaterialColor);
		PathDynamicMaterial->SetScalarParameterValue(DynamicMaterial_NameOfOpacitgy, DynamicMaterialOpacitgy);
	}
}

/*
void UActorComponent_PathTracer::SetupMarkers()
{
	if (IsValid(TargetPointMarkerClass))
	{
		if (!IsValid(TargetPointMarker))
		{
			//TargetPointMarker = CreateDefaultSubobject<UChildActorComponent>(TEXT("TargetPointMarker"));
			TargetPointMarker = NewObject<UChildActorComponent>();
			TargetPointMarker->SetChildActorClass(TargetPointMarkerClass);
			TargetPointMarker->RegisterComponentWithWorld(GetWorld());
			//TargetPointMarker->CreateChildActor();
		}

		TargetPointMarker->SetWorldScale3D(FVector(MarkerSizeScale));
		TargetPointMarker->SetVisibility(true);
	}

	if (IsValid(CharacterMarkerClass))
	{
		if (!IsValid(CharacterMarker))
		{
			//CharacterMarker = CreateDefaultSubobject<UChildActorComponent>(TEXT("CharacterMarker"));
			CharacterMarker = NewObject<UChildActorComponent>();
			CharacterMarker->SetCactorClass(CharacterMarkerClass);
			CharacterMarker->RegisterComponentWithWorld(GetWorld());
			//CharacterMarker->CreateChildActor();
		}

		CharacterMarker->SetWorldScale3D(FVector(MarkerSizeScale));
		CharacterMarker->SetVisibility(true);

	}
}
*/
float UPathTracerComponent::CalculateIndent(float Length_p)
{
	return (Length_p < SegmentIndent * 2.0f) ? Length_p * 0.5f : SegmentIndent;
}

void UPathTracerComponent::SetupPathSegments(FVector Start_P, FVector End_P, bool IsSegmentStraight_P,
                                             FVector StartTangent_P, FVector EndTangent_P)
{
	if (bChangePathType) SetSegmentsDotted(Start_P, End_P, IsSegmentStraight_P, StartTangent_P, EndTangent_P);
	USplineMeshComponent* tempSplineMesh;
	if (SplineMeshes.Num() > SplineMeshAmount)
	{
		SplineMeshAmount++;
		tempSplineMesh = SplineMeshes[SplineMeshAmount - 1];
	}
	else
	{
		int MeshIndex = SplineMeshes.Num();
		//tempSplineMesh = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SplineMesh"));
		tempSplineMesh = NewObject<USplineMeshComponent>();
		tempSplineMesh->RegisterComponentWithWorld(GetWorld());

		SplineMeshes.Add(tempSplineMesh);
		SplineMeshAmount++;
		SetupSplineMesh(tempSplineMesh, IsSegmentStraight_P);
	}

	tempSplineMesh->SetVisibility(true);
	tempSplineMesh->SetStartAndEnd(Start_P, StartTangent_P, End_P, EndTangent_P, true);
}

void UPathTracerComponent::SetSegmentsDotted(FVector Start_P, FVector End_P, bool IsSegmentStraight_P,
                                             FVector StartTangent_P, FVector EndTangent_P)
{
	PathDynamicMaterial = GetDottedMaterialInstance();


	float segmentLength;

	if (IsSegmentStraight_P)
	{
		segmentLength = FVector(Start_P - End_P).Length();
	}
	else
	{
		if (!IsValid(SupporterSpline))
		{
			//---Instead of AddSplineComponent----------------------------------------
			//SupporterSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
			SupporterSpline = NewObject<USplineComponent>();
			//SupporterSpline->RegisterComponentWithWorld(GetWorld());
		}

		SupporterSpline->SetLocationAtSplinePoint(0, Start_P, ESplineCoordinateSpace::Local, false);
		SupporterSpline->SetLocationAtSplinePoint(1, End_P, ESplineCoordinateSpace::Local, false);
		SupporterSpline->SetLocationAtSplinePoint(0, StartTangent_P, ESplineCoordinateSpace::Local, false);
		SupporterSpline->SetLocationAtSplinePoint(1, EndTangent_P, ESplineCoordinateSpace::Local, false);
		segmentLength = SupporterSpline->GetSplineLength();
	}

	PathDynamicMaterial->SetScalarParameterValue(DynamicMaterial_NameOfScaleUV, segmentLength / DashLineFrequency);
	PathDynamicMaterial->SetScalarParameterValue(DynamicMaterial_NameOfOffset, OffsetUV);

	double clampValue;
	UKismetMathLibrary::FMod(segmentLength, DashLineFrequency, clampValue);
	OffsetUV += UKismetMathLibrary::MapRangeClamped(clampValue, 0, DashLineFrequency, 0, 1);
}

UMaterialInstanceDynamic* UPathTracerComponent::GetDottedMaterialInstance()
{
	if (DottedLineDynamicMaterials.Num() > DynamicMeshAmount)
	{
		DynamicMeshAmount++;
		return DottedLineDynamicMaterials[DynamicMeshAmount - 1];
	}

	//if (DotterDynamicMaterial) DotterDynamicMaterial = CreateDefaultSubobject<UMaterialInstanceDynamic>(TEXT("DynamicMaterial"));

	UMaterialInstanceDynamic* tempMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
		GetWorld(), DotterDynamicMaterial);
	DottedLineDynamicMaterials.Add(tempMaterialInstance);
	DynamicMeshAmount++;

	tempMaterialInstance->SetScalarParameterValue(DynamicMaterial_NameOfOpacitgy, DotterOpacity);
	tempMaterialInstance->SetScalarParameterValue(DynamicMaterial_NameOfGapSize, DotterGapSize);
	tempMaterialInstance->SetScalarParameterValue(DynamicMaterial_NameOfAnimSpeed, DotterAnimSpeed);
	tempMaterialInstance->SetVectorParameterValue(DynamicMaterial_NameOfColor, DynamicMaterialColor);

	return tempMaterialInstance;
}


void UPathTracerComponent::SetupSplineMesh(USplineMeshComponent* SplineMeshComp_P, bool IsSegmentStraight_P)
{
	SplineMeshComp_P->SetMobility(EComponentMobility::Movable);
	SplineMeshComp_P->SetStaticMesh(IsSegmentStraight_P ? DefaultSplineMesh : CornerSplineMesh);
	SplineMeshComp_P->SetStartScale(PathScale);
	SplineMeshComp_P->SetEndScale(PathScale);
	SplineMeshComp_P->SetMaterial(0, PathDynamicMaterial);
}

void UPathTracerComponent::InitSetup()
{
	TargetPointMarker = nullptr;
	CharacterMarker = nullptr;
}
