// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TacticSubsystem.generated.h"

class ATacticPlayerController;
class AMyGameState;
class AMyPlayerController;
class AShowVisualFeedbackActor;
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


DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseEvent, ABaseCharacter*)

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UTacticSubsystem : public UGameInstanceSubsystem
{
	void PreSkillSelection(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility);
	void Move(ABaseCharacter* BaseCharacter);
	void CancelMove(ABaseCharacter* BaseCharacter);
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	void PreMoveBroadCast();
	
	UFUNCTION()
	void TestFunc_SwitchCharacter_RanOutOfAction();
	
	// 预先准备移动： 显示角色移动路径，让bCanMove为True
	FOnCharacterStateChange OnPreMove;
	// 执行移动： 检测bCanMove，然后bCanMove为False
	FOnCharacterStateChange OnMove;
	// 取消移动： bCanMove为False，然后取消划线
	FOnCharacterStateChange OnCancelMove;


	
	//全局的，查看某一个角色的信息的时候，显示移动范围。
	FOnMouseEvent OnMyMouseBeginCursorOver;
	FOnMouseEvent OnMyMouseEndCursorOver;

	
	//切换到另一个角色行动时
	FOnCharacterStateChange OnSwitchCharacterAction;
	//一个角色回合结束的时候
	FOnCharacterStateChange OnRoundFinish;
	
	
	

	//选择技能前，鼠标放上去显示的 : todo Actor显示范围，所有可以打的敌人高亮，一些UI显示。。
	FOnCharacterSkillStateChange OnPreSkillSelection;
	//正在选择，显示Visual FeedBack等
	FOnCharacterSkillStateChange OnPostSkillSelected;
	//技能释放了
	FOnCharacterSkillStateChange OnSkillRelease;
	//技能取消选择了
	FOnCharacterSkillStateChange OnSkillSelectionCancelled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	ABaseCharacter* CurrentControlPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JFSetting)
	ABaseCharacter* CurrentActionCharacter;
	
	FTimerHandle VisualFeedBackTimeHandle;

	TArray<FVector> MovePoints;
	float DebugLifeTime = 0.1f;
	
	


	void SwitchCharacterAction(ABaseCharacter* BaseCharacter);

	UFUNCTION()
	void CharacterPreMove(ABaseCharacter* InBaseCharacter);
	void HideVisualFeedback_Move();

protected:
	bool bCanMove=false;
	
	UTacticSubsystem();
	UPROPERTY()
	AShowVisualFeedbackActor* ShowVisualFeedbackActor;
	TSubclassOf<AShowVisualFeedbackActor> VisualFeedbackActorClass;
	AShowVisualFeedbackActor* GetShowVisualFeedbackActor();


	ATacticPlayerController* GetTacticPlayerController()
	{
		return GetWorld()->GetFirstPlayerController<ATacticPlayerController>();
	}
	
	ATacticGameState* GetTacticGameState();

	void RoundFinish(ABaseCharacter* BaseCharacter);
	void SelectedSkill(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility);


	UFUNCTION()
	void ShowMove();

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

template <typename T>
void FindMyClass(TSubclassOf<T>& YourSubClass, const TCHAR* Path)
{
	if (ConstructorHelpers::FClassFinder<T> ClassFinder(Path); ClassFinder.Succeeded())
	{
		YourSubClass = ClassFinder.Class;
	}
	else
	{
		FString TempStr = FString::Printf(TEXT("Not Found! : %s"), Path);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}
}
