// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

#include "Blueprint/UserWidget.h"

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
