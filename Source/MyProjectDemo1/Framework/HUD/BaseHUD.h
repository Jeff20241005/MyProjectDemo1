// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

class UTacticMainUI;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API ABaseHUD : public AHUD
{
	GENERATED_BODY()
public:
	//全局只有一个Widget。避免打开多个UMG
	template <class T>
	T* GetSingleObjectWidgetClass(TSubclassOf<UUserWidget> WidgetClass);	

protected:
	template <class T>
	void MakeUserWidget(T*& ObjectPtr, TSubclassOf<T> UIClass);
	
	virtual void BeginPlay() override;

	UPROPERTY()
	TMap<UClass*, UUserWidget*> SingleObjectMap;
};
