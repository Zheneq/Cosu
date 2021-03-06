// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "InventoryItem.h"
#include "InventoryComponent.generated.h"

class AInventoryItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Cosu Inventory")
		TArray<AInventoryItem*> Items;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Cosu Inventory")
		virtual void DropItem(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Cosu Inventory")
		virtual void AddItem(AInventoryItem* NewItem);

	UFUNCTION(BlueprintCallable, Category = "Cosu Inventory")
		virtual void AddItemByClass(TSubclassOf<AInventoryItem> NewItemClass, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Cosu Inventory")
		virtual void AddItemByClassWStats(TSubclassOf<AInventoryItem> NewItemClass, const TArray<FInventoryItemStat>& Stats, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Cosu Inventory")
		virtual bool HasItem(TSubclassOf<AInventoryItem> ItemClass, int32 Count = 1) const;

	UFUNCTION(BlueprintCallable, Category = "Cosu Inventory")
		virtual AInventoryItem* FindItem(TSubclassOf<AInventoryItem> ItemClass, bool bOnlyStackable = false) const;

	UFUNCTION(BlueprintCallable, Category = "Cosu Inventory")
		virtual int32 RemoveItemByClass(TSubclassOf<AInventoryItem> ItemClass, int32 Count = 1, bool bDestroy = false);

	UFUNCTION(BlueprintCallable, Category = "Cosu Inventory")
		virtual bool RemoveItem(AInventoryItem* Item, int32 Count = 0, bool bDestroy = false);

	UFUNCTION(BlueprintCallable, Category = "Cosu Inventory")
		virtual bool RemoveItemAt(int32 Index, int32 Count = 0, bool bDestroy = false);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryUpdate);
	UPROPERTY(BlueprintAssignable, Category = "Cosu Inventory")
		FInventoryUpdate OnInventoryUpdated;
	
private:
	void AddItemLow(AInventoryItem* NewItem);

	// Adds as much as possible to the existing item, than adds a copy of the item if needed
	void AddToItemCountLow(AInventoryItem* Item, int32 count);


	// Spawns a new item and adds it into the inventory
	AInventoryItem* SpawnNewItemLow(TSubclassOf<AInventoryItem> NewItemClass, int32 Count, const TArray<FInventoryItemStat>* Stats = nullptr);

	// Compresses the stacks of given class (if stackable)
	// Fixes overfull ones
	// Does not call OnInventoryUpdated!
	void NormalizeLow(TSubclassOf<AInventoryItem> Class);

	// Checks if the item is valid
	bool CheckItemLow(AInventoryItem* Item);
};
