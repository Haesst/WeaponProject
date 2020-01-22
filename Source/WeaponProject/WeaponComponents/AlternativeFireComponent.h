// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SelectiveFireComponent.h"
#include "Camera/CameraComponent.h"
#include "AlternativeFireComponent.generated.h"


UENUM(BlueprintType)
enum class EAlternativeFire : uint8
{
	AFE_None UMETA(DisplayName = "None"),
	AFE_BurstFire UMETA(DisplayName = "Burst Fire"),
	AFE_Sight UMETA(DisplayName = "Sight"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEAPONPROJECT_API UAlternativeFireComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	USelectiveFireComponent* SelectiveFireComponent;
	TArray<UCameraComponent*> AimCameras;
	UCameraComponent* MainCamera;
	int CurrentCamera = 0;
	bool Aiming = false;
	float DefaultFOV = 0.0f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alternative Fire Enum")
	EAlternativeFire AlternativeFireEnum;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UAlternativeFireComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Init(USelectiveFireComponent* InSelectiveFireComponent, UCameraComponent* InMainCamera);
	void TriggerAlternativeFire();

private:
	void SwitchBurst();
	void SwitchSight();
};
