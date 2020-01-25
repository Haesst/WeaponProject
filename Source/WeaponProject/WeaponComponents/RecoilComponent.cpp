// Fill out your copyright notice in the Description page of Project Settings.
/** 

	This class adds a spread to the bullet implemented in AWeaponBase.cpp

*/

#include "RecoilComponent.h"

URecoilComponent::URecoilComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URecoilComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URecoilComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FRotator URecoilComponent::AddRecoil()
{
	float recoilYaw = FMath::RandRange(MinYaw, MaxYaw);
	float recoilPitch = FMath::RandRange(MinPitch, MaxPitch);

	return FRotator(recoilYaw, recoilPitch, 0);
}