// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponData.h"

void FWeaponStats::ApplyAttributeChanges(FWeaponStats& To, const FAttachmentWeaponStatAdjustments& From)
{
	To.SpreadXMinInDegrees += From.SpreadXMinInDegrees;
	To.SpreadXMaxInDegrees += From.SpreadXMaxInDegrees;
	To.SpreadYMinInDegrees += From.SpreadYMinInDegrees;
	To.SpreadYMaxInDegrees += From.SpreadYMaxInDegrees;
	To.FireRate += From.FireRate;
	To.BurstFireRate += From.BurstFireCooldown;
	To.BurstCount += From.BurstCount;
}

void FWeaponStats::UnapplyAttributeChanges(const FAttachmentWeaponStatAdjustments& From, FWeaponStats& To)
{
	To.SpreadXMinInDegrees -= From.SpreadXMinInDegrees;
	To.SpreadXMaxInDegrees -= From.SpreadXMaxInDegrees;
	To.SpreadYMinInDegrees -= From.SpreadYMinInDegrees;
	To.SpreadYMaxInDegrees -= From.SpreadYMaxInDegrees;
	To.FireRate -= From.FireRate;
	To.BurstFireRate -= From.BurstFireCooldown;
	To.BurstCount -= From.BurstCount;
}

bool UWeaponData::GetImpact(const EPhysicalSurface Surface, FImpactDatabase& ImpactDatabase)
{
	for (auto Database : ImpactDatabases)
	{
		if (Database.SurfaceType == Surface)
		{
			ImpactDatabase = Database;
			return true;
		}
	}
	return false;
}
