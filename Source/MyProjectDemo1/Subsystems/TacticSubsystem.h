// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProjectDemo1/Framework/Controllers/MyPlayerController.h"
#include "MyProjectDemo1/Other/PathTracerComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TacticSubsystem.generated.h"

class UWidgetComponent;
class ATacticGameState;
class ABaseCharacter;
class ATacticPlayerCharacter;
class UWidget_CharacterSkill;
class UBaseAbility;
class USplineMeshComponent;
class UMaterialInstanceDynamic;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterStateChange, ABaseCharacter*)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterSkillStateChange, ABaseCharacter*, UBaseAbility*)


DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseEvent,ABaseCharacter*)

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UTacticSubsystem : public UGameInstanceSubsystem
{
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	//UFUNCTION()
	//void ShowSkill();
	UFUNCTION()
	void ShowMove();

	
	FOnMouseEvent OnMyMouseBeginCursorOver;
	FOnMouseEvent OnMyMouseEndCursorOver;

	
	FOnCharacterStateChange OnSwitchCharacterAction;
	FOnCharacterStateChange OnRoundFinish;

	//正在选择，显示Visual FeedBack等
	FOnCharacterSkillStateChange OnSkillSelection;
	//技能释放了
	FOnCharacterSkillStateChange OnSkillRelease;
	//技能取消选择了
	FOnCharacterSkillStateChange OnSkillSelectionCancelled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	ABaseCharacter* CurrentControlCharacter;
	FTimerHandle VisualFeedBackTimeHandle;

	TArray<FVector> MovePoints;
	float DebugLifeTime=0.1f;
	UPROPERTY()
	AMyPlayerController* MyPlayerController;

	void SwitchCharacterAction(ABaseCharacter* BaseCharacter);
	void BeginDrawVisualFeedBack();
	void ShowSkillVisualFeedBack();
	void RoundFinish(ABaseCharacter* BaseCharacter);
	void SelectedSkill(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility);
	
	// Path visualization functions
	void DebugVisual(const TArray<FVector>& PathPoints);
	UPathTracerComponent* CreateUPathTracerComponent();
	//void ClearPathVisualization();

protected:

	
	// Path visualization properties
	UPROPERTY()
	TArray<USplineMeshComponent*> PathSplineMeshes;
	
	UPROPERTY()
	UMaterialInstanceDynamic* PathMaterial;
	
	UPROPERTY()
	UStaticMesh* PathMesh;
	
	// Path visualization settings
	FVector2D PathScale = FVector2D(10.0f, 10.0f);
	FLinearColor PathColor = FLinearColor(1.0f, 0.f, 0.f, 1.0f);
	float PathOpacity = 0.8f;



	GENERATED_BODY()
};
