// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyProjectDemo1/Components/TeamComp.h"
#include "MyProjectDemo1/Framework/Controllers/MyPlayerController.h"


void APlayerCharacter::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	MyPlayerController->CurrentControlPlayer = this;
	// 切换到玩家控制
	MyPlayerController->Possess(this);
}

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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

	// 设置为玩家团队
	TeamComp->SetTeam(ETeamType::ETT_Player);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
