// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TacticSubsystem.generated.h"

class ATacticPlayerCharacter;
class AMyGameState;
class AVisualFeedbackActor;
class UWidgetComponent;
class ATacticBaseCharacter;
class ATacticPlayerController;
class UWidget_CharacterSkill;
class UBaseAbility;
class USplineMeshComponent;
class UMaterialInstanceDynamic;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSettingChange, bool)
DECLARE_MULTICAST_DELEGATE(FOnCharacterStateChange)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterMove, ATacticPlayerController*)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterPreMove, ATacticPlayerController*, UBaseAbility*)

DECLARE_MULTICAST_DELEGATE_OneParam(FSkillStateChange, UBaseAbility*)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillLocationChange, ATacticPlayerController*, UBaseAbility*)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseEvent, ATacticBaseCharacter*)

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UTacticSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//切换智能移动
	FOnSettingChange OnChangeAutomaticMoveBySkill;

	//切换到另一个角色行动时
	FOnCharacterStateChange OnSwitchToNextCharacterAction;
	//一个角色回合结束的时候
	FOnCharacterStateChange OnRoundFinish;
	//取消移动,技能的选择
	FOnCharacterStateChange OnCancelSkill;
	FOnCharacterStateChange OnCancelMove;

	//全局的，查看某一个角色的信息的时候，显示移动范围。
	FOnMouseEvent OnMyMouseBeginCursorOver;
	FOnMouseEvent OnMyMouseEndCursorOver;


	// 预先准备移动： 显示角色移动路径，让bCanMove为True
	FOnCharacterPreMove OnPreMove;
	// 执行移动： 检测bCanMove，然后bCanMove为False
	FOnCharacterMove OnMove;

	//选择技能前，鼠标放上去显示的 : todo Actor显示范围，所有可以打的敌人高亮，一些UI显示。。
	FSkillStateChange OnPreSkillSelection;
	//正在选择，显示Visual FeedBack等
	FOnSkillLocationChange OnPostSkillSelected;
	FOnSkillLocationChange OnPostSkillSelectedTimer;
	//技能释放了
	FOnSkillLocationChange OnSkillRelease;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	ATacticPlayerCharacter* CurrentControlPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JFSetting)
	ATacticBaseCharacter* CurrentActionCharacter;


	float DebugLifeTime = 0.1f;

	TArray<ATacticBaseCharacter*> GlobalPotentialTargets;

	bool bEnableAutomaticMoveBySkill = false;


	//Data of Team
	// Character team management functions
	UFUNCTION(BlueprintCallable, Category = "Team Management")
	void SortCharactersByActionValues();

	// Team query functions
	UFUNCTION(BlueprintCallable, Category = "Team Management")
	TArray<ATacticBaseCharacter*> GetAllHostileCharacters(const ATacticBaseCharacter* Character) const;

	UFUNCTION(BlueprintCallable, Category = "Team Management")
	TArray<ATacticBaseCharacter*> GetAllFriendlyCharacters(const ATacticBaseCharacter* Character) const;

	// Team status check functions
	UFUNCTION(BlueprintCallable, Category = "Team Management")
	bool AreAllEnemiesDefeated() const;

	UFUNCTION(BlueprintCallable, Category = "Team Management")
	bool AreAllPlayersDefeated() const;

	// Getter functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team Management")
	TArray<ATacticBaseCharacter*> GetAllCharactersInOrder() const { return AllCharactersInOrder; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team Management")
	TArray<ATacticBaseCharacter*> GetPlayerTeam() const { return PlayerTeam; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team Management")
	TArray<ATacticBaseCharacter*> GetEnemyTeam() const { return EnemyTeam; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team Management")
	TArray<ATacticBaseCharacter*> GetNeutralTeam() const { return NeutralTeam; }

	// Character team management
	UFUNCTION(BlueprintCallable, Category = "Team Management")
	void RemoveCharacterFromTeamByType(ATacticBaseCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Team Management")
	void AddCharacterToTeamByType(ATacticBaseCharacter* Character);

	UFUNCTION(BlueprintCallable)
	AVisualFeedbackActor* GetVisualFeedbackActor();
	void CancelMoveAndSkill();

	bool bCanMove = false;
protected:
	TArray<FVector> MovePoints;

	void SwitchToNextCharacterAction();
	void PreMove_IfHasSkillRadius(ATacticPlayerController* InTacticPlayerController, float SkillPlacementRadius = 0);


	FTimerHandle VisualFeedBackTimeHandle;

	UTacticSubsystem();
	UPROPERTY()
	AVisualFeedbackActor* ShowVisualFeedbackActor;
	TSubclassOf<AVisualFeedbackActor> VisualFeedbackActorClass;

	void RoundFinish();

	void MyMouseEndCursorOver(ATacticBaseCharacter* BaseCharacter);
	void MyMouseBeginCursorOver(ATacticBaseCharacter* BaseCharacter);
	void CancelSkill();
	void Move(ATacticPlayerController* InTacticPlayerController);
	void PreMove(ATacticPlayerController* InTacticPlayerController, UBaseAbility* InBaseAbility);
	void SkillRelease(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility);
	void DoPostSkillSelectedTimer(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility);
	void PostSkillSelected(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility);
	void PreSkillSelection(UBaseAbility* BaseAbility);
	void CancelMove();
	void ChangeAutomaticMoveBySkill(bool bNew);
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void BeginSwitchCharacter();
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void CheckGlobalPotentialTargetsOutline();
	void PostSkillSelectedTimer(ATacticPlayerController* InTacticPlayerController, UBaseAbility* BaseAbility);

	void PreMoveTimer(ATacticPlayerController* InTacticPlayerController, UBaseAbility* InBaseAbility);

	// Path visualization settings
	FVector2D PathScale = FVector2D(10.0f, 10.0f);
	FLinearColor PathColor = FLinearColor(1.0f, 0.f, 0.f, 1.0f);
	float PathOpacity = 0.8f;

	FTimerHandle SelectedSkillTimerHandle;

private:
	UPROPERTY()
	TArray<ATacticBaseCharacter*> EnemyTeam;

	UPROPERTY()
	TArray<ATacticBaseCharacter*> PlayerTeam;

	UPROPERTY()
	TArray<ATacticBaseCharacter*> NeutralTeam;

	// Character ordering by action values
	UPROPERTY()
	TArray<ATacticBaseCharacter*> AllCharactersInOrder;
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
