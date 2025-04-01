// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "BaseItem.generated.h"

class ABasePlayerController;
class UInteractionComp;
class USphereComponent;

UCLASS()
class MYPROJECTDEMO1_API ABaseItem : public AActor
{
	GENERATED_BODY()

public:
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;
	ABaseItem();

	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

protected:
	virtual void BeginPlay() override;
	template <class Comp>
	Comp* CreateComponent();

public:
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	USphereComponent* SphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UInteractionComp* InteractionComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JFSetting)
	ABasePlayerController* MyPlayerController;
};


template <typename Comp>
Comp* ABaseItem::CreateComponent()
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

	USceneComponent* TempSTempSceneComponent = Cast<USceneComponent>(TheComp);
	if (TempSTempSceneComponent)
	{
		TempSTempSceneComponent->SetupAttachment(RootComponent);
	}

	return TheComp;
}
