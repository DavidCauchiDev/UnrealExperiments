// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponData.h"
#include "GameFramework/Actor.h"
#include "WeaponAttachment.generated.h"

UCLASS()
class WEAPONSYSTEM_API AWeaponAttachment : public AActor
{
	GENERATED_BODY()

public:
	AWeaponAttachment();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponAttachment")
	FAttachmentWeaponStatAdjustments WeaponStatTweaks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponAttachmentType AttachmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponAttachment")
	TObjectPtr<USkeletalMeshComponent> AttachmentMesh;

	void NativeOnEquipped();
	void NativeOnUnequipped();
};


UCLASS()
class WEAPONSYSTEM_API AWeaponMagazine : public AWeaponAttachment
{
	GENERATED_BODY()

public:
	AWeaponMagazine();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="WeaponAttachment", meta=(ClampMin=1))
	int AmmoCapacity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="WeaponAttachment", meta=(ClampMin=1))
	int AmmoLeft = 30;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="WeaponAttachment")
	FGameplayTag AmmoTag;
};


UCLASS()
class WEAPONSYSTEM_API AWeaponMuzzle : public AWeaponAttachment
{
	GENERATED_BODY()

public:
	AWeaponMuzzle();

	UFUNCTION(BlueprintPure)
	FTransform GetMuzzleTransform();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponAttachment")
	FName MuzzleSocket = TEXT("Muzzle");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponAttachment")
	bool bIsSupressor = false;
};
