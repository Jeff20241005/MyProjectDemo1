// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComp.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECTDEMO1_API UInteractionComp : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComp();

	void HoveredActor();
	void UnHoveredActor();
	void OnClickedActor();

	void SetAsSkillTarget();
	void UnSetAsSkillTarget();

	void SetDepth(bool InIsSetDepth);


protected:
	bool bIsSelected = false;
	
	virtual void BeginPlay() override;


	UFUNCTION()
	void SelectedAsTarget();

	UFUNCTION()
	void RemoveFromTarget();

	UFUNCTION()
	void BeenHighlightAsTarget();

	UFUNCTION()
	void EndOfHighlightAsTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	USkeletalMeshComponent* SkeletalMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JFSetting)
	UStaticMeshComponent* StaticMeshComponent;
};
