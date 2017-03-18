// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryItem.h"
#include "InventoryItemUsable.generated.h"

/**
 * 
 */
UCLASS()
class AInventoryItemUsable : public AInventoryItem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Cosu Inventory")
		bool IsUsable() const;

	UFUNCTION(BlueprintCallable, Category = "Cosu Inventory")
		void Use();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Use"), Category = "Cosu Inventory")
		void Use_BP();
	
	
};
