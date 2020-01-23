// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "WeaponProjectProjectile.h"
#include "WeaponProjectCharacter.h"
#include "GameFramework/Actor.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include <Components/SceneComponent.h>
#include "WeaponComponents/RecoilComponent.h"
#include "WeaponComponents/SelectiveFireComponent.h"
#include "WeaponComponents/MagazineComponent.h"
#include "WeaponComponents/MuzzleComponent.h"
#include "WeaponComponents/AlternativeFireComponent.h"
#include "Camera/CameraComponent.h"
#include <TimerManager.h>

// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* rootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = rootScene;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetupAttachment(RootComponent);

	// Set MuzzleLocation
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetupAttachment(FP_Gun);
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	BoxCollider->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnOverlapBegin);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnOverlapEnd);

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	SetWeaponComponents();
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	TimerDel.BindUFunction(this, FName("OnResetSpread"));



	MagazineClass->CurrentMagazineSize = MagazineClass->MaxMagazineSize;

}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::SetWeaponComponents()
{
	RecoilClass = CreateDefaultSubobject<URecoilComponent>(TEXT("RecoilComponent"));
	MagazineClass = CreateDefaultSubobject<UMagazineComponent>(TEXT("MagazineComponent"));
	MuzzleClass = CreateDefaultSubobject<UMuzzleComponent>(TEXT("MuzzleComponent"));
	SelectiveFireClass = CreateDefaultSubobject<USelectiveFireComponent>(TEXT("Selective Fire"));
	AlternativeFireClass = CreateDefaultSubobject<UAlternativeFireComponent>(TEXT("Alternative Fire"));

	RecoilClass->bEditableWhenInherited = true;
	MagazineClass->bEditableWhenInherited = true;
	MuzzleClass->bEditableWhenInherited = true;
	SelectiveFireClass->bEditableWhenInherited = true;
	AlternativeFireClass->bEditableWhenInherited = true;

	if (MuzzleClass)
	{
		MuzzleClass->SetMuzzleLocation(FP_MuzzleLocation);
	}
}

void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	if (WeaponOwner == nullptr)
	{
		WeaponOwner = GetOwner();
	}

	if (WeaponOwner)
	{
		if (WeaponOwner->IsA<AWeaponProjectCharacter>())
		{
			AlternativeFireClass->Init(SelectiveFireClass, Cast<AWeaponProjectCharacter>(WeaponOwner)->GetFirstPersonCameraComponent());
		}
	}
}

bool AWeaponBase::WeaponCanFire()
{
	return (ProjectileClass != nullptr) && (MagazineClass != nullptr) && (MagazineClass->CurrentMagazineSize > 0);
}

void AWeaponBase::PlayFireEffects()
{
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	if (MuzzleClass)
	{
		MuzzleClass->SpawnMuzzleFlash();
	}
}

FVector AWeaponBase::GetBulletSpawnLocation()
{
	return ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
}

FActorSpawnParameters AWeaponBase::GetBulletSpawnParameters()
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	return ActorSpawnParams;
}

void AWeaponBase::SetBulletSpawnRotation()
{
	if (bResetSpreadAccuracy || TimesFired <= MaxShotsUntilSpread)
	{
		SpawnRotation = GetActorRotation();
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, TimeToReset, false);
	}
	else if (TimesFired >= MaxShotsUntilSpread || bResetSpreadAccuracy)
	{
		SpawnRotation = GetActorRotation() + RecoilClass->AddRecoil();
	}
}

void AWeaponBase::SpawnBullet(FVector SpawnLocation, FActorSpawnParameters ActorSpawnParams)
{
	AWeaponProjectProjectile* bullet = GetWorld()->SpawnActor<AWeaponProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

	if (bullet != nullptr)
	{
		bullet->Init(0);
		TimesFired++;
		MagazineClass->CurrentMagazineSize--;

		PlayFireEffects();
	}
}

void AWeaponBase::OnFire()
{
	// try and fire a projectile
	if (WeaponCanFire())
	{
		SetBulletSpawnRotation();

		if (TimesFired == 1)
		{
			GetWorld()->GetTimerManager().SetTimer(SpreadResetHandle, this, &AWeaponBase::OnResetSpread, 3.0f, 0.0f);
		}

		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetBulletSpawnLocation();

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams = GetBulletSpawnParameters();


		switch (SelectiveFireClass->SelectiveFireEnum)
		{
		case ESelectiveFire::SFE_BurstFire:

			BurstFire(ActorSpawnParams, SpawnLocation);

			break;

		case ESelectiveFire::SFE_FullAuto:

			FullAutoFire();
			break;

		case ESelectiveFire::SFE_SemiAuto:

			FireBullet(ActorSpawnParams, SpawnLocation);

			break;
		}
	}
}

void AWeaponBase::BurstFire(FActorSpawnParameters SpawnParameters, FVector SpawnLocation)
{
	if (TimesFired <= 3)
	{
		if (SelectiveFireClass->SelectiveFireEnum == ESelectiveFire::SFE_BurstFire)
		{
			GetWorldTimerManager().SetTimer(BurstTimerHandle, this, &AWeaponBase::FireRepeatingBullet, BurstInterval, true, BurstInterval);
			bCanFire = false;
		}
	}
}

void AWeaponBase::FireBullet(FActorSpawnParameters SpawnParameters, FVector SpawnLocation)
{
	if (GetWorld() != nullptr)
	{
		SpawnBullet(SpawnLocation, SpawnParameters);
	}
}

void AWeaponBase::FireRepeatingBullet()
{
	if (GetWorld() != nullptr)
	{
		if (SelectiveFireClass->SelectiveFireEnum == ESelectiveFire::SFE_FullAuto)
		{
			SetBulletSpawnRotation();
			SpawnBullet(GetBulletSpawnLocation(), GetBulletSpawnParameters());

		}
	}

	if (TimesFired >= NumOfBurstShots && SelectiveFireClass->SelectiveFireEnum == ESelectiveFire::SFE_BurstFire) //REFACTOR BEFORE TURN IN PLS
	{
		GetWorldTimerManager().ClearTimer(BurstTimerHandle);
		TimesFired = 0;
		bCanFire = true;
	}
}

void AWeaponBase::FullAutoFire()
void AWeaponBase::FullAutoFire(bool bFireTriggerIsPressed)
{

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

void AWeaponBase::OnAlternativeFire()
{
	AlternativeFireClass->TriggerAlternativeFire();
}

void AWeaponBase::OnOverlapBegin(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//check if player we're interacting with
	if (OtherActor && (OtherActor != this) && OtherComp)
	{

		bIsOverlaping = true;
	}
}

void AWeaponBase::OnOverlapEnd(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		bIsOverlaping = false;
	}
}

void AWeaponBase::OnInteract()
{
	if (bIsOverlaping)
	{
		AWeaponProjectCharacter* Player = Cast<AWeaponProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

		if (Player)
		{
			Player->AddToInventory(this);
			BoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			WeaponOwner = Player;
			if (WeaponOwner && WeaponOwner->IsA<AWeaponProjectCharacter>())
			{
				AlternativeFireClass->Init(SelectiveFireClass, Cast<AWeaponProjectCharacter>(WeaponOwner)->GetFirstPersonCameraComponent());
			}
		}
	}
}