// Fill out your copyright notice in the Description page of Project Settings.


#include "NoctAbility.h"

#include "EnhancedInputComponent.h"
#include "NoctAbilityComponent.h"
#include "GameFramework/Character.h"

UNoctAbility::UNoctAbility()
{
	OwningAbilityComponent = Cast<UNoctAbilityComponent>(GetOuter());
}

// Native Functions
void UNoctAbility::AbilityAddedToOwner()
{
	if (bAutoActivateOnAdd)
	{
		ActivateAbility();
	}
	
	if (ActivationAction)
	{
		if (const auto OwnerPawn = Cast<APawn>(OwningAbilityComponent->GetOwner()))
		{
			if (const auto InputComponent = OwnerPawn->FindComponentByClass<UEnhancedInputComponent>())
			{
				InputComponent->BindAction(ActivationAction, ETriggerEvent::Started, this, &UNoctAbility::InputActionStarted);

				if (bAllowInputTriggered)
				{
					InputComponent->BindAction(ActivationAction, ETriggerEvent::Triggered, this, &UNoctAbility::InputActionTriggered);
				}
				
				InputComponent->BindAction(ActivationAction, ETriggerEvent::Completed, this, &UNoctAbility::InputActionCompleted);
				InputComponent->BindAction(ActivationAction,  ETriggerEvent::Canceled, this, &UNoctAbility::InputActionCancelled);
			}
		}
	}

	AbilityAdded();
	OnAbilityAdded();
}

void UNoctAbility::AbilityRemovedFromOwner()
{
	if (ActivationAction)
	{
		if (const auto OwnerPawn = Cast<APawn>(OwningAbilityComponent->GetOwner()))
		{
			if (const auto InputComponent = OwnerPawn->FindComponentByClass<UEnhancedInputComponent>())
			{
				InputComponent->ClearBindingsForObject(this);
			}
		}
	}
	
	AbilityRemoved();
	OnAbilityRemoved();
}

void UNoctAbility::ActivateAbility()
{
	if(!CanActivateAbility())
	{
		return;
	}

	OwningAbilityComponent->CancelAbilities(CancelTags);
	OwningAbilityComponent->ActiveAbilityTags.AddTag(AbilityTag);
	bIsActive = true;

	AbilityActivated();

	// This check is to ensure the native function did not finish early or get canceled.
	if(bIsActive)
	{
		OnAbilityActivated();
	}
}

void UNoctAbility::FinishAbility()
{
	OwningAbilityComponent->ActiveAbilityTags.RemoveTag(AbilityTag);
	bIsActive = false;
	TriggerCooldown();

	AbilityFinished();
	OnAbilityFinished();
}

void UNoctAbility::CancelAbility()
{
	OwningAbilityComponent->ActiveAbilityTags.RemoveTag(AbilityTag);
	bIsActive = false;

	if(bCooldownOnCancel)
	{
		TriggerCooldown();
	}

	AbilityCancelled();
	OnAbilityCancelled();
}

void UNoctAbility::FinishCooldown()
{
	NativeOnAbilityCooldownFinished();
	OnAbilityCooldownFinished();

	OwningAbilityComponent->AbilityCooldownFinished(this);
}

void UNoctAbility::TriggerCooldown(float CooldownOverride)
{
	float Time = CooldownOverride > 0 ? CooldownOverride : BaseCooldown;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &UNoctAbility::FinishCooldown, Time, false);
}

bool UNoctAbility::CanActivateAbility()
{
	return (
		!OwningAbilityComponent->ActiveAbilityTags.HasAnyExact(BlockingTags)
		&& !CooldownActive()
		&& !bIsActive
		&& !OwningAbilityComponent->BlockedAbilityTags.HasTagExact(AbilityTag)
	);
}

UWorld* UNoctAbility::GetWorld() const
{
	return OwningAbilityComponent ? OwningAbilityComponent->GetWorld() : nullptr;
}

AActor* UNoctAbility::GetOwningActor() const
{
	return OwningAbilityComponent ? OwningAbilityComponent->GetOwner() : nullptr;
}

ACharacter* UNoctAbility::GetOwningCharacter() const
{
	return  OwningAbilityComponent ? Cast<ACharacter>(OwningAbilityComponent->GetOwner()) : nullptr;
}

void UNoctAbility::InputActionStarted(const FInputActionValue& Value)
{
	if (bAutoActivateAbilityOnInputStarted)
	{
		ActivateAbility();
	}
	OnAbilityInputStarted(Value);
}

void UNoctAbility::InputActionTriggered(const FInputActionValue& Value)
{
	OnAbilityInputTriggered(Value);
}

void UNoctAbility::InputActionCompleted(const FInputActionValue& Value)
{
	if (bAutoFinishAbilityOnInputCompleted)
	{
		FinishAbility();
	}
	OnAbilityInputCompleted(Value);
}

void UNoctAbility::InputActionCancelled(const struct FInputActionValue& Value)
{
	if (bAutoCancelAbilityOnInputCancelled)
	{
		CancelAbility();
	}
	OnAbilityInputCancelled(Value);
}

bool UNoctAbility::CooldownActive() const 
{
	return GetWorld()->GetTimerManager().IsTimerActive(CooldownTimer);
}
