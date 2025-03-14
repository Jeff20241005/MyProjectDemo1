// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAbilityComp.h"

#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Framework/GameStates/TacticGameState.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"


UMyAbilityComp::UMyAbilityComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

/* todo put it in buttonbase
for (auto Spec : GetActivatableAbilities())
	{
		if (UBaseAbility* Ability = Cast<UBaseAbility>(Spec.Ability))
		{
			if (UButtonBase* SkillWidget = CreateWidget<UButtonBase>(GetWorld(), DefaultSkillClass))
			{
				//---也能输入技能图像
				SkillWidget->SetupButton(Ability->AbilityName.ToString());
				SkillWidget->SetupButtonOnClick(WidgetOwner, Ability);
				SkillListBox->AddChild(SkillWidget);
			}
		}
	}
 */
void UMyAbilityComp::BeginPlay()
{
	AutoGiveAbilitiesAndEffectsAtStart();
	Owner_BaseCharacter = Cast<ABaseCharacter>(GetOwner());
	Super::BeginPlay();

	TacticGameState = Cast<ATacticGameState>(UGameplayStatics::GetGameState(GetWorld()));
}


void UMyAbilityComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UMyAbilityComp::ReceiveDamage(UMyAbilityComp* SourceAbilityComp, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceAbilityComp, UnmitigatedDamage, MitigatedDamage);
}

void UMyAbilityComp::AutoGiveAbilitiesAndEffectsAtStart()
{
	if (!bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server
		for (const TSubclassOf<UBaseAbility>& StartupAbility : GameplayAbilities)
		{
			if (StartupAbility)
			{
				GiveAbility(FGameplayAbilitySpec(
					StartupAbility,
					1, // Level 1
					-1, //  input ID
					this // Source is the current character
				));
			}
		}
	}

	// Now apply passives
	for (const TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
	{
		if (GameplayEffect)
		{
			FGameplayEffectContextHandle EffectContext = MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = MakeOutgoingSpec(
				GameplayEffect, 1, EffectContext // Level 1 and the effect context
			);

			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle =
					ApplyGameplayEffectSpecToTarget(
						*NewHandle.Data.Get(),
						this
					);
			}
		}
	}
	bAbilitiesInitialized = true;
}

void UMyAbilityComp::SelectSkillAbility(UBaseAbility* BaseAbility)
{
	CurrentSelectAbility = BaseAbility;
	BeginSpawnAttackRange();
	
}

void UMyAbilityComp::BeginSpawnAttackRange()
{
	FoundTargets = ScanTargets(Owner_BaseCharacter->GetActorLocation(), CurrentSelectAbility->AttackRange,
	                           CurrentSelectAbility->SkillRangeType);
}

void UMyAbilityComp::CancelSpawnAttackRange()
{
	/*for (auto TargetsRef : FoundTargets)
	{
		TargetsRef->UpdateCharacterMarkingBrightness();
		TargetsRef->UpdateCharacterMarkingColor();
		TargetsRef->DisplayHealthWidget(false);
		TargetsRef->UpdateIsMark(false);
		TargetsRef->CloseWidget();
	}*/
}


TArray<ABaseCharacter*> UMyAbilityComp::ScanTargets(FVector TargetLocation/*todo pass MousePosition or ActorLocation*/,
                                                    float InRange,
                                                    TEnumAsByte<EAttackRangeType> InEAttackRange)
{
	TArray<ABaseCharacter*> Targets;
	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	FVector StartLocation = TargetLocation + FVector(0, 0, 2000);
	FVector EndLocation = TargetLocation + FVector(0, 0, -85);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Owner_BaseCharacter);

	//
	FCollisionShape CollisionShape;
	switch (InEAttackRange)
	{
	case EAR_Circle:
		FCollisionShape::MakeSphere(InRange / 2);
		break;
	case EAR_Box:
		FVector BoxExtent(InRange, InRange, 1.0f);
		CollisionShape = FCollisionShape::MakeBox(BoxExtent);
		break;
	case EAR_Segment:
		break;
	default: ;
	}

	if (GetWorld()->SweepMultiByObjectType(HitResults, StartLocation, EndLocation, FQuat::Identity, ObjectParams,
	                                       CollisionShape, CollisionParams))
	{
		for (TIndexedContainerIterator<TArray<FHitResult>, TArray<FHitResult>::ElementType, TArray<
			                               FHitResult>::SizeType> It = HitResults.CreateIterator(); It; It++)
		{
			if (ABaseCharacter* HitTargetBaseCharacter = Cast<ABaseCharacter>(It->GetActor()))
			{
				Targets.AddUnique(HitTargetBaseCharacter);
			}
		}
		//---锁定最近的敌人---------------------------
	}

	return Targets;
}
