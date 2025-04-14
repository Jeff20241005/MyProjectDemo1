// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyProjectDemo1/Actors/VisualFeedbackActor.h"
#include "PathTracerComp.generated.h"

class USplineComponent;
class USplineMeshComponent;
class APathMarkerBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECTDEMO1_API UPathTracerComp : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPathTracerComp();

protected:
	template <class T>
	bool FindMyObject(T*& YourObject, const TCHAR* Path);
	UPROPERTY()
	TArray<FVector> PathPoints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Setup")
	bool bLimitMovement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Setup")
	bool bTranslucentPath;

	UPROPERTY(EditAnywhere, Category="Setup")
	bool bChangePathType = true;

	UPROPERTY(EditDefaultsOnly, Category="Setup|Material")
	UMaterialInterface* PathMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Setup|Material")
	UStaticMesh* PathMesh; //---------------------------------TEST---------------------

	UPROPERTY(EditDefaultsOnly, Category="Setup|Material")
	FName DynamicMaterial_NameOfColor = "Color";

	UPROPERTY(EditDefaultsOnly, Category="Setup|Material")
	FLinearColor DynamicMaterialColor = FLinearColor::Yellow;

	UPROPERTY(EditDefaultsOnly, Category="Setup|Material")
	FName DynamicMaterial_NameOfOpacitgy = "Opacity";

	UPROPERTY(EditDefaultsOnly, Category="Setup|Material")
	float DynamicMaterialOpacitgy = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Setup|Material")
	float SegmentIndent = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category="Setup|Material")
	float TangentsLength = 2.0f;

	//---修改线路材质的尺寸UV
	UPROPERTY(EditDefaultsOnly, Category="Setup|Spline")
	FName DynamicMaterial_NameOfScaleUV = "ScaleUV";

	UPROPERTY(EditDefaultsOnly, Category="Setup|Spline")
	FName DynamicMaterial_NameOfOffset = "Offset";

	//---直线和拐角的路线Mesh
	UPROPERTY(EditDefaultsOnly, Category="Setup|Spline")
	UStaticMesh* DefaultSplineMesh;

	UPROPERTY(EditDefaultsOnly, Category="Setup|Spline")
	UStaticMesh* CornerSplineMesh;

	//---修改线路尺寸
	UPROPERTY(EditDefaultsOnly, Category="Setup|Spline")
	FVector2D PathScale = FVector2D(5.0f, 5.0f);

	//---修改闪烁频率
	UPROPERTY(EditDefaultsOnly, Category="Setup|Dynamic", meta=(ClampMin="0.1"))
	float DashLineFrequency = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category="Setup|Dynamic")
	UMaterialInterface* DotterDynamicMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Setup|Dynamic")
	float DotterOpacity = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Setup|Dynamic")
	FName DynamicMaterial_NameOfGapSize = "GapSize";

	UPROPERTY(EditDefaultsOnly, Category="Setup|Dynamic")
	float DotterGapSize = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category="Setup|Dynamic")
	FName DynamicMaterial_NameOfAnimSpeed = "AnimSpeed";

	UPROPERTY(EditDefaultsOnly, Category="Setup|Dynamic", meta=(ClampMin="-10.0", ClampMax="10.0"))
	float DotterAnimSpeed;
	UPROPERTY()
	AVisualFeedbackActor* VisualFeedbackActor;
	bool bCanDraw = true;


	UPROPERTY(EditDefaultsOnly, Category="Setup|Marker")
	float MarkerSizeScale = 1.0f;
	void SetupMarkers();
	
public:
	//---寻路系统图标类
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	USkeletalMesh* CharacterMarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	USkeletalMesh* TargetPointMarkerClass;

	UPROPERTY()
	UStaticMeshComponent* EndPointMarkerMesh;
	
	// 虚影材质 - 有效状态(红色)
	UPROPERTY(EditDefaultsOnly, Category="Setup|Marker")
	UMaterialInterface* ValidMarkerMaterial;
	
	// 虚影材质 - 无效状态(蓝色)
	UPROPERTY(EditDefaultsOnly, Category="Setup|Marker")
	UMaterialInterface* InvalidMarkerMaterial;
	
	// 虚影角色网格
	UPROPERTY(EditDefaultsOnly, Category="Setup|Marker")
	UStaticMesh* CharacterSilhouetteMesh;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetLastPathPoint() { return PathPoints[PathPoints.Num() - 1]; }

	UFUNCTION(BlueprintCallable)
	void DrawPath(TArray<FVector> PathPoints_p);

	//---初始化一些设定，防止崩溃---------
	UFUNCTION(BlueprintCallable)
	void InitSetup();

	virtual void Deactivate() override;
	virtual void Activate(bool bReset = false) override;

	// 设置虚影为有效状态
	UFUNCTION(BlueprintCallable, Category="Path|Marker")
	void SetMarkerValid();
	
	// 设置虚影为无效状态
	UFUNCTION(BlueprintCallable, Category="Path|Marker")
	void SetMarkerInvalid();
	
	// 更新虚影位置和旋转
	UFUNCTION()
	void UpdateEndPointMarker();

	// 检查路径是否已清除
	UFUNCTION(BlueprintCallable, Category = "Path")
	bool IsPathClear() const;
	
	// 清除路径
	UFUNCTION(BlueprintCallable, Category = "Path")
	void ClearThePath();
	
	// 重新生成路径
	UFUNCTION(BlueprintCallable, Category = "Path")
	void RegeneratePath();

private:
	bool bIsSetupParameter = false;

	//---Setup Path Materials Property
	UPROPERTY()
	USplineComponent* SupporterSpline;

	UPROPERTY()
	UMaterialInstanceDynamic* PathDynamicMaterial;
	UPROPERTY()
	UStaticMeshComponent* MarkerMeshComponent;
	TArray<USplineMeshComponent*> SplineMeshes;
	int32 MainMeshAmount;
	int32 SplineMeshAmount;
	int32 DynamicMeshAmount;
	float OffsetUV;

	//---Setup Path Marker Property
	UPROPERTY()
	UChildActorComponent* CharacterMarker;
	UPROPERTY()
	UChildActorComponent* TargetPointMarker;


	void GeneratePaths();

	//---Setup base parameters
	void SetupParameters();
	void MoveMarkers();
	void ResetSegments();
	void GenerateRoundCornersPath();
	virtual void BeginPlay() override;
	//---Setup Parameters Functions
	void SetupMaterials();
	//---Setup Path Segment
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DottedLineDynamicMaterials;

	void SetupPathSegments(FVector Start_P, FVector End_P, bool IsSegmentStraight_P, FVector StartTangent_P,
	                       FVector EndTangent_P);
	void SetSegmentsDotted(FVector Start_P, FVector End_P, bool IsSegmentStraight_P, FVector StartTangent_P,
	                       FVector EndTangent_P);
	void SetupSplineMesh(USplineMeshComponent* SplineMeshComp_P, bool IsSegmentStraight_P);
	float CalculateIndent(float Length_p);
	UMaterialInstanceDynamic* GetDottedMaterialInstance();

	// 初始化虚影组件
	void SetupEndPointMarker();
};
