// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Components/InteractionComp.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/Framework/Controllers/MyPlayerController.h"


void ABaseCharacter::OnDamaged_Implementation(float DamageAmount, const FHitResult& HitInfo,
                                              const struct FGameplayTagContainer& DamageTags,
                                              ABaseCharacter* InstigatorCharacter, AActor* DamageCauser)
{
}

void ABaseCharacter::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);


	// 当鼠标开始悬停在物品上时
	if (InteractionComp)
	{
		InteractionComp->OnClickedActor();
	}
}

ABaseCharacter::ABaseCharacter()
{
	// 设置这个角色每帧都tick
	PrimaryActorTick.bCanEverTick = true;

	MyAbilityComp = CreateComponent<UMyAbilityComp>();
	BaseCharacterAttributeSet = CreateComponent<UBaseCharacterAttributeSet>();
	InteractionComp = CreateComponent<UInteractionComp>();
	TeamComp = CreateComponent<UTeamComp>();
	// 设置骨骼网格体的碰撞
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetCollisionObjectType(ECC_Pawn);
		GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	}

	// 确保角色可以在导航网格上移动
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 配置移动组件
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->bOrientRotationToMovement = true;
		MovementComp->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
		MovementComp->bUseControllerDesiredRotation = false;
		MovementComp->bConstrainToPlane = true;
		MovementComp->bSnapToPlaneAtStart = true;
		MovementComp->MaxWalkSpeed = 600.0f;
		MovementComp->MinAnalogWalkSpeed = 20.0f;
		MovementComp->bUseRVOAvoidance = false;
		MovementComp->NavAgentProps.bCanCrouch = false;
		MovementComp->NavAgentProps.bCanJump = false;
		MovementComp->NavAgentProps.bCanWalk = true;
	}

	// 设置AI控制器类
	AIControllerClass = ABaseAIController::StaticClass();
	// 设置自动AI控制
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//todo 去ProjectSetting设置
	//GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
	//GetMesh()->SetNotifyRigidBodyCollision(true);
	//GetMesh()->SetGenerateOverlapEvents(true);

	//战斗相关UMG//
	WidgetComponent_BattleMenu = CreateComponent<UWidgetComponent>();
	//WidgetComponent_BattleMenu->SetupAttachment(GetRootComponent());

	HealthComponent = CreateComponent<UWidgetComponent>();
	HealthComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthComponent->SetVisibility(false);

	RangeComponent = CreateComponent<UWidgetComponent>();
	RangeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RangeComponent->SetVisibility(false);
	RangeComponent->SetRelativeLocation(FVector(0, 0, -85));
	RangeComponent->SetRelativeRotation(FRotator(90, 0, 0));
}

void ABaseCharacter::BaseCharacterAIMoveTo(FVector EndLocation)
{
	// 1. 如果角色当前被PlayerController控制，需要切换到AI控制
	if (Cast<AMyPlayerController>(GetController()))
	{
		// 暂时取消玩家控制
		MyPlayerController->UnPossess();

		// 使用缓存的AIController
		if (BaseAIController)
		{
			BaseAIController->Possess(this);
		}
		else
		{
			FString TempStr = TEXT("No cached AIController found!");
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr);
			return;
		}
	}
	if (BaseAIController)
	{
		// 获取角色的移动范围
		float MoveRange = BaseCharacterAttributeSet->GetMoveRange();

		// 传递移动范围参数
		//BaseAIController->MoveToLocationWithPathFinding(EndLocation);

		BaseAIController->MoveToLocationWithPathFinding(EndLocation, false, MoveRange);
	}
}


void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 初始化AbilitySystem
	if (MyAbilityComp)
	{
		MyAbilityComp->InitStats(
			UBaseCharacterAttributeSet::StaticClass(),
			nullptr /*GameplayEffectContext*/
		);
	}


	//Controllers Init
	BaseAIController = Cast<ABaseAIController>(GetController());
	//若没有BaseAIController，则生成
	if (!BaseAIController)
	{
		const FVector SpawnOffset(200.f, 0.f, 0.f);
		const FVector SpawnLocation = GetActorTransform().TransformPosition(SpawnOffset);
		FTransform SpawnActorTransform(FQuat::Identity, SpawnLocation);
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = this;
		ABaseAIController* InBaseAIController = GetWorld()->SpawnActor<ABaseAIController>(
			ABaseAIController::StaticClass(), SpawnActorTransform,
			ActorSpawnParameters);
		BaseAIController = InBaseAIController;
	}

	MyPlayerController = Cast<AMyPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
}


void ABaseCharacter::HandleDamage(float DamageAmount, const FHitResult& HitInfo,
                                  const struct FGameplayTagContainer& DamageTags, ABaseCharacter* InstigatorCharacter,
                                  AActor* DamageCauser)
{
	if (MyAbilityComp->bAbilitiesInitialized)
	{
		OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
	}
}

void ABaseCharacter::OnHealthChanged_Implementation(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
}

void ABaseCharacter::HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (MyAbilityComp->bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue, EventTags);
	}
}


void ABaseCharacter::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();

	//todo
	if (!InteractionComp->bIsSelected)
	{
		HealthComponent->SetVisibility(true);
		if (!InteractionComp->bIsMoved && !bIsAttackRange)
		{
			//---显示移动范围等
			DrawMoveRange();
		}
	}


	// 当鼠标开始悬停在角色上时
	if (InteractionComp)
	{
		InteractionComp->HoveredActor();
	}

	// 通知PlayerController
	if (MyPlayerController)
	{
		MyPlayerController->HoveredActor = this;
	}
}

void ABaseCharacter::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();

	//todo
	if (!InteractionComp->bIsSelected)
	{
		HealthComponent->SetVisibility(false);
		if (!InteractionComp->bIsMoved && !bIsAttackRange)
		{
			//---显示移动范围等
			CloseWidget();
		}
	}


	// 当鼠标离开角色时
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


void ABaseCharacter::DrawRangeSize(float Radius_P)
{
	RangeComponent->SetDrawSize(FVector2D(Radius_P));
	RangeComponent->SetVisibility(true);
}

float ABaseCharacter::DrawAttackRange()
{
	float AttackRange = BaseCharacterAttributeSet->GetAttackRange();
	DrawRangeSize(AttackRange);
	bIsAttackRange = true;

	return AttackRange;
}

void ABaseCharacter::DrawMoveRange()
{
	DrawRangeSize(BaseCharacterAttributeSet->GetMoveRange());
}

void ABaseCharacter::CloseWidget()
{
	RangeComponent->SetVisibility(false);
}
