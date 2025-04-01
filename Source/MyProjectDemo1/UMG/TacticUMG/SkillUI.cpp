// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillUI.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "MyProjectDemo1/UMG/TacticUMG/TacticMainUI.h"
#include "MyProjectDemo1/Framework/Controllers/TacticPlayerController.h"

void USkillUI::ActionButtonUIOnHovered()
{
	if (!BaseAbility)
	{
		RemoveFromParent();
		return;
	}
}

void USkillUI::ActionButtonUIOnClicked()
{
	UTacticSubsystem* TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>();
	if (ATacticPlayerController* TacticPlayerController = GetWorld()->GetFirstPlayerController<
		ATacticPlayerController>(); TacticSubsystem->CurrentActionCharacter && BaseAbility)
	{
		TacticSubsystem->OnPostSkillSelected.Broadcast(TacticPlayerController, BaseAbility);
	}
}

void USkillUI::ActionButtonUIOnUnhovered()
{
}

void USkillUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!BaseAbility)
	{
		RemoveFromParent();
		{
			FString
				TempStr = FString::Printf(TEXT("void USkillUI::NativeConstruct() if (!BaseAbility)"));
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
		return;
	}
	ActionButtonUI->Button->OnHovered.AddDynamic(this, &ThisClass::ActionButtonUIOnHovered);
	ActionButtonUI->Button->OnClicked.AddDynamic(this, &ThisClass::ActionButtonUIOnClicked);
	ActionButtonUI->Button->OnUnhovered.AddDynamic(this, &ThisClass::ActionButtonUIOnUnhovered);

	ActionButtonUI->TextBlock->SetText(BaseAbility->SkillName);
}
