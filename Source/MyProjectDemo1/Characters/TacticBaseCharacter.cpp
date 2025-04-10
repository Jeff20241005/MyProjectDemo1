// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticBaseCharacter.h"

#include "Components/WidgetComponent.h"
#include "MyProjectDemo1/AI/AIControllers/BaseAIController.h"
#include "MyProjectDemo1/FilePaths/DefaultPropertyValue.h"
#include "MyProjectDemo1/FilePaths/FilePaths.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "MyProjectDemo1/UMG/TacticUMG/HealthUI.h"


void ATacticBaseCharacter::Move(FVector MoveLocation)
{
	if (GetMyAbilityComp() && CanMove())
	{
		float MoveRange = BaseCharacterAttributeSet->GetMoveRange();
		BaseAIController->MoveToLocationWithPathFinding(
			MoveLocation, false, MoveRange);

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
	MoveRangeStaticMeshComponent->SetScalarParameterValueOnMaterials(MaterialName_Radius, In_Radius);
	MoveRangeStaticMeshComponent->SetVectorParameterValueOnMaterials(MaterialName_Color, DefaultColorValueOfSphereSM);
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

template <typename T>
bool ATacticBaseCharacter::FindMyObject(T*& YourObject, const TCHAR* Path)
{
	if (ConstructorHelpers::FObjectFinder<T> ObjectFinder(Path); ObjectFinder.Succeeded())
	{
		YourObject = ObjectFinder.Object;
		return true;
	}
	else
	{
		FString TempStr = FString::Printf(TEXT("Not Found! : %s"), Path);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		return false;
	}
}

template <typename T>
void ATacticBaseCharacter::FindMyClass(TSubclassOf<T>& YourSubClass, const TCHAR* Path)
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

ATacticBaseCharacter::ATacticBaseCharacter()
{
	TeamComp = CreateComponent<UTeamComp>();

	//PrimaryActorTick.bCanEverTick = true;

	// 创建并设置血条UI组件
	HealthWidgetComp = CreateComponent<UWidgetComponent>();
	HealthWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthWidgetComp->SetVisibility(true); // 设置为可见
	HealthWidgetComp->SetRelativeLocation(FVector(0, 0, 130.0f)); // 位置放在角色上方
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
	MoveRangeStaticMeshComponent->SetRelativeLocation(FVector(0, 0, CharacterDefaultZHeight_FloatValue));
	UStaticMesh* ShowUpSM;
	if (FindMyObject(ShowUpSM, *MoveRangeCircleStaticMeshVisualFeedback_Path))
	{
		MoveRangeStaticMeshComponent->SetStaticMesh(ShowUpSM);
	}
}

void ATacticBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>();
}

void ATacticBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
