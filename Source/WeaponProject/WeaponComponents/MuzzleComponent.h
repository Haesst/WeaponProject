// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MuzzleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEAPONPROJECT_API UMuzzleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMuzzleComponent();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USceneComponent* MuzzleLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Muzzle Flash")
	UParticleSystem* MuzzleFlash;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Muzzle Flash")
	FVector MuzzleFlashScale;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetMuzzleLocation(USceneComponent* InMuzzleLocation);
	void SpawnMuzzleFlash();
		
};
