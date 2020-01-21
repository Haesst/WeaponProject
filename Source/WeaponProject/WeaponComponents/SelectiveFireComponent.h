// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SelectiveFireComponent.generated.h"


UENUM(BlueprintType)
enum class ESelectiveFire : uint8
{
	SFE_FullAuto UMETA(DisplayName = "Full Auto"),
	SFE_SemiAuto UMETA(DisplayName = "Semi Auto"),
	SFE_BurstFire UMETA(DisplayName = "Burst Fire")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEAPONPROJECT_API USelectiveFireComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USelectiveFireComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selective Fire Enum")
	ESelectiveFire SelectiveFireEnum;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
