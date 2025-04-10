// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "TacticBaseCharacter.generated.h"

UCLASS()
class MYPROJECTDEMO1_API ATacticBaseCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	void ShowMoveRange();
	void CloseMoveRange();

	void Move(FVector MoveLocation);
	bool CanMove();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
	float In_Radius = 0.96f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FName MaterialName_Radius = FName("Radius");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	FName MaterialName_Color = FName("Color");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FVector DefaultColorValueOfSphereSM = FVector(1, 1, 7);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTeamComp* GetTeamComp() const { return TeamComp; }

	void SetWidgetHealth(float Health,float MaxHealth);
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	template <class T>
	bool FindMyObject(T*& YourObject, const TCHAR* Path);
	template <class T>
	void FindMyClass(TSubclassOf<T>& YourSubClass, const TCHAR* Path);
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;

	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UTeamComp* TeamComp;
	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UWidgetComponent* HealthWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
	UStaticMeshComponent* MoveRangeStaticMeshComponent;

	ATacticBaseCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
