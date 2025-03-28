// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TacticSubsystem.generated.h"

class APlayerCharacter;
class AMyGameState;
class AVisualFeedbackActor;
class UWidgetComponent;
class ABaseCharacter;
class ATacticPlayerController;
class UWidget_CharacterSkill;
class UBaseAbility;
class USplineMeshComponent;
class UMaterialInstanceDynamic;

DECLARE_MULTICAST_DELEGATE(FOnCharacterStateChange)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterMove, ATacticPlayerController*)

DECLARE_MULTICAST_DELEGATE_OneParam(FSkillStateChange, UBaseAbility*)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillLocationChange, ATacticPlayerController*, UBaseAbility*)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseEvent, ABaseCharacter*)

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UTacticSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//切换到另一个角色行动时
	FOnCharacterStateChange OnSwitchToNextCharacterAction;
	//一个角色回合结束的时候
	FOnCharacterStateChange OnRoundFinish;
	//取消移动,技能的选择
	FOnCharacterStateChange OnCancelMoveAndSkill;
	FOnCharacterStateChange OnCancelMove;

	//全局的，查看某一个角色的信息的时候，显示移动范围。
	FOnMouseEvent OnMyMouseBeginCursorOver;
	FOnMouseEvent OnMyMouseEndCursorOver;


	// 预先准备移动： 显示角色移动路径，让bCanMove为True
	FOnCharacterMove OnPreMove;
	// 执行移动： 检测bCanMove，然后bCanMove为False
	FOnCharacterMove OnMove;

	//选择技能前，鼠标放上去显示的 : todo Actor显示范围，所有可以打的敌人高亮，一些UI显示。。
	FSkillStateChange OnPreSkillSelection;
	//正在选择，显示Visual FeedBack等
	FOnSkillLocationChange OnPostSkillSelected;
	//技能释放了
	FOnSkillLocationChange OnSkillRelease;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	APlayerCharacter* CurrentControlPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JFSetting)
	ABaseCharacter* CurrentActionCharacter;


	float DebugLifeTime = 0.1f;

	TArray<ABaseCharacter*> GlobalPotentialTargets;


	//Data of Team
	// Character team management functions
	UFUNCTION(BlueprintCallable, Category = "Team Management")
	void SortCharactersByActionValues();

	// Team query functions
	UFUNCTION(BlueprintCallable, Category = "Team Management")
	TArray<ABaseCharacter*> GetAllHostileCharacters(const ABaseCharacter* Character) const;

	UFUNCTION(BlueprintCallable, Category = "Team Management")
	TArray<ABaseCharacter*> GetAllFriendlyCharacters(const ABaseCharacter* Character) const;

	// Team status check functions
	UFUNCTION(BlueprintCallable, Category = "Team Management")
	bool AreAllEnemiesDefeated() const;

	UFUNCTION(BlueprintCallable, Category = "Team Management")
	bool AreAllPlayersDefeated() const;

	// Getter functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team Management")
	TArray<ABaseCharacter*> GetAllCharactersInOrder() const { return AllCharactersInOrder; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team Management")
	TArray<ABaseCharacter*> GetPlayerTeam() const { return PlayerTeam; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team Management")
	TArray<ABaseCharacter*> GetEnemyTeam() const { return EnemyTeam; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team Management")
	TArray<ABaseCharacter*> GetNeutralTeam() const { return NeutralTeam; }

	// Character team management
	UFUNCTION(BlueprintCallable, Category = "Team Management")
	void RemoveCharacterFromTeamByType(ABaseCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Team Management")
	void AddCharacterToTeamByType(ABaseCharacter* Character);

	UFUNCTION(BlueprintCallable)
	AVisualFeedbackActor* GetVisualFeedbackActor();

protected:
	TArray<FVector> MovePoints;

	void SwitchToNextCharacterAction();


	FTimerHandle VisualFeedBackTimeHandle;
	bool bCanMove = false;

	UTacticSubsystem();
	UPROPERTY()
	AVisualFeedbackActor* ShowVisualFeedbackActor;
	TSubclassOf<AVisualFeedbackActor> VisualFeedbackActorClass;

	void RoundFinish();

	void MyMouseEndCursorOver(ABaseCharacter* BaseCharacter);
	void MyMouseBeginCursorOver(ABaseCharacter* BaseCharacter);
	void CancelMoveAndSkillThenClearVisualFeedback();
	void Move(ATacticPlayerController* InTacticPlayerController);
	void CharacterPreMove(ATacticPlayerController* TacticPlayerController);
	void SkillRelease(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility);
	void PostSkillSelected(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility);
	void PreSkillSelection(UBaseAbility* BaseAbility);
	void CancelMove();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void BeginSwitchCharacter();
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void CheckGlobalPotentialTargetsOutline();
	void PostSkillSelectedTimer(ATacticPlayerController* InTacticPlayerController, UBaseAbility* BaseAbility);

	UFUNCTION()
	void PreMove(ATacticPlayerController* InTacticPlayerController);

	// Path visualization settings
	FVector2D PathScale = FVector2D(10.0f, 10.0f);
	FLinearColor PathColor = FLinearColor(1.0f, 0.f, 0.f, 1.0f);
	float PathOpacity = 0.8f;

	FTimerHandle SelectedSkillTimerHandle;

private:
	UPROPERTY()
	TArray<ABaseCharacter*> EnemyTeam;

	UPROPERTY()
	TArray<ABaseCharacter*> PlayerTeam;

	UPROPERTY()
	TArray<ABaseCharacter*> NeutralTeam;

	// Character ordering by action values
	UPROPERTY()
	TArray<ABaseCharacter*> AllCharactersInOrder;
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
