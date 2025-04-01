// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionBase.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UActionBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Text", meta=(ExposeOnSpawn=true))
	FText DefaultText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Text", meta=(ExposeOnSpawn=true, ClampMin="8", ClampMax="72", UIMin="8", UIMax="72"))
	int32 FontSize = 12;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UTextBlock* TextBlock;
	
	virtual void NativePreConstruct() override;
};
