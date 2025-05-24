// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "NoctAttribute.h"

#ifdef USE_EASY_MULTI_SAVE
#include <EMSActorSaveInterface.h>
#endif


#include "NoctAbilityComponent.generated.h"


class UNoctEffect;
class UNoctAbility;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NOCTABILITYSYSTEM_API UNoctAbilityComponent : public UActorComponent
#ifdef USE_EASY_MULTI_SAVE
	, public IEMSActorSaveInterface
#endif
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNoctAbilityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void AbilityCooldownFinished(UNoctAbility* NoctAbility);

	// Abilities
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	TArray<TSubclassOf<UNoctAbility>> DefaultAbilities;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem", SaveGame)
	FGameplayTagContainer UnlockedAbilityTags;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem", SaveGame)
	FGameplayTagContainer BlockedAbilityTags;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem", SaveGame)
	FGameplayTagContainer ActiveAbilityTags;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem", SaveGame)
	TArray<TObjectPtr<UNoctAbility>> Abilities;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem", SaveGame)
	TArray<TObjectPtr<UNoctEffect>> ActiveEffects;
	
	UFUNCTION(BlueprintPure)
	bool HasAbilityUnlocked(FGameplayTag TagToCheck) const;

	UFUNCTION(BlueprintPure)
	bool HasAbilityActive(FGameplayTag TagToCheck) const;

	UFUNCTION(BlueprintPure)
	bool HasAbilityBlocked(FGameplayTag TagToCheck) const;

	UFUNCTION(BlueprintCallable)
	bool UnlockAbilityByClass(TSubclassOf<UNoctAbility> AbilityClass);

	UFUNCTION(BlueprintCallable)
	void ActivateAbilityByTag(FGameplayTag AbilityTag);

	UFUNCTION(BlueprintCallable)
	void CancelAbilityByTag(FGameplayTag GameplayTag);

	UFUNCTION(BlueprintCallable)
	void FinishAbilityByTag(FGameplayTag GameplayTag);

	UFUNCTION(BlueprintCallable)
	void RemoveAbilityByTag(FGameplayTag GameplayTag);

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetAbilitiesOnCooldown();

	UFUNCTION(BlueprintCallable)
	bool IsAbilityOnCooldown(FGameplayTag GameplayTag);
	
	// Useful for unlocking features that are not raw abilities. Such as a "double jump" feature.
	// No point in creating a whole new ability for most things and causing more clutter.
	UFUNCTION(BlueprintCallable)
	bool UnlockAbilityByTag(FGameplayTag AbilityTag);
	
	UFUNCTION(BlueprintCallable)
	bool BlockAbilities(FGameplayTagContainer AbilityTag);

	UFUNCTION(BlueprintCallable)
	void UnblockAbility(FGameplayTag AbilityTag);

	UFUNCTION(BlueprintCallable)
	void UnblockAbilities(FGameplayTagContainer AbilityTags);
	

	UFUNCTION(BlueprintCallable)
	void CancelAbilities(FGameplayTagContainer AbilityTag);

	UFUNCTION(BlueprintCallable)
	void GetCooldownRemainingForAbility(FGameplayTag AbilityTag, float& TimeRemaining, float& CooldownDuration);
	
	UFUNCTION(BlueprintCallable)
	void SetCooldownRemainingForAbility(FGameplayTag AbilityTag, float NewTime);


	// Effects
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem", SaveGame)
	FGameplayTagContainer BlockedEffectsTags;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem", SaveGame)
	FGameplayTagContainer ActiveEffectsTags;
	
	UFUNCTION(BlueprintPure)
	bool HasEffectActive(FGameplayTag TagToCheck) const;

	UFUNCTION(BlueprintPure)
	bool HasEffectBlocked(FGameplayTag TagToCheck) const;
	
	UFUNCTION(BlueprintCallable)
	bool AddEffectByClass(TSubclassOf<UNoctEffect> EffectToAdd);

	// Useful to remove effects that are unique, such as "small burn" or "large burn"
	UFUNCTION(BlueprintCallable)
	bool RemoveEffectByTagExact(FGameplayTag EffectTag);

	// Useful to remove effects that are not unique, such as all "burn" effects
	UFUNCTION(BlueprintCallable)
	bool RemoveEffectByTag(FGameplayTag EffectTag);

	void RemoveEffect(UNoctEffect* NoctEffect);

	// Attributes
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem", SaveGame)
	TMap<FGameplayTag, FNoctAttribute> Attributes;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem", SaveGame)
	FGameplayTagContainer AttributeTags;

	bool AddAttribute(FGameplayTag AttributeTag, FNoctAttribute Attribute);

#ifdef USE_EASY_MULTI_SAVE
	// Save Interface
	virtual void ActorLoaded_Implementation() override;
	virtual void ActorPreSave_Implementation() override;
	virtual void ActorSaved_Implementation() override;
	virtual void ComponentsToSave_Implementation(TArray<UActorComponent*>& Components) override;
#endif
};
