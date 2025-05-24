// Copyright Nocturnum Games 2024


#include "Grenade.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AGrenade::AGrenade()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	TrailParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailParticles"));
	TrailParticles->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));

	// Init everything
	SphereComponent->SetCollisionProfileName("Projectile");
	SphereComponent->InitSphereRadius(10.0f);

	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->InitialSpeed = 500.0f;
	ProjectileMovementComponent->MaxSpeed = 500.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.8f;
}

void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}

float AGrenade::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float Damage =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Damage >= DetonationDamage)
	{
		Destroy();
	}

	return Damage;
}

void AGrenade::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bHasHadFirstBounce)
	{
		bHasHadFirstBounce = true;
		SetLifeSpan(DetonationTime);
	}

	if (Hit.PhysMaterial.IsValid())
	{
		FImpactDatabase ImpactDatabase;
		GetImpact(Hit.PhysMaterial->SurfaceType, ImpactDatabase);

		if (ImpactDatabase.BounceSoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactDatabase.BounceSoundEffect, GetActorLocation(),
			                                      GetActorRotation(), 1, 1, 0, ImpactDatabase.AttenuationSettings,
			                                      ImpactDatabase.ConcurrencySettings, nullptr,
			                                      ImpactDatabase.InitialParams);
		}
	}
}

void AGrenade::PostInitProperties()
{
	Super::PostInitProperties();
	SphereComponent->OnComponentHit.AddDynamic(this, &AGrenade::OnComponentHit);
}

void AGrenade::Destroyed()
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bReturnPhysicalMaterial = true;

	FImpactDatabase ImpactDatabase;
	GetImpact(SurfaceType_Default, ImpactDatabase);

	if (FHitResult SurfaceTestHitResult; GetWorld()->LineTraceSingleByChannel(
		SurfaceTestHitResult, GetActorLocation(), GetActorLocation() + FVector::DownVector * SurfaceLineTraceLength,
		DamagePreventionChannel, QueryParams))
	{
		if (GetImpact(SurfaceTestHitResult.PhysMaterial->SurfaceType, ImpactDatabase))
		{
			UMaterialInterface* SelectedDecal = nullptr;

			if (ImpactDatabase.ImpactDecals.Num() > 0)
			{
				SelectedDecal = ImpactDatabase.ImpactDecals[FMath::RandRange(0, ImpactDatabase.ImpactDecals.Num() - 1)];
			}

			if (SelectedDecal)
			{
				UGameplayStatics::SpawnDecalAttached(SelectedDecal, ImpactDatabase.DecalBaseSize,
				                                     SurfaceTestHitResult.GetComponent(), SurfaceTestHitResult.BoneName,
				                                     SurfaceTestHitResult.ImpactPoint,
				                                     SurfaceTestHitResult.ImpactNormal.Rotation(),
				                                     EAttachLocation::Type::KeepRelativeOffset,
				                                     ImpactDatabase.DecalLifespan);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(0, 4, FColor::Red,
			                                 FString::Printf(
				                                 TEXT("Failed to find impact database for surface for grenade: %s"),
				                                 *this->GetName()));
		}
	}

	if (ImpactDatabase.ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactDatabase.ImpactEffect, GetActorLocation());
	}

	if (ImpactDatabase.ImpactSoundEffect)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactDatabase.ImpactSoundEffect, GetActorLocation(),
		                                      GetActorRotation(), 1, 1, 0, ImpactDatabase.BounceAttenuationSettings,
		                                      ImpactDatabase.BounceConcurrencySettings, nullptr,
		                                      ImpactDatabase.BounceInitialParams);
	}

	UGameplayStatics::ApplyRadialDamageWithFalloff(this, BaseDamage, MinDamage, GetActorLocation(), InnerRadius,
	                                               OuterRadius, DamageFalloff, DamageType, {this}, this,
	                                               GetInstigatorController(), DamagePreventionChannel);
	Super::Destroyed();
}

void AGrenade::Launch(const FVector& ThrowersVelocity) const
{
	FVector LaunchVelocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
	LaunchVelocity += ThrowersVelocity;
	ProjectileMovementComponent->Velocity = LaunchVelocity;
}

bool AGrenade::GetImpact(const EPhysicalSurface Surface, FImpactDatabase& ImpactDatabase)
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
