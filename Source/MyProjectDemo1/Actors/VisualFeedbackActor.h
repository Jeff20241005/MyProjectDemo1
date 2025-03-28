// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VisualFeedbackActor.generated.h"

class UBaseAbility;
class UPathTracerComponent;
class ABaseCharacter;
class UStaticMeshComponent;

UCLASS()
class MYPROJECTDEMO1_API AVisualFeedbackActor : public AActor
{
	GENERATED_BODY()
	//AbilityTargetingRange;
	//SkillPlacementRadius;
public:
	//UFUNCTION(BlueprintCallable)
	//UBaseAbility* GetBaseAbility() const { return BaseAbility; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JFSetting)
	float AngleOfSphereSM = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FName MaterialName = FName("Angle");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	float AdjustedSkillProperty = 1.f / 100.f * 2.f;

	void ShowVisualFeedbackBySkill(UBaseAbility* InBaseAbility, TArray<ABaseCharacter*>& InPotentialTargets);

	float DrawAttackRange(ABaseCharacter* BaseCharacter);

	UFUNCTION(BlueprintCallable)
	USceneComponent* GetSceneComponent() const { return SceneComponent; }

	UFUNCTION(BlueprintCallable)
	UPathTracerComponent* GetPathTracerComponent() const { return PathTracerComponent; }

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

	UFUNCTION(BlueprintCallable)
	void CancelVisualFeedback(UBaseAbility* InBaseAbility);

	void CancelMove();
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	AVisualFeedbackActor();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UPathTracerComponent* PathTracerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UStaticMeshComponent* SkillPlacementRadiusStaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JFSetting)
	UStaticMeshComponent* CircleStaticMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UStaticMeshComponent* BoxStaticMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
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
				StaticMeshComponent->SetVisibility(false);
				//StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}

	return TheComp;
}
