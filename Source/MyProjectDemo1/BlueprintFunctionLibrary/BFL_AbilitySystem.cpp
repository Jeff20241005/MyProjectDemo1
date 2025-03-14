// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_AbilitySystem.h"

#include "MyProjectDemo1/Components/MyAbilityComp.h"

UMyAbilityComp* UBFL_AbilitySystem::GetMyAbilityCompFromActor(AActor* Actor)
{
	if (Actor)
	{
		return Actor->GetComponentByClass<UMyAbilityComp>();
	}
	{
		FString
			TempStr = FString::Printf(TEXT("GetAbilitySystemComponent @Param Actor == nullptr"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}
	return nullptr;
}
