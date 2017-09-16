// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "BuildableBase.h"


// Sets default values
ABuildableBase::ABuildableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// TODO: Is this needed?
	PrimaryActorTick.bCanEverTick = true;

	CanBeFoundedOn = 0;
	bCanBeFoundation = false;
	FoundationType = EBuildingFoundationType::F_Floor;
	bIgnoreCollisions = false;
	BuildingStage = EBuildingStageEnum::B_Placing;
	bCanBePlaced = false;


	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	if (Mesh)
	{
		RootComponent = Mesh;
	}
}

// Called when the game starts or when spawned
void ABuildableBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABuildableBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateState();

}

// Called every frame
void ABuildableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
bool ABuildableBase::IsValidLocation_Implementation()
{
	return true;
}

void ABuildableBase::SetCanBePlaced(bool bNewCanBePlaced)
{
	if (bCanBePlaced != bNewCanBePlaced)
	{
		bCanBePlaced = bNewCanBePlaced;
		UpdateMaterial();
	}
}

bool ABuildableBase::Place()
{
	if (bCanBePlaced)
	{
		BuildingStage = EBuildingStageEnum::B_InProgress;
		UpdateState();
		return true;
	}
	return false;
}

void ABuildableBase::UpdateState()
{
	if (!IsValid(Mesh))
	{
		return;
	}

	switch (BuildingStage)
	{
	case EBuildingStageEnum::B_Placing:
		Mesh->Mobility = EComponentMobility::Movable;
		SetActorEnableCollision(false);
		// TODO
		break;

	case EBuildingStageEnum::B_InProgress:
		// TODO
		Mesh->Mobility = EComponentMobility::Stationary;
		SetActorEnableCollision(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;

	case EBuildingStageEnum::B_Finished:
		// TODO
		Mesh->Mobility = EComponentMobility::Stationary;
		SetActorEnableCollision(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	default:
		// Destroy();
		break;

	}
}
