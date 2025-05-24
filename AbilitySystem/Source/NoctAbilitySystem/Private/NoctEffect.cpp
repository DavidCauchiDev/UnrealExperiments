// Fill out your copyright notice in the Description page of Project Settings.


#include "NoctEffect.h"
#include "NoctAbilityComponent.h"

UNoctEffect::UNoctEffect()
{
	OwningAbilityComponent = Cast<UNoctAbilityComponent>(GetOuter());
}

void UNoctEffect::EffectDurationCompleted()
{
	OwningAbilityComponent->RemoveEffect(this);
}

bool UNoctEffect::EffectApplied()
{
	if(bPermanent)
	{
		NativeEffectTriggered();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(EffectDurationTimerHandle, this, &UNoctEffect::EffectDurationCompleted, Duration, false);
		GetWorld()->GetTimerManager().SetTimer(EffectTimerHandle, this, &UNoctEffect::NativeEffectTriggered, TriggerInterval, true);
	}
	
	OnEffectApplied();

	bIsActiveAndApplied = true;

	OwningAbilityComponent->CancelAbilities(AbilitiesToCancel);
	
	return true;
}

void UNoctEffect::EffectRemoved()
{
	// If the effect interval is still running, and close to 0, clear it and trigger the effect one last time.
	// This is to avoid the effect being removed before the last interval is triggered.
	if(FMath::IsNearlyZero(GetWorld()->GetTimerManager().GetTimerRemaining(EffectTimerHandle)))
	{
		NativeEffectTriggered();
	}

	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	
	OnEffectRemoved();
	bIsActiveAndApplied = false;
}

void UNoctEffect::NativeEffectTriggered()
{
	OnEffectTriggered();
}

UWorld* UNoctEffect::GetWorld() const
{
	return OwningAbilityComponent ? OwningAbilityComponent->GetWorld() : nullptr;
}