// Fill out your copyright notice in the Description page of Project Settings.
/** 
		
		This class adds an enum which can be set in blueprint, this enum decides the weapons fire mode (Burst, semi, Full auto)
	
*/

#include "SelectiveFireComponent.h"

// Sets default values for this component's properties
USelectiveFireComponent::USelectiveFireComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void USelectiveFireComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USelectiveFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

