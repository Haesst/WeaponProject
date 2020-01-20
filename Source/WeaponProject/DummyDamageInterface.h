// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DummyDamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDummyDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WEAPONPROJECT_API IDummyDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "OnDummyHit")
	void DummyHit(float InDamage, const FString& HitLocation);
	virtual void DummyHit_Implementation(float InDamage, const FString& HitLocation) { return; };
};
