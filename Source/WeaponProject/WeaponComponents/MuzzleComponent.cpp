// Fill out your copyright notice in the Description page of Project Settings.


#include "MuzzleComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values for this component's properties
UMuzzleComponent::UMuzzleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMuzzleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMuzzleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMuzzleComponent::SetMuzzleLocation(USceneComponent* InMuzzleLocation)
{
	MuzzleLocation = InMuzzleLocation;
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, MuzzleLocation->GetName());
}

void UMuzzleComponent::SpawnMuzzleFlash()
{
	if (MuzzleFlash && MuzzleLocation)
	{
		FTransform MuzzleTransform = MuzzleLocation->GetComponentTransform();

		UParticleSystemComponent* FlashParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
		FlashParticle->SetWorldScale3D(MuzzleFlashScale);
	}
}
