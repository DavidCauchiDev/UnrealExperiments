// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponFunctionLibrary.h"

#include "Weapon.h"
#include "WeaponData.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

bool UWeaponFunctionLibrary::HandleWeaponLineTraceAttack(const UObject* WorldContextObject, FHitResult& HitResult,
                                                         const FVector& Start, const FVector& Direction,
                                                         AWeapon* Weapon, AActor* Owner,
                                                         AController* Instigator)
{
	const FWeaponStats& WeaponStats = Weapon->GetWeaponFireSettings();


	// Handle impact damage and effects
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bReturnPhysicalMaterial = true;

	if (Owner)
	{
		CollisionQueryParams.AddIgnoredActor(Owner);
		CollisionQueryParams.AddIgnoredActor(Owner->GetOwner());
		CollisionQueryParams.AddIgnoredActor(Weapon);
		CollisionQueryParams.AddIgnoredActor(Weapon->GetEquippedActor());
	}

	ECollisionChannel WeaponTraceChannel = WeaponStats.;
	if (!WorldContextObject->GetWorld()->LineTraceSingleByChannel(HitResult, Start,
	                                                              Start + Direction * WeaponStats.MaxProjectileRange,
	                                                              WeaponTraceChannel,
	                                                              CollisionQueryParams) || !HitResult.GetActor())
	{
		return false;
	}

	FNoctDamageEvent DamageEvent;
	DamageEvent.DamageTypeClass = WeaponStats.DamageType;
	DamageEvent.HitBone = HitResult.BoneName;
	DamageEvent.Impulse = HitResult.ImpactNormal;
	DamageEvent.HeadShotMultiplier = WeaponStats.HeadshotMultiplier;

	if (WeaponStats.DamageType.GetDefaultObject()->bBleedThroughArmor)
	{
		DamageEvent.bBleedThrough = true;
	}

	ensureAlways(DamageEvent.DamageTypeClass);

	float DamageMultiplied = WeaponStats.Damage;

	if (WeaponStats.AmmoType == EAmmoDamageType::LineTrace)
	{
		HitResult.GetActor()->TakeDamage(DamageMultiplied, DamageEvent, Instigator, Owner);
	}
	else
	{
		UGameplayStatics::ApplyRadialDamageWithFalloff(WorldContextObject, DamageMultiplied,
		                                               DamageMultiplied * WeaponStats.MinimumRadialDamageMultiplier,
		                                               HitResult.ImpactPoint, WeaponStats.RadialDamageInnerRadius,
		                                               WeaponStats.RadialDamageOuterRadius,
		                                               WeaponStats.RadialDamageFalloff,
		                                               WeaponStats.DamageType, {}, Owner, Instigator);
	}

	UNiagaraSystem* SelectedImpact = nullptr;
	UMaterialInterface* SelectedDecal = nullptr;
	FVector DecalSize = FVector(32);

	const TEnumAsByte<EPhysicalSurface> Surface = HitResult.PhysMaterial.IsValid()
		                                              ? HitResult.PhysMaterial.Get()->SurfaceType
		                                              : TEnumAsByte(SurfaceType_Default);

	// Select decal
	if (FImpactDatabase Database; Weapon->WeaponData->GetImpact(Surface, Database))
	{
		SelectedImpact = Database.ImpactEffect;

		if (Database.ImpactDecals.Num() > 0)
		{
			SelectedDecal = Database.ImpactDecals[FMath::RandRange(0, Database.ImpactDecals.Num() - 1)];
		}

		DecalSize = Database.DecalBaseSize;
	}

	// Spawn impact particles
	if (SelectedImpact)
	{
		FRotator Rotation = FRotationMatrix::MakeFromZ(HitResult.ImpactNormal).Rotator();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, SelectedImpact, HitResult.ImpactPoint + HitResult.ImpactNormal * 1.1f,
		                                               Rotation, FVector(.25f), true, true, ENCPoolMethod::AutoRelease);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red,
		                                 FString::Printf(
			                                 TEXT("INVALID OR NO IMPACT FOR SURFACE TYPE (%s) SET ON WEAPON DATA"),
			                                 *UEnum::GetValueAsString(Surface)));
		DrawDebugBox(WorldContextObject->GetWorld(), HitResult.ImpactPoint, FVector(2, 2, 2), FColor::Orange, false,
		             5.f, 0);
	}

	// Spawn decal
	if (SelectedDecal)
	{
		FRotator RotationX = FRotationMatrix::MakeFromY(HitResult.ImpactNormal).Rotator();
		if (const auto Decal = UGameplayStatics::SpawnDecalAttached(SelectedDecal, DecalSize, HitResult.GetComponent(),
		                                                            HitResult.BoneName, HitResult.ImpactPoint,
		                                                            RotationX, EAttachLocation::KeepWorldPosition))
		{
			Decal->SetFadeScreenSize(0.001f);
			Decal->SetFadeOut(7, 10, false);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red,
		                                 FString::Printf(
			                                 TEXT("INVALID OR NO DECAL FOR SURFACE TYPE (%s) SET ON WEAPON DATA"),
			                                 *UEnum::GetValueAsString(Surface)));
	}

	return true;
}


float UWeaponFunctionLibrary::GetFireRateInSeconds(AWeapon* Weapon)
{
	if (Weapon)
	{
		return 0;
	}

	const int Rate = Weapon->GetActiveFireMode() == EFireMode::Burst
		                 ? Weapon->GetWeaponFireSettings().BurstFireRate
		                 : Weapon->GetWeaponFireSettings().FireRate;
	return 60.0f / static_cast<float>(Rate);
}

int UWeaponFunctionLibrary::GetFireRate(class AWeapon* Weapon)
{
	return Weapon->GetActiveFireMode() == EFireMode::Burst
		       ? Weapon->GetWeaponFireSettings().BurstFireRate
		       : Weapon->GetWeaponFireSettings().FireRate;
}

int UWeaponFunctionLibrary::GetBurstCount(AWeapon* Weapon)
{
	if (!Weapon)
	{
		return 0;
	}
	const int BurstCount = Weapon->GetActiveFireMode() == EFireMode::Burst
		                       ? Weapon->GetWeaponFireSettings().BurstFireRate
		                       : 1;
	return BurstCount;
}