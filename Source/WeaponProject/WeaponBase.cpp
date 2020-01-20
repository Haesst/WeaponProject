// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "WeaponProjectProjectile.h"
#include "WeaponProjectCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include <Components/SceneComponent.h>
#include "WeaponComponents/RecoilComponent.h"
#include "WeaponComponents/MagazineComponent.h"
#include "WeaponComponents/MuzzleComponent.h"
#include <TimerManager.h>

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* rootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = rootScene;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	RecoilClass = FindComponentByClass<URecoilComponent>();
	MagazineClass = FindComponentByClass<UMagazineComponent>();
	MuzzleClass = FindComponentByClass<UMuzzleComponent>();

	if (MuzzleClass)
	{
		MuzzleClass->SetMuzzleLocation(FP_MuzzleLocation);
	}


	MagazineClass->CurrentMagazineSize = MagazineClass->MaxMagazineSize;

	TimerDel.BindUFunction(this, FName("OnResetSpread"));
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL && MagazineClass != nullptr && MagazineClass->CurrentMagazineSize > 0)
	{
		UWorld* const World = GetWorld();

		FRotator RecoilRotator = FP_MuzzleLocation->GetRelativeRotation_DirectMutable();

		if (bResetSpreadAccuracy || TimesFired <= MaxShotsUntilSpread)
		{
			SpawnRotation = GetActorRotation();
			GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, TimeToReset, false);
			//OnResetSpread();
		}

		else if (TimesFired >= MaxShotsUntilSpread || bResetSpreadAccuracy)
		{
			SpawnRotation = GetActorRotation() + RecoilClass->AddRecoil();
		}

		if (TimesFired == 1)
		{
			GetWorld()->GetTimerManager().SetTimer(SpreadResetHandle, this, &AWeaponBase::OnResetSpread, 3.0f, 0.0f);
		}

		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		if (World != NULL)
		{
			AWeaponProjectProjectile* bullet = GetWorld()->SpawnActor<AWeaponProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			bullet->Init(0);
			TimesFired++;

			MagazineClass->CurrentMagazineSize--;

			// try and play the sound if specified
			if (FireSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}
			if (MuzzleClass)
			{
				MuzzleClass->SpawnMuzzleFlash();
			}
		}
	}
}

void AWeaponBase::OnResetSpread()
{
	TimesFired = 0;
}

void AWeaponBase::ReloadWeapon()
{
	MagazineClass->ReloadMagazine();
}

void AWeaponBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(SpreadResetHandle);
}
