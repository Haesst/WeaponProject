// Fill out your copyright notice in the Description page of Project Settings.


#include "MagazineComponent.h"

UMagazineComponent::UMagazineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UMagazineComponent::ReloadMagazine()
{
	CurrentMagazineSize = MaxMagazineSize;
}

void UMagazineComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMagazineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

