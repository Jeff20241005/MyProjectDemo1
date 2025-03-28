// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ThisProjectFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UThisProjectFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Jeff|Utilities")
	static FVector FVectorZToGround(const FVector& Source);

	UFUNCTION(BlueprintCallable, Category="Jeff|Utilities")
	static void ClampMoveRange(const FVector& CurrentLocation, float RangeToMove, FVector& TargetLocation);
};
