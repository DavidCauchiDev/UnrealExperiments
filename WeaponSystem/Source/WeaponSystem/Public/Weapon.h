// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponData.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class WEAPONSYSTEM_API AWeapon : public AActor
{
	GENERATED_BODY()

	UDELEGATE(BlueprintAuthorityOnly)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGenericWeaponEvent, AWeapon*, OwningWeapon);

	UDELEGATE(BlueprintAuthorityOnly)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponChargeResumeEvent, AWeapon*, OwningWeapon, float,
	                                             ChargeProgress);

	UDELEGATE(BlueprintAuthorityOnly)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponReloadEvent, AWeapon*, OwningWeapon, bool, bReloadFromEmpty);

public:
	AWeapon();

	UFUNCTION(BlueprintCallable)
	void StartFire();

	UFUNCTION(BlueprintCallable)
	void StopFire();

	UFUNCTION(BlueprintCallable)
	void Reload();
	void FinishReload();

	bool NativeCanUseWeapon() const;

	UFUNCTION(BlueprintCallable)
	bool AddAttachment(TSubclassOf<class AWeaponAttachment> Attachment);

	UFUNCTION(BlueprintCallable)
	AWeaponAttachment* GetAttachment(EWeaponAttachmentType Attachment);

	UFUNCTION(BlueprintCallable)
	bool RemoveAttachment(EWeaponAttachmentType AttachmentPointToRemoveFrom);

	void NativeOnEquipped(const TObjectPtr<AActor>& EquippedBy);
	void NativeUnequip(bool bDelete = false);

	UFUNCTION(BlueprintPure)
	int GetAmmoInMagazine() const;

	UFUNCTION(BlueprintPure)
	int GetMagazineAmmoCapacity();


	UFUNCTION(BlueprintCallable)
	bool InstallMagazine(TSubclassOf<class AWeaponMagazine> Attachment);

	UFUNCTION(BlueprintCallable)
	bool RemoveMagazine(bool bDropToGround);

	UFUNCTION(BlueprintPure)
	FTransform GetMuzzleTransform() const;

	UFUNCTION(BlueprintPure)
	const FWeaponStats& GetWeaponFireSettings() { return WeaponFireSettings; }

	UFUNCTION(BlueprintPure)
	EFireMode GetActiveFireMode() const { return ActiveFireMode; }

	UFUNCTION(BlueprintCallable)
	void SetAmmoInMagazine(int GetMagazineAmmoCapacity);

	void SetFirstPerson();
	void SetThirdPerson();

	UFUNCTION(BlueprintCallable)
	void SetPreferredHitPoint(const FVector_NetQuantize& ImpactPoint, const FVector_NetQuantize& NewEyeViewpoint);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UWeaponData> WeaponData;

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UFUNCTION(BlueprintCallable)
	AActor* GetEquippedActor();

protected:
	virtual void BeginPlay() override;

	void WeaponFiredLogic();
	void NativeChargeFullyFinished();
	void NativeShotCooldownFinished();

	// Properties
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	FWeaponStats WeaponFireSettings;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TMap<EWeaponAttachmentType, AWeaponAttachment*> CurrentAttachments;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<AWeaponMagazine> InstalledMagazine;

	// State
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<AActor> EquippedByActor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Ammo")
	EFireMode ActiveFireMode = EFireMode::Semi;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Ammo")
	int BurstShotsLeft = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	EWeaponState WeaponState;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bCharging = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bCharged = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bNeedsTriggerRelease = false;


	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bHasTriggerInput = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	float LastChargeTime = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	float LastChargeRemaining = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FVector PreferredHitPosition;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FVector EyeViewpoint;


	// Events
public:
	UPROPERTY(BlueprintAssignable)
	FWeaponChargeResumeEvent OnChargeResumed;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnChargeStart;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnChargeFinished;

	UPROPERTY(BlueprintAssignable)
	FWeaponChargeResumeEvent OnChargeCancelled;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnEquipped;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnUnequipped;

	UPROPERTY(BlueprintAssignable)
	FWeaponReloadEvent OnReloadStarted;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnReloadFinished;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnFired;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnBurstFinished;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnTriggerReleased;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnFirstFired;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnShootCooldownFinished;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnAmmoDepleted;

	UPROPERTY(BlueprintAssignable)
	FGenericWeaponEvent OnDryFire;

protected:
	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Charge Start")
	void OnChargeStartEvent();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Charge Resumed")
	void OnChargeResumedEvent(float Progress);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Charge Cancelled")
	void OnChargeCanceledEvent(float Progress);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Charge Finished")
	void OnChargeFinishedEvent();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Equipped")
	void OnEquippedEvent();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Unequipped")
	void OnUnequippedEvent();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Reload Started")
	void OnReloadStartedEvent(bool bReloadFromEmpty);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Reload Finished")
	void OnReloadFinishedEvent();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Fired")
	void OnFiredEvent();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Handle Attack")
	void HandleAttackEvent(const FVector Start, const FVector TargetDirection, AWeapon* Weapon,
	                       AActor* WeaponOwner, AController* InstigatorAttacker);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On First Fire")
	void OnFirstFireEvent();


	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Burst Finished")
	void OnBurstFinishedEvent();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Trigger Released")
	void OnTriggerReleasedEvent();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Ammo Depleted")
	void OnAmmoDepletedEvent();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Shoot Cooldown Finished")
	void OnShootCooldownFinishedEvent();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Dry Fire")
	void OnDryFireEvent();

private:
	FTimerHandle ShootCooldown;
	FTimerHandle ChargeTimer;
	int TotalShotsFiredSinceStart = 0;
};
