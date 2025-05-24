// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "NoctEffect.generated.h"

class UNoctAbilityComponent;

UENUM(BlueprintType)
enum class ENoctAttributeOperation : uint8
{
	// Add the magnitude to the attribute value
	Add = 0      UMETA(DisplayName = "Add"),
	// Subtract the magnitude from the attribute value
	Subtract     UMETA(DisplayName = "Subtract"),
	// Multiply the attribute value by the magnitude
	Multiply     UMETA(DisplayName = "Multiply"),
	// Divide the attribute value by the magnitude
	Divide       UMETA(DisplayName = "Divide"),
	// Set the attribute value to the magnitude
	Set          UMETA(DisplayName = "Set")
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class NOCTABILITYSYSTEM_API UNoctEffect : public UObject
{
	GENERATED_BODY()

	UNoctEffect();
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnEffectApplied();

	UFUNCTION(BlueprintImplementableEvent)
	void OnEffectRemoved();
	
	// Implement this in BP to do something when the effect is triggered.
	// Can be one shot or triggered every x time over total duration (do damage every second for 10 seconds).
	UFUNCTION(BlueprintImplementableEvent)
	void OnEffectTriggered();

	UFUNCTION()
	void EffectDurationCompleted();
	
	UFUNCTION()
	bool EffectApplied();

	UFUNCTION()
	void EffectRemoved();
	
	UFUNCTION()
	void NativeEffectTriggered();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem|Magnitude")
	UCurveFloat* MagnitudeScalingCurve = nullptr;
	
	/** Calculate the final magnitude of the effect with level scaling */
	UFUNCTION(BlueprintCallable, Category = "NoctAbilitySystem|Magnitude")
	float GetFinalMagnitude() const
	{
		// If we have a curve, use it for scaling based on level
		if (MagnitudeScalingCurve)
		{
			return BaseMagnitude * MagnitudeScalingCurve->GetFloatValue(Level);
		}
		// Otherwise use the linear scaling factor
		return BaseMagnitude + (MagnitudeScalePerLevel * (Level - 1));
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	FGameplayTag EffectTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	bool bOneShotEffect = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem")
	bool bPermanent = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bPermanent", EditConditionHides, ClampMin = 0.02f))
	float Duration = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bOneShotEffect && !bPermanent", EditConditionHides, ClampMin = 0.01f))
	float TriggerInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem|Magnitude")
	float BaseMagnitude = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem|Magnitude", meta = (EditCondition = "MagnitudeScalingCurve == nullptr"))
	float MagnitudeScalePerLevel = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem|Level")
	int32 Level = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem|Level")
	int32 MaxLevel = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem|Target")
	FGameplayTag TargetAttributeTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoctAbilitySystem|Magnitude")
	ENoctAttributeOperation AttributeOperation;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	FGameplayTagContainer AppliedTags;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	FGameplayTagContainer BlockedTags;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	FGameplayTagContainer AbilitiesToCancel;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	bool bIsActiveAndApplied = false;
	
	// Loops the OnEffectTriggered event every x seconds.
	FTimerHandle EffectTimerHandle;

	// Duration of the effect.
	FTimerHandle EffectDurationTimerHandle;

	UPROPERTY(visibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	TObjectPtr<UNoctAbilityComponent> OwningAbilityComponent;


	virtual UWorld* GetWorld() const override;
};
