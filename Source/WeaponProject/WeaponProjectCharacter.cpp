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
	SpawnInfo.Instigator = GetInstigator();

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	DefaultFOV = FirstPersonCameraComponent->FieldOfView;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	//Create a trigger capsule
	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Capsule"));
	TriggerCapsule->InitCapsuleSize(55.f, 96.f);
	TriggerCapsule->SetCollisionProfileName(TEXT("Trigger"));
	TriggerCapsule->SetupAttachment(RootComponent);
}

void AWeaponProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	int index = 0;

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	CreateWeapons();

	//Declare overlap events
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &AWeaponProjectCharacter::OnOverlapBegin);
	TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &AWeaponProjectCharacter::OnOverlapEnd);
}

void AWeaponProjectCharacter::OnOverlapBegin(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp && SweepResult.GetComponent()->GetName() == "BoxCollider")
	{
		bIsOverlaping = true;
		WeaponActor = Cast<AWeaponBase>(OtherActor);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, (TEXT("%s"), SweepResult.GetComponent()->GetName()));
	}
}

void AWeaponProjectCharacter::OnOverlapEnd(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		bIsOverlaping = false;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("End Overlap"));
	}
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


	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AWeaponProjectCharacter::CharacterFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AWeaponProjectCharacter::CharacterStopFire);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("PickUpWeapon", IE_Pressed, this, &AWeaponProjectCharacter::PickUpWeapon);
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

	// Bind weapon switching to the same function but with different parameters
	{
		FInputActionBinding AB("WeaponSwitchUp", IE_Pressed);
		FInputActionHandlerSignature Delegate;
		Delegate.BindUObject(this, &AWeaponProjectCharacter::ChangeWeapon, 1);
		AB.ActionDelegate = Delegate;
		PlayerInputComponent->AddActionBinding(MoveTemp(AB));
	}
	{
		FInputActionBinding AB("WeaponSwitchDown", IE_Pressed);
		FInputActionHandlerSignature Delegate;
		Delegate.BindUObject(this, &AWeaponProjectCharacter::ChangeWeapon, -1);
		AB.ActionDelegate = Delegate;
		PlayerInputComponent->AddActionBinding(MoveTemp(AB));
	}
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

void AWeaponProjectCharacter::ChangeFireMode()
{
	CurrentWeapon->OnAlternativeFire();
}

void AWeaponProjectCharacter::ChangeWeapon(int modifier)
{
	if (CodeWeaponList.Num() > 0)
	{
		CodeWeaponList[WeaponIndex]->SetActorHiddenInGame(true);

		WeaponIndex += modifier;

		if (WeaponIndex < 0)
		{
			WeaponIndex = CodeWeaponList.Num() - 1;
		}
		else if (WeaponIndex > CodeWeaponList.Num() - 1)
		{
			WeaponIndex = 0;
		}

		CodeWeaponList[WeaponIndex]->SetActorHiddenInGame(false);
		CurrentWeapon = CodeWeaponList[WeaponIndex];

		ResetFOV();
	}
}

void AWeaponProjectCharacter::ResetFOV()
{
	if (GetFirstPersonCameraComponent()->FieldOfView != DefaultFOV)
	{
		GetFirstPersonCameraComponent()->FieldOfView = DefaultFOV;
	}
}

void AWeaponProjectCharacter::CharacterFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->bCanFire = true;
		CurrentWeapon->OnFire();
	}
}

void AWeaponProjectCharacter::CharacterStopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->bCanFire = false;
		CurrentWeapon->ResetFullAutoTimer();
	}
}

void AWeaponProjectCharacter::CharacterReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->ReloadWeapon();
	}
}

void AWeaponProjectCharacter::PickUpWeapon()
{
	if (bIsOverlaping)
	{
		WeaponActor->OnInteract();
	}
}

void AWeaponProjectCharacter::AddToInventory(AWeaponBase* weapon)
{
	CodeWeaponList.Add(weapon);
	CodeWeaponList.Last()->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
	CodeWeaponList.Last()->SetActorHiddenInGame(true);
	ChangeWeapon(1);
}