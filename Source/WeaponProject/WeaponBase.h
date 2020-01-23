// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include <Engine/EngineTypes.h>
#include "WeaponBase.generated.h"

UCLASS()
class WEAPONPROJECT_API AWeaponBase : public AActor
{
	GENERATED_BODY()

private:
	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh") //Mesh to add recoil rotation to
	class USkeletalMeshComponent* Mesh;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UBoxComponent* BoxCollider;

	FTimerHandle SpreadResetHandle; //For spread reset timer

public:	
	// Sets default values for this actor's properties
	AWeaponBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void OnConstruction(const FTransform& Transform);

	/* Rotation given to bullet being spawned */
	FRotator SpawnRotation;

	/* Default Recoil Rotation */

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Owner")
	class AActor* WeaponOwner;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AWeaponProjectProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Shooting")
	bool bCanFire = true;

	/** Fires a projectile. */
	UFUNCTION(BlueprintCallable, Category = "Shooting")
	void OnFire();
	
	UFUNCTION(BlueprintCallable, Category = "Alternative Fire")
	void OnAlternativeFire();

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlapComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(class UPrimitiveComponent* OverlapComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void BurstFire(FActorSpawnParameters SpawnParameters, FVector SpawnLocation);

	void FireBullet(FActorSpawnParameters SpawnParameters, FVector SpawnLocation);
	void FireRepeatingBullet();
	virtual void OnInteract();

	void FullAutoFire();
	void FullAutoFire(bool bFireTriggerIsPressed);

	UFUNCTION()
	void OnResetSpread();
	void EndPlay(const EEndPlayReason::Type EndPlayReason);

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	FTimerDelegate BurstTimerDel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float TimeToReset = 10.f;

	void ReloadWeapon();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Stats")
	float fireRate;

	FTimerHandle BurstTimerHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Burst Values")
	int NumOfBurstShots = 3;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Burst Values")
	float BurstInterval = .1f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Stats")
	float spread;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Recoil")
	class URecoilComponent* RecoilClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Magazine")
	class UMagazineComponent* MagazineClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon Select Fire")
	class USelectiveFireComponent* SelectiveFireClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Magazine")
	class UMuzzleComponent* MuzzleClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Magazine")
	class UAlternativeFireComponent* AlternativeFireClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Recoil")
	bool bResetSpreadAccuracy = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Recoil")
	int MaxShotsUntilSpread = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Recoil")
	int TimesFired = 0;

	bool bIsOverlaping = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetWeaponComponents();
	bool WeaponCanFire();
	void PlayFireEffects();
	FVector GetBulletSpawnLocation();
	FActorSpawnParameters GetBulletSpawnParameters();
	void SetBulletSpawnRotation();
	void SpawnBullet(FVector SpawnLocation, FActorSpawnParameters ActorSpawnParams);
};
