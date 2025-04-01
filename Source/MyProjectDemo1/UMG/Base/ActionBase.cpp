// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBase.h"
#include "Components/TextBlock.h"


void UActionBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (TextBlock)
	{
		TextBlock->SetText(DefaultText);
		
		// Apply font size
		FSlateFontInfo FontInfo = TextBlock->GetFont();
		FontInfo.Size = FontSize;
		TextBlock->SetFont(FontInfo);
	}
}
