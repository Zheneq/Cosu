// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "Net/UnrealNetwork.h"
#include "SpawnMark.h"


// Sets default values
ASpawnMark::ASpawnMark()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMeshComponent;
}

// Called when the game starts or when spawned
void ASpawnMark::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnMark::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

