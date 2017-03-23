// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "Net/UnrealNetwork.h"
#include "InventoryItemInterface.h"
#include "InventoryDroppedItem.h"
#include "InventoryItem.h"

// Default actor to spawn when an item is dropped
const TSubclassOf<AActor> AInventoryItem::DefaultItemClass = AInventoryDroppedItem::StaticClass();

AInventoryItem::AInventoryItem()
{
	bReplicates = true;

	bStackable = true;
	Count = 1;
	MaxCount = 0;
	bNotDroppable = false;
	bHidden = false;
}


void AInventoryItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInventoryItem, bStackable);
	DOREPLIFETIME(AInventoryItem, Count);
	DOREPLIFETIME(AInventoryItem, Stats);
	DOREPLIFETIME(AInventoryItem, bNotDroppable);

}


FText AInventoryItem::GetDisplayName() const
{
	return DisplayName;
}

bool AInventoryItem::IsStackable() const
{
	return bStackable && !Stats.Num();
}