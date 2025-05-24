// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponAttachment.h"

AWeaponAttachment::AWeaponAttachment()
{
	PrimaryActorTick.bCanEverTick = false;
	AttachmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Attachment Mesh Component"));
	RootComponent = AttachmentMesh;
	AttachmentMesh->bCastHiddenShadow = true;
}

void AWeaponAttachment::NativeOnEquipped()
{
	SetActorHiddenInGame(false);
}

void AWeaponAttachment::NativeOnUnequipped()
{
	SetActorHiddenInGame(true);
}

AWeaponMagazine::AWeaponMagazine()
{
	AttachmentType = EWeaponAttachmentType::Magazine;
}

AWeaponMuzzle::AWeaponMuzzle()
{
	AttachmentType = EWeaponAttachmentType::Muzzle;
}

FTransform AWeaponMuzzle::GetMuzzleTransform()
{
	FTransform MuzzleTransform = GetActorTransform();

	if (ensure(AttachmentMesh->DoesSocketExist(MuzzleSocket)))
	{
		MuzzleTransform = AttachmentMesh->GetSocketTransform(MuzzleSocket);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
		                                 TEXT(
			                                 "ATTACHMENT MESH DOES NOT HAVE MUZZLE SOCKET!!! SPAWNING FROM ATTACHMENT TRANSFORM."));
	}

	return MuzzleTransform;
}
