// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "InventoryComponent.h"
#include "InventoryItemUsable.h"

bool AInventoryItemUsable::IsUsable_Implementation() const
{
	return true;
}

void AInventoryItemUsable::Use()
{
	Use_BP();

	if (Inventory)
	{
		Inventory->OnInventoryUpdated.Broadcast();
	}
}
