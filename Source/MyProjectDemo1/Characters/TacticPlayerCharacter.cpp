// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticPlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"

// Sets default values
ATacticPlayerCharacter::ATacticPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	//WidgetComponent->SetVisibility(false);
	//PathTracer = CreateDefaultSubobject<UActorComponent_PathTracer>("PathTracer");

	//SetupCharacterDataFromBP();
}



void ATacticPlayerCharacter::SetupCharacterDataFromBP()
{
	static ConstructorHelpers::FObjectFinder<UUserWidget> MainMenuWidgetFinder(
		TEXT("/Game/UI/WidgetBP_CharacterActionMenu"));
}

// Called when the game starts or when spawned
void ATacticPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	TacticSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTacticSubsystem>();
	
	//todo test delete
	if (TacticSubsystem->OnSwitchCharacterAction.IsBound())
	{
		TacticSubsystem->OnSwitchCharacterAction.Broadcast(this);
	}
}

// Called every frame
void ATacticPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*test
float ATacticPlayerCharacter::GetValueByName(FString Name)
{
	if (BaseCharacterAttributeSet)
	{
		FGameplayAttribute Attribute;
		FName AttributeName = FName(*Name);
		static FProperty* Property = FindFieldChecked<FProperty>(UBaseCharacterAttributeSet::StaticClass(),
		                                                         AttributeName);
		Attribute = FGameplayAttribute(Property);
		if (Attribute.IsValid())
		{
			return Attribute.GetNumericValue(BaseCharacterAttributeSet);
		}
	}
	return 0.0f;
}
*/