// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TacticMainUI.generated.h"

class UCanvasPanel;
class UCharacterActionUI;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UTacticMainUI : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UCharacterActionUI* CharacterActionUI;
	FVector2D MousePos;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UCanvasPanel* MainCanvasPanel;

	
};
