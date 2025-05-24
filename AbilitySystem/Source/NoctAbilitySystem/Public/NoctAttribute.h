// Copyright Nocturnum Games 2023 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "NoctAttribute.generated.h"

/**
 * Defines how modifiers with the same stack key should be combined
 */
UENUM(BlueprintType)
enum class EModifierStackingPolicy : uint8
{
	// Stack modifiers by adding their values (default)
	Stack_Add UMETA(DisplayName = "Add Values"), 
	
	// Only apply the highest value modifier
	Stack_Max UMETA(DisplayName = "Use Highest Value"),
	
	// Only apply the lowest value modifier
	Stack_Min UMETA(DisplayName = "Use Lowest Value"),
	
	// Replace the existing modifier
	Stack_Replace UMETA(DisplayName = "Replace"),
	
	// Don't stack, maintain separate modifiers
	Stack_None UMETA(DisplayName = "No Stacking")
};

/**
 * Represents a modifier (buff/debuff) applied to an attribute
 */
USTRUCT(BlueprintType)
struct FNoctAttributeModifier
{
	GENERATED_BODY()

	// Unique identifier for the modifier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	FGuid Id = FGuid::NewGuid();
	
	// The value of the modifier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	float Value = 0.0f;

	// If true, the modifier is a percentage (0.1 = 10% increase)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	bool bIsPercentage = false;

	// The source of the modifier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	UObject* Source = nullptr;
	
	// Tags that define this modifier (can be used for categorization, filtering, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	FGameplayTagContainer Tags;
	
	// Tag that identifies modifiers that should stack together
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	FGameplayTag StackTag;
	
	// Controls how this modifier stacks with others with the same StackTag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	EModifierStackingPolicy StackingPolicy = EModifierStackingPolicy::Stack_Add;
	
	// How many times this modifier has been stacked
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	int32 StackCount = 1;
	
	// Maximum number of stacks allowed (0 = unlimited)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	int32 MaxStacks = 0;
	
	FNoctAttributeModifier()
	{
		Id = FGuid::NewGuid();
	}
	
	FNoctAttributeModifier(float InValue, bool bInIsPercentage, UObject* InSource = nullptr)
		: Value(InValue), bIsPercentage(bInIsPercentage), Source(InSource)
	{
		Id = FGuid::NewGuid();
	}
	
	// Add tag to this modifier
	void AddTag(const FGameplayTag& Tag)
	{
		Tags.AddTag(Tag);
	}
	
	// Check if the modifier has the specified tag
	bool HasTag(const FGameplayTag& Tag) const
	{
		return Tags.HasTag(Tag);
	}
	
	// Check if modifier has any of the specified tags
	bool HasAnyTags(const FGameplayTagContainer& InTags) const
	{
		return Tags.HasAnyExact(InTags);
	}
	
	// Check if modifier has all of the specified tags
	bool HasAllTags(const FGameplayTagContainer& InTags) const
	{
		return Tags.HasAllExact(InTags);
	}
};

USTRUCT(Blueprintable)
struct FNoctAttribute
{
	GENERATED_BODY()
 
	// The maximum value of the attribute
	UPROPERTY(EditAnywhere, Category = "NoctAbilitySystem")
	float MaxValue = 1;

	// The current value after all modifiers
	UPROPERTY(EditAnywhere, Category = "NoctAbilitySystem")
	float CurrentValue = 1;
	
	// The base value without any modifiers
	UPROPERTY(EditAnywhere, Category = "NoctAbilitySystem")
	float BaseValue = 1;
	
	// Flat modifiers that are added directly to the base value
	UPROPERTY(EditAnywhere, Category = "NoctAbilitySystem")
	TArray<FNoctAttributeModifier> FlatModifiers;
	
	// Percentage modifiers that scale the value after flat modifiers
	UPROPERTY(EditAnywhere, Category = "NoctAbilitySystem")
	TArray<FNoctAttributeModifier> PercentModifiers;
	
	// Add a modifier to the attribute
	void AddModifier(const FNoctAttributeModifier& Modifier);
	
	// Remove modifiers by source
	void RemoveModifiersBySource(UObject* Source);
	
	// Remove a specific modifier by ID
	void RemoveModifierById(const FGuid& ModifierId);
	
	// Remove modifiers with a specific tag
	void RemoveModifiersByTag(const FGameplayTag& Tag);
	
	// Remove modifiers matching any of the tags
	void RemoveModifiersByTags(const FGameplayTagContainer& Tags);
	
	// Remove one stack from a stacking modifier
	void RemoveStack(const FGameplayTag& StackTag);
	
	// Calculate the final value based on base and all modifiers
	float CalculateValue();
	
	// Initialize with a specific base value
	void Initialize(float InBaseValue, float InMaxValue = -1);
	
	// Get the combined value of all modifiers
	float GetModifierValue() const;
	
	// Get the stack count for a specific stacking modifier
	int32 GetStackCount(const FGameplayTag& StackTag) const;
	
	// Get modifiers with a specific tag
	TArray<FNoctAttributeModifier> GetModifiersWithTag(const FGameplayTag& Tag) const;
	
	// Get modifiers that match any of the specified tags
	TArray<FNoctAttributeModifier> GetModifiersWithAnyTags(const FGameplayTagContainer& Tags) const;
};
