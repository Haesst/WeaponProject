// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "WeaponProjectCharacter.h"
#include "WeaponProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine.h"
#include "EngineGlobals.h"
#include "Engine/World.h"
#include "WeaponBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AWeaponProjectCharacter

AWeaponProjectCharacter::AWeaponProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	SpawnInfo.Owner = this;
	SpawnInfo.Instigator = Instigator;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
}

void AWeaponProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	int index = 0;

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
}

void AWeaponProjectCharacter::CreateWeapons()
{
	for (auto Weapon : UserWeaponList)
	{
		WeaponSpawn = (UClass*)Weapon->GeneratedClass;

		CodeWeaponList.Add(GetWorld()->SpawnActor<AWeaponBase>(WeaponSpawn, SpawnInfo));
		CodeWeaponList.Last()->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
		CodeWeaponList.Last()->SetActorHiddenInGame(true);
	}

	if (CodeWeaponList.Num() > 0 && CodeWeaponList[0])
	{
		CodeWeaponList[0]->SetActorHiddenInGame(false);
		CurrentWeapon = CodeWeaponList[0];
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AWeaponProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up game play key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("WeaponSwitchUp", IE_Pressed, this, &AWeaponProjectCharacter::ChangeWeaponUp);
	PlayerInputComponent->BindAction("WeaponSwitchDown", IE_Pressed, this, &AWeaponProjectCharacter::ChangeWeaponDown);
	// Bind fire event
	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACPPWeaponProjectCharacter::CharacterFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AWeaponProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWeaponProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turn rate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AWeaponProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AWeaponProjectCharacter::LookUpAtRate);
}

void AWeaponProjectCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AWeaponProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AWeaponProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWeaponProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AWeaponProjectCharacter::ChangeWeaponUp()
{
	if (CodeWeaponList.Num() > 0)
	{
		CodeWeaponList[WeaponIndex]->SetActorHiddenInGame(true);

		WeaponIndex++;
		if (WeaponIndex > CodeWeaponList.Num() - 1)
		{
			WeaponIndex = 0;
		}
		CodeWeaponList[WeaponIndex]->SetActorHiddenInGame(false);

		CurrentWeapon = CodeWeaponList[WeaponIndex];
	}
}

void AWeaponProjectCharacter::ChangeWeaponDown()
{
	if (CodeWeaponList.Num() > 0)
	{
		CodeWeaponList[WeaponIndex]->SetActorHiddenInGame(true);

		WeaponIndex--;
		if (WeaponIndex < 0)
		{
			WeaponIndex = CodeWeaponList.Num() - 1;
		}
		CodeWeaponList[WeaponIndex]->SetActorHiddenInGame(false);

		CurrentWeapon = CodeWeaponList[WeaponIndex];
	}
}

void AWeaponProjectCharacter::CharacterFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->OnFire();
	}
}

void AWeaponProjectCharacter::CharacterReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->ReloadWeapon();
	}
}


