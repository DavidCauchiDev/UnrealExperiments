// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DamageEvents.h"
#include "GameplayTagContainer.h"
#include "WeaponData.generated.h"

class UInitialActiveSoundParams;
class UNiagaraSystem;
class AWeaponMagazine;

UENUM(Blueprintable)
enum class EWeaponSlotID : uint8
{
	None,
	HolsterBack,
	HolsterSide,
	EquippedRightHand,
	EquippedLeftHand,
};

UENUM(Blueprintable)
enum class EWeaponSlotType : uint8
{
	TwoHanded,
	CanDualWield,
	TemporaryCarry,
};


UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EFireMode : uint8
{
	None = 0 UMETA(Hidden),
	Semi = 1 << 1,
	Burst = 1 << 2,
	Auto = 1 << 3,
};

ENUM_CLASS_FLAGS(EFireMode);

UENUM()
enum class EWeaponType : uint8
{
	Projectile,
	Melee,
};

UENUM()
enum class EWeaponAttachmentType : uint8
{
	None,
	Sight,
	Barrel,
	Muzzle,
	Magazine,
	Stock,
	TopRail,
	LeftRail,
	RightRail,
	BottomRail,
};

USTRUCT(BlueprintType)
struct FAttachmentWeaponStatAdjustments
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMax = 180))
	int SpreadXMinInDegrees = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMax = 180))
	int SpreadXMaxInDegrees = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMax = 180))
	int SpreadYMinInDegrees = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMax = 180))
	int SpreadYMaxInDegrees = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Fire Rate (RPM)", meta=(ClampMax = 7500))
	int FireRate = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = -3, ClampMax = 3))
	float BurstFireCooldown = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 2, ClampMax = 12))
	int BurstCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = -2, ClampMax = 2))
	int ReloadSpeed = 0;
};

UENUM(BlueprintType)
enum class EAmmoDamageType : uint8
{
	LineTrace,
	Radial
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Ready,
	ShootCooldown,
	ReloadCooldown,
	BurstCooldown,
};

USTRUCT(BlueprintType)
struct FImpactDatabase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EPhysicalSurface> SurfaceType = SurfaceType_Default;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> ImpactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundBase> ImpactSoundEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundBase> BounceSoundEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundAttenuation> AttenuationSettings = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundConcurrency> ConcurrencySettings = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInitialActiveSoundParams> InitialParams = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundAttenuation> BounceAttenuationSettings = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundConcurrency> BounceConcurrencySettings = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInitialActiveSoundParams> BounceInitialParams = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UMaterialInterface>> ImpactDecals = {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector DecalBaseSize = FVector(1);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DecalLifespan = 12;
};

UCLASS()
class USolDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bBleedThroughArmor = false;
};

USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MuzzleSocket = FName(TEXT("Muzzle"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AmmoTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRuntimeFloatCurve SpreadCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFireMode FireMode = EFireMode::Semi;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType = EWeaponType::Projectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 0))
	int SpreadXMinInDegrees = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 0))
	int SpreadXMaxInDegrees = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 0))
	int SpreadYMinInDegrees = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 0))
	int SpreadYMaxInDegrees = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 1))
	float MaxProjectileRange = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Base Damage", meta=(ClampMin = 1))
	float Damage = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin=0.1, ClampMax=1, EditCondition="AmmoType == EAmmoDamageType::Radial", EditConditionHides))
	float MinimumRadialDamageMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 1))
	float HeadshotMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin=5, ClampMax = 500, ForceUnits="cm", EditCondition="AmmoType == EAmmoDamageType::Radial",
			EditConditionHides))
	float RadialDamageInnerRadius = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin=10, ClampMax = 1000, ForceUnits="cm", EditCondition="AmmoType == EAmmoDamageType::Radial",
			EditConditionHides))
	float RadialDamageOuterRadius = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(EditCondition="AmmoType == EAmmoDamageType::Explosive", EditConditionHides))
	float RadialDamageFalloff = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USolDamageType> DamageType = USolDamageType::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoDamageType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 1f, ClampMax = 4))
	int ZoomLevelOne = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 0, ClampMax = 8))
	int ZoomLevelTwo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 0.1f, ClampMax = 25))
	int ReloadSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 1, ClampMax = 32))
	int ShotCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 0.1f, ClampMax = 12))
	bool bChargeBeforeUse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin = 0.1f, ClampMax = 12, EditCondition="bChargeBeforeUse", EditConditionHides))
	float ChargeTime = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bChargeBeforeUse", EditConditionHides))
	bool bChargeCancelFullyResetsTime = true;

	// How many shots to fire per minute. If burst fire is enabled, then this will be used to determine gaps between shots and BurstFireCooldown will be used to delay firing again
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Fire Rate",
		meta=(ClampMin = 1, ClampMax = 7500, ForceMode="rpm"))
	int FireRate = 600;

	// Time between bursts. Clamped max at 3 seconds to prevent things from getting out of hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin = 60, EditCondition="Firemode == EFireMode::Burst", EditConditionHides))
	float BurstFireRate = 900;

	// How many shots to fire during a burst.
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(ClampMin = 2, ClampMax = 12, EditCondition="Firemode == EFireMode::Burst", EditConditionHides))
	int BurstCount = 3;

	static void ApplyAttributeChanges(FWeaponStats& To, const FAttachmentWeaponStatAdjustments& From);
	static void UnapplyAttributeChanges(const FAttachmentWeaponStatAdjustments& From, FWeaponStats& To);
};


UCLASS(BlueprintType)
class UWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon",
		meta = (ToolTip = "Display Icon of the Weapon in the HUD"))
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon",
		meta = (ToolTip = "Crosshair of the Weapon in the HUD"))
	TSoftObjectPtr<UTexture2D> Crosshair;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon",
		meta = (ToolTip = "Display Icon of the Weapon in the HUD"))
	TSubclassOf<AActor> WorldPickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (ToolTip = "Magazine to attach"))
	TSubclassOf<AWeaponMagazine> Magazine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon",
		meta = (ToolTip = "Display Name of the Weapon in the HUD"))
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon",
	meta = (ToolTip = "Name of the socket to attach this weapon to on the arms"))
	FName AttachToSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon",
		meta = (ToolTip = "Display Name of the Weapon in the HUD"))
	TMap<EWeaponAttachmentType, FName> AttachmentSockets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon",
		meta = (ToolTip = "Weapon slot type either two handed, can duel wield or can only be held but not stored"))
	EWeaponSlotType WeaponSlotType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon",
		meta = (ToolTip =
			"If true the weapon will be holstered on the side of the player, otherwise it will be attached to the back"
		))
	bool bHolsterOnSide;

	// Leave empty to hit everything on initial test
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectQueryList = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<FImpactDatabase> ImpactDatabases = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UNiagaraSystem> TracerSystem = {};

	bool GetImpact(EPhysicalSurface Surface, FImpactDatabase& ImpactDatabase);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (ToolTip = "Stats of the weapon"))
	FWeaponStats Stats;
};

USTRUCT()
struct WEAPONSYSTEM_API FNoctDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

	FName HitBone = NAME_None;
	FVector Impulse = FVector::ZeroVector;
	float HeadShotMultiplier = 1.0f;
	bool bBleedThrough = false;
};
