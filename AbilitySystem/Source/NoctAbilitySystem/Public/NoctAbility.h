 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "UObject/Object.h"
#include "Curves/CurveFloat.h"
#include "NoctAbility.generated.h"

class UInputAction;
class UNoctAbilityComponent;
 /**
  *
  */
 UCLASS(Blueprintable, BlueprintType)
class NOCTABILITYSYSTEM_API UNoctAbility : public UObject
{
	GENERATED_BODY()
public:

	UNoctAbility();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem|Input")
	TObjectPtr<UInputAction> ActivationAction;

 	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem|Input")
 	FInputActionValue CurrentInputValue;

 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem|Input", meta = (EditCondition = "ActivationAction != nullptr") )
 	bool bAllowInputTriggered = false;

 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem|Input", meta = (EditCondition = "ActivationAction != nullptr") )
 	bool bAutoActivateAbilityOnInputStarted = false;

 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem|Input", meta = (EditCondition = "ActivationAction != nullptr") )
 	bool bAutoFinishAbilityOnInputCompleted = false;

 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem|Input", meta = (EditCondition = "ActivationAction != nullptr") )
 	bool bAutoCancelAbilityOnInputCancelled = false;
 	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	FGameplayTag AbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	FGameplayTagContainer BlockingTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	FGameplayTagContainer CancelTags;

 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
 	bool bAutoActivateOnAdd = false;
 	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	bool bCancelOnBlock = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	bool bCooldownOnCancel = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	float BaseCooldown = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NoctAbilitySystem")
	bool bIsActive = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	int Level = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem|Cost")
	FGameplayTag CostAttributeTag;  // Points to EVE attribute

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem|Cost")
	float CostValue = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem|Cost")
	UCurveFloat* CostScalingCurve = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NoctAbilitySystem|Cost", meta = (EditCondition = "CostScalingCurve == nullptr"))
	float CostScalingPerLevel = 0.0f;
	
	UFUNCTION(BlueprintPure, Category = "NoctAbilitySystem|Cost")
	float GetScaledCostValue() const
	{
		// If we have a curve, use it for scaling based on level
		if (CostScalingCurve)
		{
			return CostValue * CostScalingCurve->GetFloatValue(Level);
		}
		// Otherwise use the linear scaling factor
		return CostValue + (CostScalingPerLevel * (Level - 1));
	}

	FTimerHandle CooldownTimer;

	// Parent functions to handle ability flow
	// These allow us to not require calling super functions from abilities
	void AbilityAddedToOwner();
 	void AbilityRemovedFromOwner();

	UFUNCTION(BlueprintCallable, DisplayName="Activate Ability")
	void ActivateAbility();

	UFUNCTION(BlueprintCallable, DisplayName="Finish Ability")
	void FinishAbility();

	UFUNCTION(BlueprintCallable, DisplayName="Cancel Ability")
	void CancelAbility();

	UFUNCTION()
	void FinishCooldown();

	void TriggerCooldown(float CooldownOverride = 0);

	// Native Events
	virtual void AbilityAdded()     {};
 	virtual void AbilityRemoved()   {};
	virtual void AbilityActivated() {};
	virtual void AbilityFinished()  {};
	virtual void AbilityCancelled() {};

	UFUNCTION()
	virtual void NativeOnAbilityCooldownFinished() {};

	// Blueprint Events
	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityAdded();

 	UFUNCTION(BlueprintImplementableEvent)
 	void OnAbilityRemoved();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityActivated();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityFinished();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityCancelled();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityCooldownFinished();

 	// INPUT STUFF
 	UFUNCTION()
 	virtual void InputActionStarted(const struct FInputActionValue& Value);
    
 	UFUNCTION()
 	virtual void InputActionTriggered(const struct FInputActionValue& Value);
    
 	UFUNCTION()
 	virtual void InputActionCompleted(const struct FInputActionValue& Value);

 	UFUNCTION()
 	virtual void InputActionCancelled(const struct FInputActionValue& Value);
 	
 	UFUNCTION(BlueprintImplementableEvent)
 	void OnAbilityInputStarted(const FInputActionValue& Value);
    
 	UFUNCTION(BlueprintImplementableEvent)
 	void OnAbilityInputTriggered(const FInputActionValue& Value);
    
 	UFUNCTION(BlueprintImplementableEvent)
 	void OnAbilityInputCompleted(const FInputActionValue& Value);

 	UFUNCTION(BlueprintImplementableEvent)
 	void OnAbilityInputCancelled(const FInputActionValue& Value);

	// Helpers
	UFUNCTION(BlueprintPure)
	bool CooldownActive() const;

	virtual bool CanActivateAbility();

	virtual UWorld* GetWorld() const override;

	// Owning Component used to control abilities
	UPROPERTY(visibleInstanceOnly, BlueprintReadOnly, Category = "NoctAbilitySystem")
	TObjectPtr<UNoctAbilityComponent> OwningAbilityComponent;

 	UFUNCTION(BlueprintPure, Category = "NoctAbilitySystem")
 	AActor* GetOwningActor() const;

 	UFUNCTION(BlueprintPure, Category = "NoctAbilitySystem")
 	ACharacter* GetOwningCharacter() const;
};
