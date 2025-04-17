// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Components/InteractionComp.h"
#include "MyProjectDemo1/Framework/Controllers/BasePlayerController.h"


void ABaseItem::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
	// 当鼠标开始悬停在物品上时
	if (InteractionComp)
	{
		InteractionComp->HoveredActor();
	}
	
	if (MyPlayerController)
	{
		MyPlayerController->HoveredActor = this;
	}
}

void ABaseItem::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	if (InteractionComp)
	{
		InteractionComp->UnHoveredActor();
	}
	
	// 通知PlayerController
	if (MyPlayerController && MyPlayerController->HoveredActor == this)
	{
		MyPlayerController->HoveredActor = nullptr;
	}
}


ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateComponent<USceneComponent>();
	SetRootComponent(SceneComponent);
	SphereComponent = CreateComponent<USphereComponent>();
	InteractionComp = CreateComponent<UInteractionComp>();
	
	//SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &InteractionComp::OnComponentBeginOverlap);
	//SphereComponent->OnComponentEndOverlap.AddDynamic(this, &InteractionComp::OnComponentEndOverlap);
}

void ABaseItem::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	if (InteractionComp)
	{
		InteractionComp->OnClickedActor();
	}
	MyPlayerController->ClickedItem = this;
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	MyPlayerController = Cast<ABasePlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
