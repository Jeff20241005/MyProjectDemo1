// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComp.h"

UInteractionComp::UInteractionComp()
{
	PrimaryComponentTick.bCanEverTick = false; //tick disabled
}

void UInteractionComp::HoveredActor()
{
	// 设置高亮效果
	SetDepth(true);
}

void UInteractionComp::UnHoveredActor()
{
	// 设置高亮效果
	SetDepth(false);
}

void UInteractionComp::OnClickedActor()
{
	SetDepth(true);
}

void UInteractionComp::SetAsSkillTarget()
{
	SetDepth(true);
}

void UInteractionComp::UnSetAsSkillTarget()
{
	
	SetDepth(false);
}

void UInteractionComp::SetDepth(bool InIsSetDepth)
{
	//Visual Feedback
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetRenderCustomDepth(InIsSetDepth);
	}
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(InIsSetDepth);
	}
}

void UInteractionComp::BeginPlay()
{
	//Character
	SkeletalMeshComponent = Cast<USkeletalMeshComponent>(
		GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	//Item
	StaticMeshComponent = Cast<UStaticMeshComponent>(
		GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	Super::BeginPlay();
}

void UInteractionComp::SelectedAsTarget()
{
	SkeletalMeshComponent->SetColorParameterValueOnMaterials("LightingColor", FColor::Red);
	SkeletalMeshComponent->SetScalarParameterValueOnMaterials("Brightness", 0.1f);
	bIsSelected = true;
}

void UInteractionComp::RemoveFromTarget()
{
	SkeletalMeshComponent->SetColorParameterValueOnMaterials("LightingColor", FColor::Yellow);
	SkeletalMeshComponent->SetScalarParameterValueOnMaterials("Brightness", 1.0f);
	bIsSelected = false;
}

void UInteractionComp::BeenHighlightAsTarget()
{
	SkeletalMeshComponent->SetScalarParameterValueOnMaterials("Brightness", .5f);
	bIsSelected = true;
}

void UInteractionComp::EndOfHighlightAsTarget()
{
	SkeletalMeshComponent->SetScalarParameterValueOnMaterials("Brightness", .0f);
	bIsSelected = false;
}
