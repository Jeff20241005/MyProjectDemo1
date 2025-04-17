// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticBaseCharacter.h"

#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/FilePaths/DefaultPropertyValue.h"
#include "MyProjectDemo1/FilePaths/FilePaths.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "MyProjectDemo1/UMG/TacticUMG/HealthUI.h"


void ATacticBaseCharacter::TacticMove(FVector MoveLocation)
{
	if (GetMyAbilityComp() && CanMove())
	{
		float MoveRange = BaseCharacterAttributeSet->GetMoveRange();
		BaseAIController->MoveToLocationInTacticMode(
			MoveRange);

		// 修改ActionValues属性值
		BaseCharacterAttributeSet->SetActionValues(BaseCharacterAttributeSet->GetActionValues() - 1.0f);
	}
}

bool ATacticBaseCharacter::CanMove()
{
	return BaseCharacterAttributeSet->GetActionValues() >= 1;
}

void ATacticBaseCharacter::SetWidgetHealth(float Health, float MaxHealth)
{
	if (HealthWidgetComp)
	{
		if (UUserWidget* Widget = HealthWidgetComp->GetUserWidgetObject())
		{
			if (UHealthUI* HealthWidget = Cast<UHealthUI>(Widget))
			{
				HealthWidget->ChangeHealthPercent(Health, MaxHealth);
			}
		}
		/*
		else
		{
			FTimerHandle CustomTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(CustomTimerHandle, [&]()
			{
				if (UUserWidget* UserWidget = HealthWidgetComp->GetUserWidgetObject())
				{
					if (UHealthUI* HealthWidget = Cast<UHealthUI>(UserWidget))
					{
						HealthWidget->ChangeHealthPercent(Health, MaxHealth);
					}
				}
			}, 0.5f, false);
		}
		*/
	}
}

void ATacticBaseCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (MoveRangeStaticMeshComponent)
	{
		MoveRangeStaticMeshComponent->SetScalarParameterValueOnMaterials(MaterialName_Radius, In_Radius);
		MoveRangeStaticMeshComponent->SetVectorParameterValueOnMaterials(
			MaterialName_Color, DefaultColorValueOfSphereSM);
	}
}

void ATacticBaseCharacter::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
}

void ATacticBaseCharacter::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
	// 添加空指针检查
	if (TacticSubsystem)
	{
		TacticSubsystem->OnMyMouseBeginCursorOver.Broadcast(this);
	}
}

void ATacticBaseCharacter::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	// 添加空指针检查
	if (TacticSubsystem)
	{
		TacticSubsystem->OnMyMouseEndCursorOver.Broadcast(this);
	}
}


void ATacticBaseCharacter::Destroyed()
{
	StopHeadIndicatorRotation();
	Super::Destroyed();
	if (TacticSubsystem)
	{
		TacticSubsystem->RemoveCharacterFromTeamByType(this);
	}
}

void ATacticBaseCharacter::ShowMoveRange()
{
	if (MoveRangeStaticMeshComponent)
	{
		FVector Scale = CircleRangeAdjustmentValue_FloatValue * FVector(GetBaseCharacterAttributeSet()->GetMoveRange());
		MoveRangeStaticMeshComponent->SetWorldScale3D(Scale);
		MoveRangeStaticMeshComponent->SetVisibility(true);
	}
}

void ATacticBaseCharacter::CloseMoveRange()
{
	if (MoveRangeStaticMeshComponent)
	{
		MoveRangeStaticMeshComponent->SetVisibility(false);
	}
}


ATacticBaseCharacter::ATacticBaseCharacter()
{
	TeamComp = CreateComponent<UTeamComp>();

	//PrimaryActorTick.bCanEverTick = true;

	// 创建并设置血条UI组件
	HealthWidgetComp = CreateComponent<UWidgetComponent>();
	HealthWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthWidgetComp->SetVisibility(true); // 设置为可见
	HealthWidgetComp->SetRelativeLocation(FVector(0, 0, 115.0f)); // 位置放在角色上方
	HealthWidgetComp->SetWidgetSpace(EWidgetSpace::Screen); // 使用屏幕空间渲染

	// 查找并设置血条Widget蓝图类
	TSubclassOf<UUserWidget> HealthWidgetClass;
	FindMyClass(HealthWidgetClass, *WBP_HealthWidget_Path);
	if (HealthWidgetClass)
	{
		HealthWidgetComp->SetWidgetClass(HealthWidgetClass);
		HealthWidgetComp->SetDrawSize(FVector2D(130.0f, 15.0f)); // 设置绘制大小
	}

	MoveRangeStaticMeshComponent = CreateComponent<UStaticMeshComponent>();
	MoveRangeStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MoveRangeStaticMeshComponent->SetVisibility(false);
	MoveRangeStaticMeshComponent->SetRelativeLocation(FVector(0, 0, CharacterDefaultZHeight_FloatValue + 10.f));
	UStaticMesh* ShowUpSM;
	if (FindMyObject(ShowUpSM, *MoveRangeCircleStaticMeshVisualFeedback_Path))
	{
		MoveRangeStaticMeshComponent->SetStaticMesh(ShowUpSM);
	}

	// 创建头顶图标组件
	HeadIndicatorMeshComponent = CreateComponent<UStaticMeshComponent>();
	HeadIndicatorMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadIndicatorMeshComponent->SetVisibility(false);
	HeadIndicatorMeshComponent->SetRelativeLocation(FVector(0, 0, 180.0f)); // 放在角色头顶
	HeadIndicatorMeshComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f)); // 缩小一些

	// 使用FindMyObject加载静态网格资源
	UStaticMesh* RotationHandleMesh = nullptr;
	UStaticMesh* StartRotationHandleMesh = nullptr;

	if (FindMyObject(RotationHandleMesh, *RotationHandleIndicator_Path))
	{
		RotationHandleIndicatorMesh = RotationHandleMesh;
	}
	if (FindMyObject(StartRotationHandleMesh, *StartRotationHandleIndicator_Path))
	{
		StartRotationHandleIndicatorMesh = StartRotationHandleMesh;
	}
}

void ATacticBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>();
	
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		// 设置极高的旋转速率，实现"瞬时"转向
		MovementComp->RotationRate = FRotator(0.0f, 300.0f, 0.0f); // 极高的Yaw旋转速度

		// 确保角色朝向移动方向
		//MovementComp->bOrientRotationToMovement = true;

		// 禁用控制器旋转影响角色旋转
		//bUseControllerRotationYaw = false;
	}
}

void ATacticBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATacticBaseCharacter::ShowRotationHandleIndicator()
{
	if (HeadIndicatorMeshComponent && RotationHandleIndicatorMesh.Get())
	{
		HeadIndicatorMeshComponent->SetStaticMesh(RotationHandleIndicatorMesh.Get());
		HeadIndicatorMeshComponent->SetVisibility(true);
		StartHeadIndicatorRotation(); // 启动旋转
	}
}

void ATacticBaseCharacter::ShowStartRotationHandleIndicator()
{
	if (HeadIndicatorMeshComponent && StartRotationHandleIndicatorMesh.Get())
	{
		HeadIndicatorMeshComponent->SetStaticMesh(StartRotationHandleIndicatorMesh.Get());
		HeadIndicatorMeshComponent->SetVisibility(true);
		StartHeadIndicatorRotation(); // 启动旋转
	}
}

void ATacticBaseCharacter::HideHeadIndicator()
{
	if (HeadIndicatorMeshComponent)
	{
		HeadIndicatorMeshComponent->SetVisibility(false);
		StopHeadIndicatorRotation();
	}
}

// 添加旋转函数实现
void ATacticBaseCharacter::StartHeadIndicatorRotation()
{
	if (HeadIndicatorMeshComponent && HeadIndicatorMeshComponent->IsVisible())
	{
		// 设置定时器，每帧调用旋转函数
		GetWorld()->GetTimerManager().SetTimer(
			HeadIndicatorRotationTimerHandle,
			this,
			&ATacticBaseCharacter::RotateHeadIndicator,
			0.016f, // 
			true); // 
	}
}

void ATacticBaseCharacter::StopHeadIndicatorRotation()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HeadIndicatorRotationTimerHandle);
	}
}

void ATacticBaseCharacter::RotateHeadIndicator()
{
	if (HeadIndicatorMeshComponent)
	{
		FRotator CurrentRotation = HeadIndicatorMeshComponent->GetRelativeRotation();
		float NewYaw = CurrentRotation.Yaw + (HeadIndicatorRotationSpeed * 0.016f);
		HeadIndicatorMeshComponent->SetRelativeRotation(FRotator(0, NewYaw, 0));
	}
}
