// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

class UMainUI;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API AMyHUD : public AHUD
{
	GENERATED_BODY()
public:
	//全局只有一个Widget。避免打开多个UMG
	template <class T>
	T* GetSingleObjectWidgetClass(TSubclassOf<UUserWidget> WidgetClass);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	TSubclassOf<UMainUI> MainUIClass;
	UPROPERTY()
	UMainUI* MainUI;


protected:
	template <class T>
	void MakeUserWidget(T*& ObjectPtr, TSubclassOf<T> UIClass);
	
	virtual void BeginPlay() override;

	UPROPERTY()
	TMap<UClass*, UUserWidget*> SingleObjectMap;
};
