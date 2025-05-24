// Copyright Nocturnum Games 2023 


#include "NoctAttribute.h"

void FNoctAttribute::AddModifier(const FNoctAttributeModifier& Modifier)
{
	TArray<FNoctAttributeModifier>& ModifiersArray = Modifier.bIsPercentage ? PercentModifiers : FlatModifiers;
	
	// Check if we need to handle stacking
	if (!Modifier.StackTag.IsValid())
	{
		// No stacking, just add the modifier
		ModifiersArray.Add(Modifier);
		CurrentValue = CalculateValue();
		return;
	}
	
	// Find existing modifiers with the same stack tag
	for (int32 i = 0; i < ModifiersArray.Num(); ++i)
	{
		FNoctAttributeModifier& ExistingMod = ModifiersArray[i];
		
		if (ExistingMod.StackTag == Modifier.StackTag)
		{
			// Handle the stacking based on the policy
			switch (Modifier.StackingPolicy)
			{
				case EModifierStackingPolicy::Stack_Add:
				{
					// If max stacks is defined and we've reached it, don't stack more
					if (ExistingMod.MaxStacks > 0 && ExistingMod.StackCount >= ExistingMod.MaxStacks)
					{
						// Recalculate and return without adding
						CurrentValue = CalculateValue();
						return;
					}
					
					// Add to stack count
					ExistingMod.StackCount++;
					
					// Increase the value
					ExistingMod.Value += Modifier.Value;
					
					// Copy over any new tags
					ExistingMod.Tags.AppendTags(Modifier.Tags);
					
					// Recalculate and return
					CurrentValue = CalculateValue();
					return;
				}
				case EModifierStackingPolicy::Stack_Max:
				{
					// Keep only the highest value
					if (Modifier.Value > ExistingMod.Value)
					{
						ExistingMod.Value = Modifier.Value;
						ExistingMod.Source = Modifier.Source;
						ExistingMod.Tags = Modifier.Tags;
						ExistingMod.StackCount++;
					}
					
					// Recalculate and return
					CurrentValue = CalculateValue();
					return;
				}
				case EModifierStackingPolicy::Stack_Min:
				{
					// Keep only the lowest value
					if (Modifier.Value < ExistingMod.Value)
					{
						ExistingMod.Value = Modifier.Value;
						ExistingMod.Source = Modifier.Source;
						ExistingMod.Tags = Modifier.Tags;
						ExistingMod.StackCount++;
					}
					
					// Recalculate and return
					CurrentValue = CalculateValue();
					return;
				}
				case EModifierStackingPolicy::Stack_Replace:
				{
					// Just replace the existing one
					ExistingMod = Modifier;
					ExistingMod.StackCount = 1;
					
					// Recalculate and return
					CurrentValue = CalculateValue();
					return;
				}
				case EModifierStackingPolicy::Stack_None:
				default:
					// Don't stack, just add as a new modifier
					break;
			}
		}
	}
	
	// If we reached here, either the modifier has Stack_None policy,
	// or no existing modifier with the same tag was found
	ModifiersArray.Add(Modifier);
	
	// Recalculate the current value
	CurrentValue = CalculateValue();
}

void FNoctAttribute::RemoveModifiersBySource(UObject* Source)
{
	bool bModified = false;
	
	// Remove flat modifiers from this source
	for (int32 i = FlatModifiers.Num() - 1; i >= 0; --i)
	{
		if (FlatModifiers[i].Source == Source)
		{
			FlatModifiers.RemoveAt(i);
			bModified = true;
		}
	}
	
	// Remove percent modifiers from this source
	for (int32 i = PercentModifiers.Num() - 1; i >= 0; --i)
	{
		if (PercentModifiers[i].Source == Source)
		{
			PercentModifiers.RemoveAt(i);
			bModified = true;
		}
	}
	
	// Only recalculate if we removed something
	if (bModified)
	{
		CurrentValue = CalculateValue();
	}
}

void FNoctAttribute::RemoveModifierById(const FGuid& ModifierId)
{
	bool bModified = false;
	
	// Look for the modifier in flat modifiers
	for (int32 i = 0; i < FlatModifiers.Num(); ++i)
	{
		if (FlatModifiers[i].Id == ModifierId)
		{
			FlatModifiers.RemoveAt(i);
			bModified = true;
			break;  // IDs should be unique
		}
	}
	
	// If not found, check percent modifiers
	if (!bModified)
	{
		for (int32 i = 0; i < PercentModifiers.Num(); ++i)
		{
			if (PercentModifiers[i].Id == ModifierId)
			{
				PercentModifiers.RemoveAt(i);
				bModified = true;
				break;  // IDs should be unique
			}
		}
	}
	
	// Only recalculate if we removed something
	if (bModified)
	{
		CurrentValue = CalculateValue();
	}
}

void FNoctAttribute::RemoveModifiersByTag(const FGameplayTag& Tag)
{
	bool bModified = false;
	
	// Remove flat modifiers with this tag
	for (int32 i = FlatModifiers.Num() - 1; i >= 0; --i)
	{
		if (FlatModifiers[i].HasTag(Tag))
		{
			FlatModifiers.RemoveAt(i);
			bModified = true;
		}
	}
	
	// Remove percent modifiers with this tag
	for (int32 i = PercentModifiers.Num() - 1; i >= 0; --i)
	{
		if (PercentModifiers[i].HasTag(Tag))
		{
			PercentModifiers.RemoveAt(i);
			bModified = true;
		}
	}
	
	// Only recalculate if we removed something
	if (bModified)
	{
		CurrentValue = CalculateValue();
	}
}

void FNoctAttribute::RemoveModifiersByTags(const FGameplayTagContainer& Tags)
{
	bool bModified = false;
	
	// Remove flat modifiers with any of these tags
	for (int32 i = FlatModifiers.Num() - 1; i >= 0; --i)
	{
		if (FlatModifiers[i].HasAnyTags(Tags))
		{
			FlatModifiers.RemoveAt(i);
			bModified = true;
		}
	}
	
	// Remove percent modifiers with any of these tags
	for (int32 i = PercentModifiers.Num() - 1; i >= 0; --i)
	{
		if (PercentModifiers[i].HasAnyTags(Tags))
		{
			PercentModifiers.RemoveAt(i);
			bModified = true;
		}
	}
	
	// Only recalculate if we removed something
	if (bModified)
	{
		CurrentValue = CalculateValue();
	}
}

void FNoctAttribute::RemoveStack(const FGameplayTag& StackTag)
{
	if (!StackTag.IsValid())
	{
		return;
	}
	
	bool bModified = false;
	
	// Check flat modifiers first
	for (int32 i = 0; i < FlatModifiers.Num(); ++i)
	{
		FNoctAttributeModifier& Modifier = FlatModifiers[i];
		if (Modifier.StackTag == StackTag)
		{
			// Decrease stack count
			Modifier.StackCount--;
			
			if (Modifier.StackCount <= 0)
			{
				// Remove the modifier if no stacks left
				FlatModifiers.RemoveAt(i);
			}
			else if (Modifier.StackingPolicy == EModifierStackingPolicy::Stack_Add)
			{
				// Decrease the value for additive stacks
				// For simplicity, we assume each stack adds the same amount
				Modifier.Value = (Modifier.Value / (Modifier.StackCount + 1)) * Modifier.StackCount;
			}
			
			bModified = true;
			break;
		}
	}
	
	// If not found in flat modifiers, check percent modifiers
	if (!bModified)
	{
		for (int32 i = 0; i < PercentModifiers.Num(); ++i)
		{
			FNoctAttributeModifier& Modifier = PercentModifiers[i];
			if (Modifier.StackTag == StackTag)
			{
				// Decrease stack count
				Modifier.StackCount--;
				
				if (Modifier.StackCount <= 0)
				{
					// Remove the modifier if no stacks left
					PercentModifiers.RemoveAt(i);
				}
				else if (Modifier.StackingPolicy == EModifierStackingPolicy::Stack_Add)
				{
					// Decrease the value for additive stacks
					// For simplicity, we assume each stack adds the same amount
					Modifier.Value = (Modifier.Value / (Modifier.StackCount + 1)) * Modifier.StackCount;
				}
				
				bModified = true;
				break;
			}
		}
	}
	
	// Only recalculate if we modified something
	if (bModified)
	{
		CurrentValue = CalculateValue();
	}
}

float FNoctAttribute::CalculateValue()
{
	// Start with the base value
	float FinalValue = BaseValue;
	
	// Add all flat modifiers
	for (const FNoctAttributeModifier& Modifier : FlatModifiers)
	{
		FinalValue += Modifier.Value;
	}
	
	// Apply all percentage modifiers
	for (const FNoctAttributeModifier& Modifier : PercentModifiers)
	{
		FinalValue *= (1.0f + Modifier.Value);
	}
	
	// Cap at max value if positive
	if (MaxValue > 0 && FinalValue > MaxValue)
	{
		FinalValue = MaxValue;
	}
	
	return FinalValue;
}

void FNoctAttribute::Initialize(float InBaseValue, float InMaxValue)
{
	BaseValue = InBaseValue;
	
	// Only update max value if a valid value was provided
	if (InMaxValue > 0)
	{
		MaxValue = InMaxValue;
	}
	
	// Clear all modifiers
	FlatModifiers.Empty();
	PercentModifiers.Empty();
	
	// Set current value to base value initially
	CurrentValue = BaseValue;
}

float FNoctAttribute::GetModifierValue() const
{
	return CurrentValue - BaseValue;
}

int32 FNoctAttribute::GetStackCount(const FGameplayTag& StackTag) const
{
	if (!StackTag.IsValid())
	{
		return 0;
	}
	
	// Check flat modifiers first
	for (const FNoctAttributeModifier& Modifier : FlatModifiers)
	{
		if (Modifier.StackTag == StackTag)
		{
			return Modifier.StackCount;
		}
	}
	
	// Then check percent modifiers
	for (const FNoctAttributeModifier& Modifier : PercentModifiers)
	{
		if (Modifier.StackTag == StackTag)
		{
			return Modifier.StackCount;
		}
	}
	
	// No stacks found
	return 0;
}

TArray<FNoctAttributeModifier> FNoctAttribute::GetModifiersWithTag(const FGameplayTag& Tag) const
{
	TArray<FNoctAttributeModifier> Result;
	
	// Check flat modifiers
	for (const FNoctAttributeModifier& Modifier : FlatModifiers)
	{
		if (Modifier.HasTag(Tag))
		{
			Result.Add(Modifier);
		}
	}
	
	// Check percent modifiers
	for (const FNoctAttributeModifier& Modifier : PercentModifiers)
	{
		if (Modifier.HasTag(Tag))
		{
			Result.Add(Modifier);
		}
	}
	
	return Result;
}

TArray<FNoctAttributeModifier> FNoctAttribute::GetModifiersWithAnyTags(const FGameplayTagContainer& Tags) const
{
	TArray<FNoctAttributeModifier> Result;
	
	// Check flat modifiers
	for (const FNoctAttributeModifier& Modifier : FlatModifiers)
	{
		if (Modifier.HasAnyTags(Tags))
		{
			Result.Add(Modifier);
		}
	}
	
	// Check percent modifiers
	for (const FNoctAttributeModifier& Modifier : PercentModifiers)
	{
		if (Modifier.HasAnyTags(Tags))
		{
			Result.Add(Modifier);
		}
	}
	
	return Result;
}
