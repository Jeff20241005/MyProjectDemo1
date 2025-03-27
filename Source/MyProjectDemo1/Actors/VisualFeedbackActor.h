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

	void ShowVisualFeedback(UBaseAbility* InBaseAbility);
	
	void OnMouseCursorOver();

	float DrawAttackRange(ABaseCharacter* BaseCharacter);
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
	UStaticMeshComponent* GetBoxStaticMeshComponentForSector() const { return BoxStaticMeshComponentForSector; }

	UFUNCTION(BlueprintCallable, meta=(ToolTip="技能生效的大小，影响攻击/治疗/Buff等效果的作用范围"))
	UStaticMeshComponent* GetSphereStaticMeshComponent() const { return SphereStaticMeshComponent; }

	UFUNCTION(BlueprintCallable)
	void CloseVisualFeedback(UBaseAbility* InBaseAbility);

	//todo，之后想办法生成一个扇形的Collision
protected:
	AVisualFeedbackActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UPathTracerComponent* PathTracerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UStaticMeshComponent* SkillPlacementRadiusStaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JFSetting)
	UStaticMeshComponent* SphereStaticMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UStaticMeshComponent* BoxStaticMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UStaticMeshComponent* BoxStaticMeshComponentForSector;


	template <class Comp>
	Comp* CreateComponent();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	UBaseAbility* BaseAbility;
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

	Comp* TheComp = CreateDefaultSubobject<Comp>(CompName);
	if (USceneComponent* TempSceneComponentTemp = Cast<USceneComponent>(TheComp))
	{
		if (TempSceneComponentTemp)
		{
			TempSceneComponentTemp->SetupAttachment(RootComponent);
		}
	}

	return TheComp;
}
