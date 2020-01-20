// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetDummy.h"

ATargetDummy::ATargetDummy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ATargetDummy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATargetDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

