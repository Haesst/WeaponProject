// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

/** *


		This is the Playable Character class that Player controlled characters should derive from.

*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponBase.h"
#include "WeaponProjectCharacter.generated.h"

class UInputComponent;
class AWeaponBase;

UCLASS(config=Game)
class AWeaponProjectCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	float DefaultFOV = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Trigger Capsule")
	class UCapsuleComponent* TriggerCapsule;
protected:
	bool bIsOverlaping;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon list")
	TArray<UBlueprint*> UserWeaponList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon list")
	AWeaponBase* CurrentWeapon;

	AWeaponBase* WeaponActor;

	TSubclassOf<class AWeaponBase> WeaponSpawn;

public:
	TArray <AWeaponBase*> CodeWeaponList;
	FActorSpawnParameters SpawnInfo;

public:

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlapComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(class UPrimitiveComponent* OverlapComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void CharacterFire();

	UFUNCTION(BlueprintCallable)
	void CharacterStopFire();

	UFUNCTION(BlueprintCallable)
	void CharacterReload();

	UFUNCTION(BlueprintCallable)
	void CharacterAlternativeFire();

	UPROPERTY(BlueprintReadWrite)
	int WeaponIndex = 0;

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	AWeaponProjectCharacter();
	void AddToInventory(AWeaponBase* weapon);

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);
	/** Handles strafing movement, left and right */
	void MoveRight(float Val);
	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);
	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION()
	void ChangeWeapon(int modifier);
	void PickUpWeapon();
	virtual void BeginPlay();

private:
	void CreateWeapons();
	void ResetFOV();
};