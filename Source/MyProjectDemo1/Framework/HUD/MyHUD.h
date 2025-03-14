// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API AMyHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<UClass*, UUserWidget*> SingleObjectMap;

	//全局只有一个Widget。避免打开多个UMG
	template <class T>
	T* GetSingleObjectWidgetClass(TSubclassOf<UUserWidget> WidgetClass);
};

