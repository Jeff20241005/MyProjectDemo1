// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Components/InteractionComp.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/FilePaths/DefaultPropertyValue.h"
#include "MyProjectDemo1/FilePaths/FilePaths.h"
#include "MyProjectDemo1/Framework/Controllers/BasePlayerController.h"
#include "MyProjectDemo1/Framework/GameModes/MyGameMode.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"


void ABaseCharacter::OnDamaged_Implementation(float DamageAmount, const FHitResult& HitInfo,
                                              const struct FGameplayTagContainer& DamageTags,
                                              ABaseCharacter* InstigatorCharacter, AActor* DamageCauser)
{
}

void ABaseCharacter::Destroyed()
{
	//GetCapsuleComponent()->SetCanEverAffectNavigation(false);
	Super::Destroyed();
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
	//PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetWorldLocation(FVector(0, 0, CharacterDefaultZHeight_FloatValue));
	GetMesh()->SetWorldRotation(FRotator(0, -90.f, 0));

	MyAbilityComp = CreateComponent<UMyAbilityComp>();
	BaseCharacterAttributeSet = CreateComponent<UBaseCharacterAttributeSet>();
	InteractionComp = CreateComponent<UInteractionComp>();

	// 设置骨骼网格体的碰撞为NoCollision
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	}

	// 设置胶囊体组件为Channel2（自定义游戏轨道），配合线追踪检测
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel2);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
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

	//GetCapsuleComponent()->SetCanEverAffectNavigation(true);

	//todo 去ProjectSetting设置
	//GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
	//GetMesh()->SetNotifyRigidBodyCollision(true);
	//GetMesh()->SetGenerateOverlapEvents(true);

	// 创建导航辅助立方体组件
	NavCubeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NavCubeComponent"));
	NavCubeComponent->SetupAttachment(RootComponent);

	// 设置位置、旋转和缩放
	NavCubeComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
	NavCubeComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	NavCubeComponent->SetRelativeScale3D(FVector(0.12f));
	NavCubeComponent->SetMobility(EComponentMobility::Movable);

	// 在构造函数中加载静态网格
	UStaticMesh* CubeMesh = nullptr;
	if (FindMyObject(CubeMesh, *NavCube_Path))
	{
		NavCubeComponent->SetStaticMesh(CubeMesh);
	}

	// 使立方体透明但启用碰撞
	NavCubeComponent->SetVisibility(false);
	NavCubeComponent->SetCollisionProfileName(TEXT("BlockAll"));
	//NavCubeComponent->SetCollisionProfileName(TEXT("NoCollision"));
	//NavCubeComponent->DestroyComponent();
	
	//GetCapsuleComponent()->SetCanEverAffectNavigation(true);

	GetCapsuleComponent()->SetCollisionProfileName("TacticCharacter");
}

template <typename T>
void ABaseCharacter::FindMyClass(TSubclassOf<T>& YourSubClass, const TCHAR* Path)
{
	if (ConstructorHelpers::FClassFinder<T> ClassFinder(Path); ClassFinder.Succeeded())
	{
		YourSubClass = ClassFinder.Class;
	}
	else
	{
		FString TempStr = FString::Printf(TEXT("Not Found! : %s"), Path);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}
}

template <typename T>
bool ABaseCharacter::FindMyObject(T*& YourObject, const TCHAR* Path)
{
	// 检查我们是否在构造函数中
	if (HasAnyFlags(RF_ClassDefaultObject) || IsTemplate())
	{
		// 使用ConstructorHelpers (仅在构造函数中有效)
		if (ConstructorHelpers::FObjectFinder<T> ObjectFinder(Path); ObjectFinder.Succeeded())
		{
			YourObject = ObjectFinder.Object;
			return true;
		}
	}
	else
	{
		// 运行时加载 (使用StaticLoadObject)
		YourObject = Cast<T>(StaticLoadObject(T::StaticClass(), nullptr, Path));
		if (YourObject)
		{
			return true;
		}
	}

	// 失败处理
	FString TempStr = FString::Printf(TEXT("未找到资源: %s"), Path);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
	UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	return false;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Try to get GameState
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

	MyPlayerController = Cast<ABasePlayerController>(
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
}
