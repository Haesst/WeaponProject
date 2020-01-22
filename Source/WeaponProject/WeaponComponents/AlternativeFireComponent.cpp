// Fill out your copyright notice in the Description page of Project Settings.

#include "AlternativeFireComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UAlternativeFireComponent::UAlternativeFireComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UAlternativeFireComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UAlternativeFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAlternativeFireComponent::TriggerAlternativeFire()
{
	switch (AlternativeFireEnum)
	{
	case EAlternativeFire::AFE_None:
		break;
	case EAlternativeFire::AFE_BurstFire:
		SwitchBurst();
		break;
	case EAlternativeFire::AFE_Sight:
		SwitchSight();
		break;
	}
}

void UAlternativeFireComponent::SwitchBurst()
{
	if (SelectiveFireComponent->SelectiveFireEnum == ESelectiveFire::SFE_BurstFire)
	{
		SelectiveFireComponent->SelectiveFireEnum = ESelectiveFire::SFE_SemiAuto;
	}
	else
	{
		SelectiveFireComponent->SelectiveFireEnum = ESelectiveFire::SFE_BurstFire;
	}
}

void UAlternativeFireComponent::SwitchSight()
{
	if (!Aiming)
	{
		DefaultFOV = MainCamera->FieldOfView;

		MainCamera->FieldOfView = DefaultFOV - 20.0f;
		Aiming = true;
	}
	else
	{
		MainCamera->FieldOfView = DefaultFOV;
		Aiming = false;
	}
}

void UAlternativeFireComponent::Init(USelectiveFireComponent* InSelectiveFireComponent, UCameraComponent* InMainCamera)
{
	this->SelectiveFireComponent = InSelectiveFireComponent;
	this->MainCamera = InMainCamera;
}