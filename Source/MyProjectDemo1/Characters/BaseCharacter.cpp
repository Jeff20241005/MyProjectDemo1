// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Components/InteractionComp.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/Framework/Controllers/MyPlayerController.h"
#include "MyProjectDemo1/Framework/GameModes/MyGameMode.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "MyProjectDemo1/Framework/GameStates/TacticGameState.h"


void ABaseCharacter::OnDamaged_Implementation(float DamageAmount, const FHitResult& HitInfo,
                                              const struct FGameplayTagContainer& DamageTags,
                                              ABaseCharacter* InstigatorCharacter, AActor* DamageCauser)
{
}

void ABaseCharacter::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
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


	SpringArmComponent = CreateComponent<USpringArmComponent>();
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->TargetArmLength = 1000;

	// 禁用所有旋转继承
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;

	// 设置固定的旋转角度（俯视角）
	SpringArmComponent->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));

	// 确保角色旋转不会影响摄像机
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraComponent = CreateComponent<UCameraComponent>();
	CameraComponent->SetupAttachment(SpringArmComponent);

	//todo 去ProjectSetting设置
	//GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
	//GetMesh()->SetNotifyRigidBodyCollision(true);
	//GetMesh()->SetGenerateOverlapEvents(true);

	HealthWidgetComp = CreateComponent<UWidgetComponent>();
	HealthWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthWidgetComp->SetVisibility(false);


	MoveRangeWidgetComp = CreateComponent<UWidgetComponent>();
	MoveRangeWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MoveRangeWidgetComp->SetVisibility(false);
	MoveRangeWidgetComp->SetRelativeLocation(FVector(0, 0, -85));
	MoveRangeWidgetComp->SetRelativeRotation(FRotator(90, 0, 0));
}

void ABaseCharacter::DrawRangeSize(float Radius_P)
{
	MoveRangeWidgetComp->SetDrawSize(FVector2D(Radius_P));
	MoveRangeWidgetComp->SetVisibility(true);
}

void ABaseCharacter::DrawMoveRange(ABaseCharacter* BaseCharacter)
{
	DrawRangeSize(BaseCharacter->GetBaseCharacterAttributeSet()->GetMoveRange());
}

void ABaseCharacter::CloseWidget()
{
	MoveRangeWidgetComp->SetVisibility(false);
}


void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Try to get GameState
	TacticGameState = Cast<ATacticGameState>(UGameplayStatics::GetGameState(GetWorld()));
	
	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		TacticSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTacticSubsystem>();
	}
	
	MyGameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

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
	
	// 添加空指针检查
	if (TacticSubsystem)
	{
		TacticSubsystem->OnMyMouseBeginCursorOver.Broadcast(this);
	}
}

void ABaseCharacter::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
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

	// 添加空指针检查
	if (TacticSubsystem && TacticSubsystem->OnMyMouseEndCursorOver.IsBound())
	{
		TacticSubsystem->OnMyMouseEndCursorOver.Broadcast(this);
	}
}
