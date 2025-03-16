// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

#include "Blueprint/UserWidget.h"
#include "MyProjectDemo1/UMG/MainUI.h"

template <typename T>
T* AMyHUD::GetSingleObjectWidgetClass(TSubclassOf<UUserWidget> WidgetClass)
{
	if (SingleObjectMap.Contains(T::StaticClass()))
	{
		return Cast<T>(SingleObjectMap[T::StaticClass()]);
	}
	else
	{
		T* Object = CreateWidget<T>(GetWorld(), WidgetClass);
		SingleObjectMap.Add(T::StaticClass(), Object);
		return Object;
	}
}

template <typename T>
void AMyHUD::MakeUserWidget(T*& ObjectPtr, TSubclassOf<T> UIClass)
{
	if (!ObjectPtr)
	{
		if (UIClass)
		{
			ObjectPtr = CreateWidget<T>(GetOwningPlayerController(), UIClass);
			ObjectPtr->AddToViewport();
		}
		else
		{
			FString TempStr = FString::Printf(TEXT("No specific class specified at : %s"), *GetClass()->GetName());
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
	}
}

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();
	MakeUserWidget(MainUI,MainUIClass);
}
