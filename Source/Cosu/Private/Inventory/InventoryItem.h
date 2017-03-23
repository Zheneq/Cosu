// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

USTRUCT()
struct FInventoryItemStat
{
	GENERATED_BODY()

	UPROPERTY()
		FName StatName;

	UPROPERTY()
		float Value;
};

/**
 * 
 */
UCLASS()
class COSU_API AInventoryItem : public AActor
{
	GENERATED_BODY()

private:
	// If ItemClass is not specified, this one will be used
	const static TSubclassOf<AActor> DefaultItemClass;

public:
	AInventoryItem();

	// Inventory the item is currently in
	UPROPERTY(BlueprintReadOnly, Category = "Cosu Inventory")
		class UInventoryComponent* Inventory;

	// Basic name of the item
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Cosu Inventory")
		FText DisplayName;

	// Picture to show in inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Cosu Inventory")
		UTexture2D* DisplayPicture;

	// True if item can be stackable
	// If Stats array is not empty, this will be ignored
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Cosu Inventory")
		bool bStackable;

	// Number of items in the stack
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Cosu Inventory")
		int32 Count;

	// Max number of items in a stack (0 if unlimited)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cosu Inventory")
		int32 MaxCount;

	// Unique stats of the item
	// If the array is not empty, the item is unstackable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Cosu Inventory")
		TArray<FInventoryItemStat> Stats;

	// If true, player will be unable move it out of any inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Cosu Inventory")
		bool bNotDroppable;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cosu Inventory")
		FText GetDisplayName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cosu Inventory")
		bool IsStackable() const;
};
