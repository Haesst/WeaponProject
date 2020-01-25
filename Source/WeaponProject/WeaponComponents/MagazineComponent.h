// Fill out your copyright notice in the Description page of Project Settings.

/** *


		This is a MagazineComponent for weapons that holds magazine related things.

*/
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagazineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEAPONPROJECT_API UMagazineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMagazineComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Magazine Stats")
	int CurrentMagazineSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Magazine Stats")
	int MaxMagazineSize;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ReloadMagazine();
		
};
