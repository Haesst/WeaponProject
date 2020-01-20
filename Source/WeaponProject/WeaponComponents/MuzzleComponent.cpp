// Fill out your copyright notice in the Description page of Project Settings.


#include "MuzzleComponent.h"
#include "Particles/ParticleSystemComponent.h"

UMuzzleComponent::UMuzzleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMuzzleComponent::BeginPlay()
{
	Super::BeginPlay();}


void UMuzzleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMuzzleComponent::SetMuzzleLocation(USceneComponent* InMuzzleLocation)
{
	MuzzleLocation = InMuzzleLocation;
}

void UMuzzleComponent::SpawnMuzzleFlash()
{
	if (IsMuzzleFlashSpawnable())
	{
		FTransform MuzzleTransform = MuzzleLocation->GetComponentTransform();

		UParticleSystemComponent* FlashParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
		FlashParticle->SetWorldScale3D(MuzzleFlashScale);
	}
}

bool UMuzzleComponent::IsMuzzleFlashSpawnable()
{
	return MuzzleFlash != nullptr && MuzzleLocation != nullptr;
}