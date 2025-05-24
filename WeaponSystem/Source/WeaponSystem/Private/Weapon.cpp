// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "WeaponAttachment.h"
#include "WeaponFunctionLibrary.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->bCastHiddenShadow = true;
}

AActor* AWeapon::GetEquippedActor()
{
	return EquippedByActor;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	ensureAlways(WeaponData);
	WeaponFireSettings = WeaponData->Stats;
	ActiveFireMode = WeaponFireSettings.FireMode;
	InstallMagazine(WeaponData->Magazine);
}

bool AWeapon::NativeCanUseWeapon() const
{
	if (!InstalledMagazine)
	{
		return false;
	}

	return InstalledMagazine->AmmoLeft > 0 && !bNeedsTriggerRelease && WeaponState == EWeaponState::Ready && !
		GetWorldTimerManager().IsTimerActive(ShootCooldown) && WeaponState != EWeaponState::BurstCooldown;
}

bool AWeapon::AddAttachment(const TSubclassOf<AWeaponAttachment> Attachment)
{
	if (!Attachment)
	{
		return false;
	}

	const EWeaponAttachmentType Type = Attachment.GetDefaultObject()->AttachmentType;

	if (CurrentAttachments.Contains(Type) || Type == EWeaponAttachmentType::Magazine && !Cast<AWeaponMagazine>(
		Attachment.GetDefaultObject()))
	{
		return false;
	}

	// Spawn attachment
	FActorSpawnParameters Parameters;
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AWeaponAttachment* WorldAttachment = GetWorld()->SpawnActor<AWeaponAttachment>(
		Attachment, GetActorTransform(), Parameters);

	ensure(WorldAttachment);

	if (auto Magazine = Cast<AWeaponMagazine>(WorldAttachment))
	{
		InstalledMagazine = Magazine;
	}

	// Attach and setup values
	CurrentAttachments.Add(Type, WorldAttachment);
	WorldAttachment->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale,
	                                   WeaponData->AttachmentSockets.Contains(Type)
		                                   ? WeaponData->AttachmentSockets[Type]
		                                   : NAME_None);
	FWeaponStats::ApplyAttributeChanges(WeaponFireSettings, WorldAttachment->WeaponStatTweaks);
	return true;
}

AWeaponAttachment* AWeapon::GetAttachment(EWeaponAttachmentType Attachment)
{
	if (CurrentAttachments.Contains(Attachment))
	{
		return CurrentAttachments[Attachment];
	}

	return nullptr;
}

bool AWeapon::RemoveAttachment(const EWeaponAttachmentType AttachmentPointToRemoveFrom)
{
	if (!CurrentAttachments.Contains(AttachmentPointToRemoveFrom))
	{
		return false;
	}

	AWeaponAttachment* Attachment = CurrentAttachments[AttachmentPointToRemoveFrom];
	FWeaponStats::UnapplyAttributeChanges(Attachment->WeaponStatTweaks, WeaponFireSettings);
	Attachment->Destroy();
	return true;
}

int AWeapon::GetAmmoInMagazine() const
{
	return InstalledMagazine ? InstalledMagazine->AmmoLeft : 0;
}

int AWeapon::GetMagazineAmmoCapacity()
{
	return InstalledMagazine ? InstalledMagazine->AmmoCapacity : 0;
}

bool AWeapon::InstallMagazine(const TSubclassOf<class AWeaponMagazine> Attachment)
{
	if (!InstalledMagazine && Attachment)
	{
		// Spawn attachment
		FActorSpawnParameters Parameters;
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		InstalledMagazine = GetWorld()->SpawnActor<AWeaponMagazine>(Attachment, Parameters);

		if (ensure(InstalledMagazine))
		{
			InstalledMagazine->AmmoLeft = InstalledMagazine->AmmoCapacity;
			InstalledMagazine->AmmoTag = WeaponFireSettings.AmmoTag;

			constexpr EWeaponAttachmentType Type = EWeaponAttachmentType::Magazine;
			const FName SocketToAttachTo = WeaponData->AttachmentSockets.Contains(Type)
				                               ? WeaponData->AttachmentSockets[Type]
				                               : NAME_None;

			InstalledMagazine->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale,
			                                     SocketToAttachTo);
		}

		return true;
	}

	return false;
}

bool AWeapon::RemoveMagazine(bool bDropToGround)
{
	if (InstalledMagazine)
	{
		if (bDropToGround)
		{
			InstalledMagazine->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		else
		{
			// Return to inventory
		}

		InstalledMagazine = nullptr;
		return true;
	}

	return false;
}

FTransform AWeapon::GetMuzzleTransform() const
{
	FTransform MuzzleTransform = GetActorTransform();
	FName MuzzleSocket = WeaponFireSettings.MuzzleSocket;

	// We could probably cache this tbh.
	// Check to see if we have a muzzle attachment and use its socket for spawning in the trace
	if (CurrentAttachments.Contains(EWeaponAttachmentType::Muzzle))
	{
		if (AWeaponMuzzle* Muzzle = Cast<AWeaponMuzzle>(CurrentAttachments[EWeaponAttachmentType::Muzzle]))
		{
			return Muzzle->GetMuzzleTransform();
		}
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, TEXT("MUZZLE ATTACHMENT IS NOT CORRECT TYPE"));
		}
	}
	else if (WeaponMesh->DoesSocketExist(MuzzleSocket))
	// No muzzle attachment exists so lets use the base muzzle location.
	{
		MuzzleTransform = WeaponMesh->GetSocketTransform(MuzzleSocket);
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red,
		                                 TEXT(
			                                 "WEAPON MESH DOES NOT HAVE MUZZLE SOCKET!!! SPAWNING FROM BASE MUZZLE TRANSFORM."));
	}
	return MuzzleTransform;
}

void AWeapon::SetAmmoInMagazine(const int GetMagazineAmmoCapacity)
{
	if (InstalledMagazine)
	{
		InstalledMagazine->AmmoLeft = GetMagazineAmmoCapacity;
	}
}

void AWeapon::NativeOnEquipped(const TObjectPtr<AActor>& EquippedBy)
{
	OnEquippedEvent();

	if (OnEquipped.IsBound())
	{
		OnEquipped.Broadcast(this);
	}

	if (EquippedBy)
	{
		EquippedByActor = EquippedBy;
	}

	for (const auto& Attachment : CurrentAttachments)
	{
		Attachment.Value->NativeOnEquipped();
	}
}

void AWeapon::NativeUnequip(const bool bDelete)
{
	OnUnequippedEvent();

	if (OnUnequipped.IsBound())
	{
		OnUnequipped.Broadcast(this);
	}

	for (const auto& Attachment : CurrentAttachments)
	{
		Attachment.Value->NativeOnUnequipped();
	}

	EquippedByActor = nullptr;

	if (bDelete)
	{
		Destroy();
	}
}

void AWeapon::Reload()
{
	if (GetAmmoInMagazine() >= InstalledMagazine->AmmoCapacity || WeaponState == EWeaponState::ReloadCooldown)
	{
		return;
	}

	StopFire();

	OnReloadStartedEvent(GetAmmoInMagazine() < 1);

	if (OnReloadStarted.IsBound())
	{
		OnReloadStarted.Broadcast(this, GetAmmoInMagazine() < 1);
	}

	GetWorldTimerManager().ClearTimer(ChargeTimer);
	GetWorldTimerManager().ClearTimer(ShootCooldown);

	WeaponState = EWeaponState::ReloadCooldown;
}

void AWeapon::FinishReload()
{
	WeaponState = EWeaponState::Ready;

	OnReloadFinishedEvent();

	if (OnReloadFinished.IsBound())
	{
		OnReloadFinished.Broadcast(this);
	}
}

void AWeapon::NativeShotCooldownFinished()
{
	WeaponState = EWeaponState::Ready;

	OnShootCooldownFinishedEvent();

	if (OnShootCooldownFinished.IsBound())
	{
		OnShootCooldownFinished.Broadcast(this);
	}
}

void AWeapon::WeaponFiredLogic()
{
	const FTransform WeaponMuzzleTransform = GetMuzzleTransform();
	const FVector StartLocation = WeaponMuzzleTransform.GetLocation();

	// DrawDebugLine(GetWorld(), EyeViewpoint, PreferredHitPosition, FColor::Green, false, 4);
	// DrawDebugLine(GetWorld(), StartLocation, PreferredHitPosition, FColor::Red, false, 4);
	//
	const float SpreadCurveValue = WeaponFireSettings.SpreadCurve.GetRichCurve()->Eval(TotalShotsFiredSinceStart, 1);

	for (int i = 0; i < WeaponFireSettings.ShotCount; ++i)
	{
		const float XSpread = FMath::DegreesToRadians(FMath::RandRange(WeaponFireSettings.SpreadXMinInDegrees,
		                                                               FMath::Lerp(
			                                                               WeaponFireSettings.SpreadXMinInDegrees,
			                                                               WeaponFireSettings.SpreadXMaxInDegrees,
			                                                               SpreadCurveValue)));
		const float YSpread = FMath::DegreesToRadians(FMath::RandRange(WeaponFireSettings.SpreadYMinInDegrees,
		                                                               FMath::Lerp(
			                                                               WeaponFireSettings.SpreadYMinInDegrees,
			                                                               WeaponFireSettings.SpreadYMaxInDegrees,
			                                                               SpreadCurveValue)));

		const FVector Direction = FMath::VRandCone(PreferredHitPosition - StartLocation, XSpread, YSpread);

		HandleAttackEvent(StartLocation, Direction, this, EquippedByActor, GetInstigatorController());
		PreferredHitPosition = FVector::Zero();
	}

	TotalShotsFiredSinceStart++;

	if (InstalledMagazine && InstalledMagazine->AmmoLeft > 0)
	{
		InstalledMagazine->AmmoLeft -= 1;
	}

	WeaponState = EWeaponState::ShootCooldown;

	if (ActiveFireMode == EFireMode::Burst && BurstShotsLeft > 0)
	{
		WeaponState = EWeaponState::BurstCooldown;
		BurstShotsLeft--;
		GetWorldTimerManager().SetTimer(ShootCooldown, this, &AWeapon::WeaponFiredLogic,
		                                60.0f / WeaponFireSettings.BurstFireRate, false);
	}
	else
	{
		if (ActiveFireMode == EFireMode::Burst && BurstShotsLeft < 1)
		{
			OnBurstFinishedEvent();

			if (OnBurstFinished.IsBound())
			{
				OnBurstFinished.Broadcast(this);
			}
		}
		GetWorldTimerManager().SetTimer(ShootCooldown, this, &AWeapon::NativeShotCooldownFinished,
		                                60.0f / WeaponFireSettings.FireRate, false);
	}

	OnFiredEvent();

	if (OnFired.IsBound())
	{
		OnFired.Broadcast(this);
	}

	if (GetAmmoInMagazine() <= 0)
	{
		StopFire();

		OnAmmoDepletedEvent();

		if (OnAmmoDepleted.IsBound())
		{
			OnAmmoDepleted.Broadcast(this);
		}
	}
}

void AWeapon::NativeChargeFullyFinished()
{
	bCharged = true;
	bCharging = false;

	OnChargeFinishedEvent();

	if (OnChargeFinished.IsBound())
	{
		OnChargeFinished.Broadcast(this);
	}
}

void AWeapon::StartFire()
{
	if (!NativeCanUseWeapon())
	{
		if (GetAmmoInMagazine() < 1 && !bNeedsTriggerRelease && WeaponState == EWeaponState::Ready)
		{
			bNeedsTriggerRelease = true;
			OnDryFireEvent();
			if (OnDryFire.IsBound())
			{
				OnDryFire.Broadcast(this);
			}
		}
		return;
	}

	if (!bHasTriggerInput)
	{
		OnFirstFireEvent();
		if (OnFirstFired.IsBound())
		{
			OnFirstFired.Broadcast(this);
		}
		bHasTriggerInput = true;
	}

	if (ActiveFireMode == EFireMode::Burst)
	{
		BurstShotsLeft = WeaponFireSettings.BurstCount;

		// Consume a burst shot up front 
		BurstShotsLeft--;
	}

	if (WeaponFireSettings.bChargeBeforeUse && !bCharged && !GetWorldTimerManager().IsTimerActive(ChargeTimer))
	{
		const float TimeSinceLastCharge = GetWorld()->TimeSince(LastChargeTime);
		float ChargeTime = WeaponFireSettings.ChargeTime;

		if (TimeSinceLastCharge < WeaponFireSettings.ChargeTime && TimeSinceLastCharge > 0)
		{
			ChargeTime = WeaponFireSettings.ChargeTime - TimeSinceLastCharge;

			OnChargeResumedEvent(WeaponFireSettings.ChargeTime - ChargeTime);

			if (OnChargeResumed.IsBound())
			{
				OnChargeResumed.Broadcast(this, WeaponFireSettings.ChargeTime - ChargeTime);
			}
		}
		else
		{
			OnChargeStartEvent();

			if (OnChargeStart.IsBound())
			{
				OnChargeStart.Broadcast(this);
			}
		}

		GetWorldTimerManager().SetTimer(ChargeTimer, this, &AWeapon::NativeChargeFullyFinished, ChargeTime, false);
		bCharging = true;
	}
	else
	{
		// If we are in semi or burst fire mode then disallow shooting again until trigger release.
		bNeedsTriggerRelease = ActiveFireMode != EFireMode::Auto;
		WeaponFiredLogic();
	}
}

void AWeapon::StopFire()
{
	bNeedsTriggerRelease = false;
	bHasTriggerInput = false;
	TotalShotsFiredSinceStart = 0;

	OnTriggerReleasedEvent();

	if (OnTriggerReleased.IsBound())
	{
		OnTriggerReleased.Broadcast(this);
	}

	if (bCharging || bCharged)
	{
		LastChargeTime = WeaponFireSettings.bChargeCancelFullyResetsTime ? 0 : GetWorld()->GetTimeSeconds();

		if (bCharging)
		{
			GetWorldTimerManager().ClearTimer(ChargeTimer);

			const float CurrentChargeTime = WeaponFireSettings.ChargeTime - LastChargeTime;

			OnChargeCanceledEvent(CurrentChargeTime);

			if (OnChargeCancelled.IsBound())
			{
				OnChargeCancelled.Broadcast(this, CurrentChargeTime);
			}
		}

		bCharged = false;
		bCharging = false;
	}
}

void AWeapon::SetFirstPerson()
{
	WeaponMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::FirstPerson);
	WeaponMesh->CastShadow = false;
	WeaponMesh->SetHiddenInGame(false);

	for (const auto Attachment : CurrentAttachments)
	{
		Attachment.Value->AttachmentMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::FirstPerson);
		Attachment.Value->AttachmentMesh->CastShadow = false;
		Attachment.Value->AttachmentMesh->SetHiddenInGame(false);
	}


	if (InstalledMagazine)
	{
		InstalledMagazine->AttachmentMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::FirstPerson);
		InstalledMagazine->AttachmentMesh->CastShadow = false;
		InstalledMagazine->AttachmentMesh->SetHiddenInGame(false);
	}
}

void AWeapon::SetThirdPerson()
{
	WeaponMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::None);
	WeaponMesh->CastShadow = true;
	WeaponMesh->SetHiddenInGame(true);

	for (const auto Attachment : CurrentAttachments)
	{
		Attachment.Value->AttachmentMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::None);
		Attachment.Value->AttachmentMesh->CastShadow = true;
		InstalledMagazine->AttachmentMesh->SetHiddenInGame(true);
	}

	if (InstalledMagazine)
	{
		InstalledMagazine->AttachmentMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::None);
		InstalledMagazine->AttachmentMesh->CastShadow = true;
		InstalledMagazine->AttachmentMesh->SetHiddenInGame(true);
	}
}

void AWeapon::SetPreferredHitPoint(const FVector_NetQuantize& ImpactPoint, const FVector_NetQuantize& NewEyeViewpoint)
{
	PreferredHitPosition = ImpactPoint;
	EyeViewpoint = NewEyeViewpoint;
}
