// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TacticSubsystem.generated.h"

class APlayerCharacter;
class ATacticPlayerController;
class AMyGameState;
class AMyPlayerController;
class AShowVisualFeedbackActor;
class UWidgetComponent;
class ABaseCharacter;
class ATacticPlayerCharacter;
class UWidget_CharacterSkill;
class UBaseAbility;
class USplineMeshComponent;
class UMaterialInstanceDynamic;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterStateChange, ABaseCharacter*)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterSkillStateChange, ABaseCharacter*, UBaseAbility*)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnCharacterSkillRelease, ABaseCharacter*, UBaseAbility*,TArray<ABaseCharacter*>)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseEvent, ABaseCharacter*)

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UTacticSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void PreMoveBroadCast();


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
	FOnCharacterSkillRelease OnSkillRelease;
	//技能取消选择了
	FOnCharacterSkillStateChange OnSkillSelectionCancelled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	APlayerCharacter* CurrentControlPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JFSetting)
	ABaseCharacter* CurrentActionCharacter;


	float DebugLifeTime = 0.1f;

	bool bIsInRange;


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

protected:
	TArray<FVector> MovePoints;

	void SwitchCharacterAction(ABaseCharacter* BaseCharacter);

	UFUNCTION()
	void CharacterPreMove(ABaseCharacter* InBaseCharacter);
	void HideVisualFeedback_Move();

	FTimerHandle VisualFeedBackTimeHandle;
	void CancelSelectedSkill();
	bool bCanMove = false;

	UTacticSubsystem();
	UPROPERTY()
	AShowVisualFeedbackActor* ShowVisualFeedbackActor;
	TSubclassOf<AShowVisualFeedbackActor> VisualFeedbackActorClass;
	AShowVisualFeedbackActor* GetShowVisualFeedbackActor();

	void PreSkillSelection(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility);
	void Move(ABaseCharacter* BaseCharacter);
	void CancelMove(ABaseCharacter* BaseCharacter);

	void MyMouseEndCursorOver(ABaseCharacter* BaseCharacter);
	void MyMouseBeginCursorOver(ABaseCharacter* BaseCharacter);
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	ATacticPlayerController* GetTacticPlayerController()
	{
		if (!TacticPlayerController)
		{
			TacticPlayerController = GetWorld()->GetFirstPlayerController<ATacticPlayerController>();
		}
		return TacticPlayerController;
	}

	void SkillRelease(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility, TArray<ABaseCharacter*> PotentialTargets);
	void RoundFinish(ABaseCharacter* BaseCharacter);
	void PostSkillSelected(ABaseCharacter* BaseCharacter, UBaseAbility* BaseAbility);

	UFUNCTION()
	void ShowMove();

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

private:
	UPROPERTY()
	ATacticPlayerController* TacticPlayerController;
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
