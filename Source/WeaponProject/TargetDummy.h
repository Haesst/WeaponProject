// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DummyDamageInterface.h"
#include "TargetDummy.generated.h"

UCLASS()
class WEAPONPROJECT_API ATargetDummy : public AActor, public IDummyDamageInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATargetDummy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Settings")
	bool bMovingTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Settings")
	bool bIsShooting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Settings")
	float MovementSpeed = 10.0f;
};
