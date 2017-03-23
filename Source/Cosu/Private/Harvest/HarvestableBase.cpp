// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "HarvestableBase.h"


// Sets default values
AHarvestableBase::AHarvestableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHarvestableBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHarvestableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
