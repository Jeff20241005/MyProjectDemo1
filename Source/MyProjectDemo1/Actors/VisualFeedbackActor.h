// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VisualFeedbackActor.generated.h"

class UTacticSubsystem;
class UBaseAbility;
class UPathTracerComp;
class ATacticBaseCharacter;
class UStaticMeshComponent;

UCLASS()
class MYPROJECTDEMO1_API AVisualFeedbackActor : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Test")
	float TestScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Test",
		meta=(UIMax=1, UIMin=0))
	float TestAngleValueOfSphereSM = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Test")
	bool bTestIsValidColor = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FVector DefaultColorValueOfSphereSM = FVector(1, 1, 7);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FVector UnselectableColorValueOfSphereSM = FVector(7, 1, 1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
	float In_Radius = 0.99f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FName MaterialName_Angle = FName("Angle");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FName MaterialName_Color = FName("Color");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FName MaterialName_Radius = FName("Radius");

	void ShowVisualFeedbackBySkill(UBaseAbility* InBaseAbility, const FVector& AbilityCenter, bool bIsValid,
	                               ATacticBaseCharacter* OwnerCharacter, const FVector& ForwardVector, const bool& bAutomaticMoveBySkill);

	float DrawAttackRange(ATacticBaseCharacter* BaseCharacter);

	UFUNCTION(BlueprintCallable)
	USceneComponent* GetSceneComponent() const { return SceneComponent; }

	UFUNCTION(BlueprintCallable)
	UPathTracerComp* GetPathTracerComp() const { return PathTracerComp; }

	UFUNCTION(BlueprintCallable, meta=(ToolTip="鼠标引导模式下，以角色为中心的技能释放位置调整半径（单位：厘米），在此范围内可自由移动技能施放点"))
	UStaticMeshComponent* GetSkillPlacementRadiusStaticMeshComponent() const
	{
		return SkillPlacementRadiusStaticMeshComponent;
	}

	UFUNCTION(BlueprintCallable, meta=(ToolTip="技能生效的大小，影响攻击/治疗/Buff等效果的作用范围"))
	UStaticMeshComponent* GetBoxStaticMeshComponent() const { return BoxStaticMeshComponent; }

	UFUNCTION(BlueprintCallable, meta=(ToolTip="技能生效的大小，影响攻击/治疗/Buff等效果的作用范围"))
	UStaticMeshComponent* GetBoxStaticMeshComponentForSector() const { return BoxStaticMeshComponentForCross; }

	UFUNCTION(BlueprintCallable, meta=(ToolTip="技能生效的大小，影响攻击/治疗/Buff等效果的作用范围"))
	UStaticMeshComponent* GetSphereStaticMeshComponent() const { return CircleStaticMeshComponent; }

	void CancelSkill_SetAllVisibilitiesToFalse();

	void CancelMove();
	void ResetCircleStaticMeshComponentWithVariables(bool InbHasValidTargets, float InAngleValueOfSphereSM);

protected:
	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;

	virtual void OnConstruction(const FTransform& Transform) override;
	AVisualFeedbackActor();
	template <class T>
	bool FindMyObject(T*& YourObject, const TCHAR* Path);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPathTracerComp* PathTracerComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SkillPlacementRadiusStaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CircleStaticMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* BoxStaticMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* BoxStaticMeshComponentForCross;


	template <typename Comp>
	Comp* CreateComponent();

	void ShowStaticMesh(UStaticMeshComponent* InStaticMeshComponent, FVector InScale);
	virtual void BeginPlay() override;

private:
	//UPROPERTY()
	//UBaseAbility* BaseAbility;
};

//记得包含头文件在C++文件
template <typename Comp>
Comp* AVisualFeedbackActor::CreateComponent()
{
	TArray<UActorComponent*> ExistingComponents;
	GetComponents(ExistingComponents);
	int32 CompCount = 0;
	for (UActorComponent* CompInstance : ExistingComponents)
	{
		if (CompInstance->IsA<Comp>())
		{
			CompCount++;
		}
	}

	FName CompName = FName(*(Comp::StaticClass()->GetName() + TEXT("_") + FString::FromInt(CompCount)));

	// 创建组件
	Comp* TheComp = CreateDefaultSubobject<Comp>(CompName);

	if (USceneComponent* TempSceneComponentTemp = Cast<USceneComponent>(TheComp))
	{
		if (TempSceneComponentTemp)
		{
			TempSceneComponentTemp->SetupAttachment(RootComponent);
			if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(TempSceneComponentTemp))
			{
				StaticMeshComponent->SetCollisionProfileName("NoCollision");
				//StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}

	return TheComp;
}
