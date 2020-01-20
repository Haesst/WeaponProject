// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RecoilComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEAPONPROJECT_API URecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URecoilComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Values")
	float MinYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Values")
	float MaxYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Values")
	float MinPitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Values")
	float MaxPitch;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Add recoil when the player fires the weapon*/
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	FRotator AddRecoil();
};
