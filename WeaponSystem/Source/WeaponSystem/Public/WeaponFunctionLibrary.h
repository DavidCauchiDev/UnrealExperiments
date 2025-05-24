// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WeaponFunctionLibrary.generated.h"

class AWeapon;
class URecoilData;

UCLASS()
class WEAPONSYSTEM_API UWeaponFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = Weapons,
		meta = (Keywords = "projectile trace", WorldContext = "WorldContextObject", UnsafeDuringActorConstruction =
			"true"))
	static bool HandleWeaponLineTraceAttack(const UObject* WorldContextObject, FHitResult& HitResult,
	                                        const FVector& Start,
	                                        const FVector& Direction, AWeapon* Weapon, AActor* Owner,
	                                        AController* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Weapon,
		meta = (Keywords = "weapon fire rate", UnsafeDuringActorConstruction = "true"))
	static float GetFireRateInSeconds(class AWeapon* Weapon);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Weapon,
		meta = (Keywords = "weapon fire rate", UnsafeDuringActorConstruction = "true"))
	static int GetFireRate(class AWeapon* Weapon);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Weapon,
		meta = (Keywords = "weapon fire rate", UnsafeDuringActorConstruction = "true"))
	static int GetBurstCount(class AWeapon* Weapon);
};
