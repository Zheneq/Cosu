// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "Net/UnrealNetwork.h"
#include "InventoryComponent.h"
#include "InventoryDroppedItem.h"


// Sets default values
AInventoryDroppedItem::AInventoryDroppedItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshOb_torus(TEXT("StaticMesh'/Engine/EngineMeshes/Cube.Cube'"));
	if (StaticMeshOb_torus.Object)
	{
		StaticMeshComponent->SetStaticMesh(StaticMeshOb_torus.Object);
		StaticMeshComponent->SetWorldScale3D(FVector(.15f, .15f, .15f));
	}

	RootComponent = StaticMeshComponent;


	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void AInventoryDroppedItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInventoryDroppedItem, InventoryComponent);
	DOREPLIFETIME(AInventoryDroppedItem, StaticMeshComponent);

}