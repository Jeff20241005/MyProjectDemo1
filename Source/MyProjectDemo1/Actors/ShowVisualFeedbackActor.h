// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShowVisualFeedbackActor.generated.h"

class UPathTracerComponent;
class ABaseCharacter;
class UWidgetComponent;

UCLASS()
class MYPROJECTDEMO1_API AShowVisualFeedbackActor : public AActor
{
	GENERATED_BODY()

public:
	AShowVisualFeedbackActor();

	void OnMouseCursorOver();

	float DrawAttackRange(ABaseCharacter* BaseCharacter);
	UFUNCTION(BlueprintCallable)
	UPathTracerComponent* GetPathTracerComponent() const { return PathTracerComponent; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UPathTracerComponent* PathTracerComponent;

	template <class Comp>
	Comp* CreateComponent();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};

//记得包含头文件在C++文件
template <typename Comp>
Comp* AShowVisualFeedbackActor::CreateComponent()
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
