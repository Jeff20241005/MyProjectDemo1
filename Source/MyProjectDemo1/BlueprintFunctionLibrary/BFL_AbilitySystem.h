// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BFL_AbilitySystem.generated.h"

class UMyAbilityComp;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UBFL_AbilitySystem : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//Get modified comp from a specific actor
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Jeff|Utilities", meta=(WorldContext="WorldContextObject"))
	static UMyAbilityComp* GetMyAbilityCompFromActor(AActor* Actor);
};
