// Fill out your copyright notice in the Description page of Project Settings.


#include "NoctAbilityComponent.h"
#include "NoctAbility.h"
#include "NoctEffect.h"

UNoctAbilityComponent::UNoctAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UNoctAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const auto Ability : DefaultAbilities)
	{
		UnlockAbilityByClass(Ability);
	}
}

bool UNoctAbilityComponent::HasAbilityUnlocked(const FGameplayTag TagToCheck) const
{
	return UnlockedAbilityTags.HasTagExact(TagToCheck);
}

bool UNoctAbilityComponent::HasAbilityActive(const FGameplayTag TagToCheck) const
{
	return ActiveAbilityTags.HasTagExact(TagToCheck);
}

bool UNoctAbilityComponent::HasAbilityBlocked(const FGameplayTag TagToCheck) const
{
	return BlockedAbilityTags.HasTagExact(TagToCheck);
}

bool UNoctAbilityComponent::UnlockAbilityByTag(const FGameplayTag AbilityTag)
{
	if(UnlockedAbilityTags.HasTagExact(AbilityTag))
	{
		return false;
	}

	UnlockedAbilityTags.AddTag(AbilityTag);
	return true;
}

bool UNoctAbilityComponent::UnlockAbilityByClass(const TSubclassOf<UNoctAbility> AbilityClass)
{
	bool bValidAbility = false;
	if (AbilityClass)
	{
		if (auto DefaultAbility = AbilityClass->GetDefaultObject<UNoctAbility>())
		{
			bValidAbility = ! UnlockedAbilityTags.HasTagExact(DefaultAbility->AbilityTag);
		}
	}

	if (bValidAbility)
	{
		if (auto NewAbility = NewObject<UNoctAbility>(this, AbilityClass))
		{
			Abilities.Add(NewAbility);
			NewAbility->AbilityAddedToOwner();
			UnlockedAbilityTags.AddTag(NewAbility->AbilityTag);
		}
	}

	return bValidAbility;
}


void UNoctAbilityComponent::ActivateAbilityByTag(const FGameplayTag AbilityTag)
{
	for (auto& Ability : Abilities)
	{
		if(Ability->AbilityTag.MatchesTagExact(AbilityTag) && Ability->CanActivateAbility())
			Ability->ActivateAbility();
	}
}

void UNoctAbilityComponent::CancelAbilityByTag(const FGameplayTag GameplayTag)
{
	for (auto& Ability : Abilities)
	{
		if(Ability->AbilityTag.MatchesTagExact(GameplayTag))
		{
			if(Ability->bIsActive)
				Ability->CancelAbility();
		}
	}
}

void UNoctAbilityComponent::FinishAbilityByTag(const FGameplayTag GameplayTag)
{
	for (auto& Ability : Abilities)
	{
		if(Ability->AbilityTag.MatchesTagExact(GameplayTag))
		{
			if(Ability->bIsActive)
				Ability->FinishAbility();
		}
	}
}

void UNoctAbilityComponent::RemoveAbilityByTag(const FGameplayTag GameplayTag)
{
	for (auto& Ability : Abilities)
	{
		if(Ability->AbilityTag.MatchesTagExact(GameplayTag))
		{
			Ability->AbilityRemovedFromOwner();
			Abilities.Remove(Ability);
			UnlockedAbilityTags.RemoveTag(Ability->AbilityTag);
		}
	}
}

FGameplayTagContainer UNoctAbilityComponent::GetAbilitiesOnCooldown()
{
	FGameplayTagContainer Tags;
	
	for (auto& Ability : Abilities)
	{
		if(Ability->CooldownActive())
		{
			Tags.AddTagFast(Ability->AbilityTag);
		}
	}
	
	return Tags;
}

bool UNoctAbilityComponent::IsAbilityOnCooldown(const FGameplayTag GameplayTag)
{
	for (auto& Ability : Abilities)
	{
		if(GameplayTag == Ability->AbilityTag && Ability->CooldownActive())
		{
			return true;
		}
	}

	return false;
}

bool UNoctAbilityComponent::BlockAbilities(const FGameplayTagContainer AbilityTag)
{
	if(BlockedAbilityTags.HasAll(AbilityTag))
	{
		return false;
	}

	BlockedAbilityTags.AppendTags(AbilityTag);
	return true;
}

void UNoctAbilityComponent::UnblockAbility(const FGameplayTag AbilityTag)
{
	if(HasEffectBlocked(AbilityTag))
	{
		BlockedAbilityTags.RemoveTag(AbilityTag);
	}
}

void UNoctAbilityComponent::UnblockAbilities(const FGameplayTagContainer AbilityTags)
{
	if(BlockedAbilityTags.HasAnyExact(AbilityTags))
	{
		BlockedAbilityTags.RemoveTags(AbilityTags);
	}
}

void UNoctAbilityComponent::CancelAbilities(const FGameplayTagContainer AbilityTag)
{
	for (const auto& Ability : Abilities)
	{
		if(Ability->AbilityTag.MatchesAny(AbilityTag))
		{
			if(Ability->bIsActive)
				Ability->CancelAbility();
		}
	}
}

bool UNoctAbilityComponent::HasEffectActive(const FGameplayTag TagToCheck) const
{
	return ActiveEffectsTags.HasTagExact(TagToCheck);
}

bool UNoctAbilityComponent::HasEffectBlocked(const FGameplayTag TagToCheck) const
{
	return BlockedEffectsTags.HasTagExact(TagToCheck);
}

bool UNoctAbilityComponent::AddEffectByClass(const TSubclassOf<UNoctEffect> EffectToAdd)
{
	if(ActiveEffectsTags.HasTagExact(EffectToAdd.GetDefaultObject()->EffectTag))
	{
		return false;
	}

	const auto NewEffect = NewObject<UNoctEffect>(this, EffectToAdd);

	if(NewEffect)
	{
		ActiveEffects.Add(NewEffect);
	}

	NewEffect->EffectApplied();
	
	return NewEffect != nullptr;	
}

void UNoctAbilityComponent::RemoveEffect(UNoctEffect* NoctEffect)
{
	if(NoctEffect->bIsActiveAndApplied)
	{
		NoctEffect->EffectRemoved(); 
	}
	
	ActiveEffects.Remove(NoctEffect);
}

void UNoctAbilityComponent::GetCooldownRemainingForAbility(const FGameplayTag AbilityTag, float& TimeRemaining, float& CooldownDuration)
{
	if(IsAbilityOnCooldown(AbilityTag))
	{
		for (const auto& Ability : Abilities)
		{
			if(Ability->AbilityTag.MatchesTagExact(AbilityTag))
			{
				TimeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(Ability->CooldownTimer);
				CooldownDuration = Ability->BaseCooldown;
				return;
			}
		}
	}
}

void UNoctAbilityComponent::SetCooldownRemainingForAbility(const FGameplayTag AbilityTag, const float NewTime)
{
	for (const auto& Ability : Abilities)
	{
		if (Ability->AbilityTag.MatchesTagExact(AbilityTag))
		{
			Ability->TriggerCooldown(NewTime);
			return;
		}
	}
}

void UNoctAbilityComponent::AbilityCooldownFinished(UNoctAbility* NoctAbility)
{
	
}

bool UNoctAbilityComponent::RemoveEffectByTagExact(const FGameplayTag EffectTag)
{
	if(ActiveEffectsTags.HasTagExact(EffectTag))
	{
		for (const auto& Effect : ActiveEffects)
		{
			if(Effect->EffectTag.MatchesTagExact(EffectTag))
			{
				RemoveEffect(Effect);
				return true;
			}
		}
	}
	return false;
}

bool UNoctAbilityComponent::RemoveEffectByTag(const FGameplayTag EffectTag)
{
	if(ActiveEffectsTags.HasTagExact(EffectTag))
	{
		for (const auto& Effect : ActiveEffects)
		{
			if(Effect->EffectTag.MatchesTagExact(EffectTag))
			{
				RemoveEffect(Effect);
				return true;
			}
		}
	}
	return false;
}

bool UNoctAbilityComponent::AddAttribute(const FGameplayTag AttributeTag, FNoctAttribute Attribute)
{
	if(!AttributeTags.HasTagExact(AttributeTag))
	{
		Attributes.Add(AttributeTag, Attribute);
		AttributeTags.AddTagFast(AttributeTag);
		return true;
	}
	return false;
}

#ifdef USE_EASY_MULTI_SAVE

void UNoctAbilityComponent::ActorLoaded_Implementation()
{

}

void UNoctAbilityComponent::ActorPreSave_Implementation()
{

}

void UNoctAbilityComponent::ActorSaved_Implementation()
{

}

void UNoctAbilityComponent::ComponentsToSave_Implementation(TArray<UActorComponent*>& Components)
{

}

#endif