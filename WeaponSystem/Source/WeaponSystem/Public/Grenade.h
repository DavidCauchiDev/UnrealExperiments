

// Copyright Nocturnum Games 2024

#pragma once

#include "CoreMinimal.h"
#include "WeaponData.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class WEAPONSYSTEM_API AGrenade : public AActor
{
	GENERATED_BODY()

public:
	AGrenade();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
	float DetonationTime = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
	float BaseDamage = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
	float MinDamage = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
	float InnerRadius = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
	float OuterRadius = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
	float DamageFalloff = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
	float SurfaceLineTraceLength = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
	TArray<FImpactDatabase> ImpactDatabases = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
	TEnumAsByte<ECollisionChannel> DamagePreventionChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
	float DetonationDamage = 30;
	
	// Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade|Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade|Components")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade|Components")
	TObjectPtr<UNiagaraComponent> TrailParticles;


	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    FVector NormalImpulse, const FHitResult& Hit);

	virtual void PostInitProperties() override;
	virtual void Destroyed() override;
	void Launch(const FVector& ThrowersVelocity) const;

private:
	bool bHasHadFirstBounce = false;
	bool GetImpact(EPhysicalSurface Surface, FImpactDatabase& ImpactDatabase);
};
