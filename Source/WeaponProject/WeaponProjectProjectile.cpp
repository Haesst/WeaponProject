// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "WeaponProjectProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine.h"
#include "EngineGlobals.h"

AWeaponProjectProjectile::AWeaponProjectProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AWeaponProjectProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = InitialSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AWeaponProjectProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if (CheckForCollisionPhysics(OtherActor, OtherComp))
	{
		// Try to apply test dummy damage if the hit actor implements it
		ApplyTestDummyDamage(OtherActor, Hit);

		if (OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		}

		Destroy();
	}
}

void AWeaponProjectProjectile::Init(float Velocity)
{
	ProjectileMovement->InitialSpeed = Velocity;
}

void AWeaponProjectProjectile::OnConstruction(const FTransform& Transform)
{
	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = InitialSpeed + 0.f;
}

bool AWeaponProjectProjectile::CheckForCollisionPhysics(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	return (OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr);
}

/**
	Try to execute a dummy hit if the hit target is a test dummy.
*/
void AWeaponProjectProjectile::ApplyTestDummyDamage(AActor* OtherActor, const FHitResult& Hit)
{
	IDummyDamageInterface* TestDummyHit = Cast<IDummyDamageInterface>(OtherActor);

	if (TestDummyHit)
	{
		FString boneName = Hit.BoneName.ToString();
		TestDummyHit->Execute_DummyHit(OtherActor, Damage, *boneName);
	}
}
