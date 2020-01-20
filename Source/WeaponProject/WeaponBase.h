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

	FTimerHandle SpreadResetHandle; //For spread reset timer

public:	
	// Sets default values for this actor's properties
	AWeaponBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Rotation given to bullet being spawned */
	FRotator SpawnRotation;

	/* Default Recoil Rotation */

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AWeaponProjectProjectile> ProjectileClass;

	/** Fires a projectile. */
	UFUNCTION(BlueprintCallable, Category = "Shooting")
	void OnFire();

	UFUNCTION()
	void OnResetSpread();
	void EndPlay(const EEndPlayReason::Type EndPlayReason);

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float TimeToReset = 10.f;

	void ReloadWeapon();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Stats")
	float fireRate;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Stats")
	float spread;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Recoil")
	class URecoilComponent* RecoilClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Magazine")
	class UMagazineComponent* MagazineClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Magazine")
	class UMuzzleComponent* MuzzleClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Recoil")
	bool bResetSpreadAccuracy = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Recoil")
	int MaxShotsUntilSpread = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon Recoil")
	int TimesFired = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
