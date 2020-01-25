// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

/** *


		This is a base projectile that every created projectile should derive from as it holds all the projectile functionality.

*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DummyDamageInterface.h"
#include "WeaponProjectProjectile.generated.h"

UCLASS(config=Game)
class AWeaponProjectProjectile : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float InitialSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float Damage = 30.0f;

public:
	AWeaponProjectProjectile();
	void Init(float velocity);
	void OnConstruction(const FTransform& Transform);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

private:
	bool CheckForCollisionPhysics(AActor* OtherActor, UPrimitiveComponent* OtherComp);
	void ApplyTestDummyDamage(AActor* OtherActor, const FHitResult& Hit);
};

