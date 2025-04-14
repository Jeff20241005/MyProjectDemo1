// Fill out your copyright notice in the Description page of Project Settings.


#include "PathTracerComp.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "MyProjectDemo1/Actors/VisualFeedbackActor.h"
#include "MyProjectDemo1/FilePaths/FilePaths.h"

template <typename T>
bool UPathTracerComp::FindMyObject(T*& YourObject, const TCHAR* Path)
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

// Sets default values for this component's properties
UPathTracerComp::UPathTracerComp()
{
	PrimaryComponentTick.bCanEverTick = false;

	
	FindMyObject(ValidMarkerMaterial, *SharingBlue_Path);
	FindMyObject(InvalidMarkerMaterial, *SharingRed_Path);
	FindMyObject(CharacterSilhouetteMesh, *CharacterSilhousette_Path);
	
	// 在构造函数中设置默认值而不是在UPROPERTY声明中
	DotterAnimSpeed = -0.6f;
}


// Called when the game starts
void UPathTracerComp::DrawPath(TArray<FVector> PathPoints_p)
{
	if (PathPoints_p.Num() > 1 && bCanDraw)
	{
		PathPoints = PathPoints_p;
		GeneratePaths();

		// 确保虚影组件已初始化
		if (!EndPointMarkerMesh)
		{
			SetupEndPointMarker();
		}

		// 更新虚影位置
		UpdateEndPointMarker();
	}
}

void UPathTracerComp::GeneratePaths()
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

void UPathTracerComp::SetupParameters()
{
	SetupMaterials();
	//SetupMarkers();
}

void UPathTracerComp::MoveMarkers()
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

void UPathTracerComp::ResetSegments()
{
	// Properly clean up components that are no longer needed
	for (int32 i = 0; i < SplineMeshes.Num(); ++i)
	{
		USplineMeshComponent* SplineMesh = SplineMeshes[i];
		if (IsValid(SplineMesh))
		{
			if (i < SplineMeshAmount)
			{
				SplineMesh->SetVisibility(false);
			}
			else
			{
				// Properly destroy components we don't need anymore
				SplineMesh->DestroyComponent();
				SplineMeshes[i] = nullptr;
			}
		}
	}

	// Remove null entries from the array
	SplineMeshes.RemoveAll([](const USplineMeshComponent* Comp) { return Comp == nullptr; });

	MainMeshAmount = SplineMeshAmount = DynamicMeshAmount = 0;
	OffsetUV = 0;
}

void UPathTracerComp::GenerateRoundCornersPath()
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

void UPathTracerComp::BeginPlay()
{
	Super::BeginPlay();

	// 获取拥有者以设置附加关系
	VisualFeedbackActor = Cast<AVisualFeedbackActor>(GetOwner());
	if (!VisualFeedbackActor || !VisualFeedbackActor->GetRootComponent())
	{
		UE_LOG(LogTemp, Error, TEXT("PathTracerComponent: Invalid owner or root component"));
	}
}

void UPathTracerComp::SetupMaterials()
{
	if (IsValid(PathMaterial))
	{
		// Configure the mesh component for potential future marker use
		//MarkerMeshComponent->SetStaticMesh(PathMesh);
		//MarkerMeshComponent->SetMaterial(0, PathMaterial);

		// Create the dynamic material instance that will be used by spline meshes
		PathDynamicMaterial = UMaterialInstanceDynamic::Create(PathMaterial, this);

		// Set the material parameters
		PathDynamicMaterial->SetVectorParameterValue(DynamicMaterial_NameOfColor, DynamicMaterialColor);
		PathDynamicMaterial->SetScalarParameterValue(DynamicMaterial_NameOfOpacitgy, DynamicMaterialOpacitgy);
	}
}


void UPathTracerComp::SetupMarkers()
{
	/*
	if (IsValid(TargetPointMarkerClass))
	{
		if (!IsValid(TargetPointMarker))
		{
			
			SupporterSpline = NewObject<USplineComponent>(VisualFeedbackActor);
			SupporterSpline->RegisterComponentWithWorld(GetWorld());
			SupporterSpline->AttachToComponent(VisualFeedbackActor->GetRootComponent(),
			                                   FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SupporterSpline->SetCollisionProfileName("NoCollision");
			

			TargetPointMarker = CreateDefaultSubobject<UChildActorComponent>(TEXT("TargetPointMarker"));
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
			SupporterSpline = NewObject<USplineComponent>(VisualFeedbackActor);
			SupporterSpline->RegisterComponentWithWorld(GetWorld());
			SupporterSpline->AttachToComponent(VisualFeedbackActor,
			                                   FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SupporterSpline->SetCollisionProfileName("NoCollision");

			//CharacterMarker = CreateDefaultSubobject<UChildActorComponent>(TEXT("CharacterMarker"));
			CharacterMarker = NewObject<UChildActorComponent>();
			CharacterMarker->SetCactorClass(CharacterMarkerClass);
			CharacterMarker->RegisterComponentWithWorld(GetWorld());
			//CharacterMarker->CreateChildActor();
		}

		CharacterMarker->SetWorldScale3D(FVector(MarkerSizeScale));
		CharacterMarker->SetVisibility(true);

	}
	*/
}

float UPathTracerComp::CalculateIndent(float Length_p)
{
	return (Length_p < SegmentIndent * 2.0f) ? Length_p * 0.5f : SegmentIndent;
}

void UPathTracerComp::SetupPathSegments(FVector Start_P, FVector End_P, bool IsSegmentStraight_P,
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
		// Create a new spline mesh component
		if (VisualFeedbackActor)
		{
			tempSplineMesh = NewObject<USplineMeshComponent>(VisualFeedbackActor);
			tempSplineMesh->RegisterComponentWithWorld(GetWorld());
			tempSplineMesh->SetCollisionProfileName("NoCollision");
		}
		// Register the component with the world

		SplineMeshes.Add(tempSplineMesh);
		SplineMeshAmount++;
		SetupSplineMesh(tempSplineMesh, IsSegmentStraight_P);
	}

	tempSplineMesh->SetVisibility(true);
	tempSplineMesh->SetStartAndEnd(Start_P, StartTangent_P, End_P, EndTangent_P, true);
}

void UPathTracerComp::SetSegmentsDotted(FVector Start_P, FVector End_P, bool IsSegmentStraight_P,
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
		//todo here may cause memory leak
		if (!IsValid(SupporterSpline))
		{
			SupporterSpline = NewObject<USplineComponent>(VisualFeedbackActor);
			SupporterSpline->RegisterComponentWithWorld(GetWorld());

			SupporterSpline->SetCollisionProfileName("NoCollision");
		}

		SupporterSpline->ClearSplinePoints(false);
		SupporterSpline->AddSplinePoint(Start_P, ESplineCoordinateSpace::Local, false);
		SupporterSpline->AddSplinePoint(End_P, ESplineCoordinateSpace::Local, false);
		SupporterSpline->SetTangentAtSplinePoint(0, StartTangent_P, ESplineCoordinateSpace::Local, false);
		SupporterSpline->SetTangentAtSplinePoint(1, EndTangent_P, ESplineCoordinateSpace::Local, false);
		SupporterSpline->UpdateSpline();
		segmentLength = SupporterSpline->GetSplineLength();
	}
	if (PathDynamicMaterial)
	{
		PathDynamicMaterial->SetScalarParameterValue(DynamicMaterial_NameOfScaleUV, segmentLength / DashLineFrequency);
		PathDynamicMaterial->SetScalarParameterValue(DynamicMaterial_NameOfOffset, OffsetUV);
	}

	double clampValue;
	UKismetMathLibrary::FMod(segmentLength, DashLineFrequency, clampValue);
	OffsetUV += UKismetMathLibrary::MapRangeClamped(clampValue, 0, DashLineFrequency, 0, 1);
}

UMaterialInstanceDynamic* UPathTracerComp::GetDottedMaterialInstance()
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
	if (tempMaterialInstance)
	{
		tempMaterialInstance->SetScalarParameterValue(DynamicMaterial_NameOfOpacitgy, DotterOpacity);
		tempMaterialInstance->SetScalarParameterValue(DynamicMaterial_NameOfGapSize, DotterGapSize);
		tempMaterialInstance->SetScalarParameterValue(DynamicMaterial_NameOfAnimSpeed, DotterAnimSpeed);
		tempMaterialInstance->SetVectorParameterValue(DynamicMaterial_NameOfColor, DynamicMaterialColor);
	}

	return tempMaterialInstance;
}


void UPathTracerComp::SetupSplineMesh(USplineMeshComponent* SplineMeshComp_P, bool IsSegmentStraight_P)
{
	SplineMeshComp_P->SetMobility(EComponentMobility::Movable);
	SplineMeshComp_P->SetStaticMesh(IsSegmentStraight_P ? DefaultSplineMesh : CornerSplineMesh);
	SplineMeshComp_P->SetStartScale(PathScale);
	SplineMeshComp_P->SetEndScale(PathScale);
	SplineMeshComp_P->SetMaterial(0, PathDynamicMaterial);
}

void UPathTracerComp::InitSetup()
{
	TargetPointMarker = nullptr;
	CharacterMarker = nullptr;
}

void UPathTracerComp::Deactivate()
{
	// Clean up all created components
	for (USplineMeshComponent* SplineMesh : SplineMeshes)
	{
		if (IsValid(SplineMesh))
		{
			SplineMesh->DestroyComponent();
		}
	}
	SplineMeshes.Empty();

	if (IsValid(SupporterSpline))
	{
		SupporterSpline->DestroyComponent();
		SupporterSpline = nullptr;
	}

	bCanDraw = false;

	// 隐藏虚影
	if (EndPointMarkerMesh)
	{
		EndPointMarkerMesh->SetVisibility(false);
	}

	Super::Deactivate();
}

void UPathTracerComp::Activate(bool bReset)
{
	Super::Activate(bReset);
	bCanDraw = true;
}

// 添加虚影设置函数实现
void UPathTracerComp::SetupEndPointMarker()
{
	// 确保父组件存在
	if (!VisualFeedbackActor)
	{
		VisualFeedbackActor = Cast<AVisualFeedbackActor>(GetOwner());
		if (!VisualFeedbackActor)
		{
			return;
		}
	}

	// 创建终点虚影网格组件
	if (!EndPointMarkerMesh)
	{
		EndPointMarkerMesh = NewObject<UStaticMeshComponent>(VisualFeedbackActor, UStaticMeshComponent::StaticClass(),
		                                                     TEXT("EndPointMarkerMesh"));
		EndPointMarkerMesh->RegisterComponent();
		EndPointMarkerMesh->AttachToComponent(VisualFeedbackActor->GetRootComponent(),
		                                      FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// 设置网格和材质
		if (CharacterSilhouetteMesh)
		{
			EndPointMarkerMesh->SetStaticMesh(CharacterSilhouetteMesh);
		}

		// 默认设置为有效材质
		if (ValidMarkerMaterial)
		{
			EndPointMarkerMesh->SetMaterial(0, ValidMarkerMaterial);
		}

		// 设置为不可碰撞
		EndPointMarkerMesh->SetCollisionProfileName(TEXT("NoCollision"));

		// 初始设置为隐藏
		EndPointMarkerMesh->SetVisibility(false);
	}
}

// 状态切换函数实现
void UPathTracerComp::SetMarkerValid()
{
	if (EndPointMarkerMesh && ValidMarkerMaterial)
	{
		EndPointMarkerMesh->SetMaterial(0, ValidMarkerMaterial);
	}
}

void UPathTracerComp::SetMarkerInvalid()
{
	if (EndPointMarkerMesh && InvalidMarkerMaterial)
	{
		EndPointMarkerMesh->SetMaterial(0, InvalidMarkerMaterial);
	}
}

// 更新虚影位置和旋转
void UPathTracerComp::UpdateEndPointMarker()
{
	if (!EndPointMarkerMesh || PathPoints.Num() < 2)
	{
		return;
	}

	// 设置终点位置
	FVector EndLocation = PathPoints[PathPoints.Num() - 1];
	EndPointMarkerMesh->SetWorldLocation(EndLocation);

	// 计算朝向 - 朝向路径的前一个点
	FVector PrevPoint = PathPoints[PathPoints.Num() - 2];
	FRotator LookRotation = (EndLocation - PrevPoint).Rotation();
	EndPointMarkerMesh->SetWorldRotation(LookRotation);

	// 确保可见
	EndPointMarkerMesh->SetVisibility(true);
}

/*
// Add a cleanup method to properly destroy components when the owner is destroyed
void UPathTracerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Clean up all created components
	for (USplineMeshComponent* SplineMesh : SplineMeshes)
	{
		if (IsValid(SplineMesh))
		{
			SplineMesh->DestroyComponent();
		}
	}
	SplineMeshes.Empty();

	if (IsValid(SupporterSpline))
	{
		SupporterSpline->DestroyComponent();
		SupporterSpline = nullptr;
	}
}
*/

// 实现检查路径是否已清除的函数
bool UPathTracerComp::IsPathClear() const
{
	// 如果路径点数组为空，或只有一个点，则认为路径已清除
	return PathPoints.Num() <= 1;
}

// 实现清除路径的函数
void UPathTracerComp::ClearThePath()
{
	// 清空路径点
	PathPoints.Empty();
	
	// 清理路径的可视化元素
	ResetSegments();
	
	/*
	if (EndPointMarkerMesh)
	{
		EndPointMarkerMesh->SetVisibility(false);
	}
	*/
	
	// 可以在此处添加其他需要清理的元素
	bCanDraw = true; // 确保下次可以重新绘制
}

// 实现重新生成路径的函数
void UPathTracerComp::RegeneratePath()
{
	// 只有在有有效路径点的情况下才重新生成
	if (PathPoints.Num() > 1 && bCanDraw)
	{
		GeneratePaths();
		
		// 确保虚影组件已初始化
		if (!EndPointMarkerMesh)
		{
			SetupEndPointMarker();
		}
		
		// 更新虚影位置和材质
		UpdateEndPointMarker();
	}
}
