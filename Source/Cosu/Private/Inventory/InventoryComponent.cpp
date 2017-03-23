// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "Net/UnrealNetwork.h"
#include "InventoryItem.h"
#include "InventoryDroppedItem.h"
#include "InventoryItemInterface.h"
#include "InventoryComponent.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;

	// ...
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Items);

}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::DropItem(int32 Index)
{
	if (!Items.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::DropItem: Attempted to drop an item that does not exist."));
		return;
	}

	const auto Item = Items[Index];

	auto OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory has no owner."));
		return;
	}

	if (Item->bNotDroppable || Item->bHidden)
	{
		UE_LOG(LogTemp, Log, TEXT("Attempted to drop an undroppable item."));
		return;
	}

	FTransform SpawnTransform = OwnerActor->GetActorTransform();
	SpawnTransform.SetLocation(SpawnTransform.GetLocation() + SpawnTransform.GetRotation().RotateVector(FVector(50.f, 0.f, 0.f)));

	auto World = GetWorld();
	if (World)
	{
		auto DroppedContainer = World->SpawnActor<AInventoryDroppedItem>(AInventoryDroppedItem::StaticClass(), SpawnTransform);
		RemoveItemAt(Index);
		DroppedContainer->GetInventoryComponent()->AddItem(Item);
	}
}

void UInventoryComponent::AddItemLow(AInventoryItem* NewItem)
{
	if (!IsValid(NewItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItemLow: Attempted to add an invalid item."));
		return;
	}

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, FString::Printf(TEXT("%s added to some inventory."), *NewItem->GetDisplayName().ToString()));

	Items.Add(NewItem);
	NewItem->Inventory = this;
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::AddToItemCountLow(AInventoryItem* Item, int32 Count)
{
	if (!Item || !Count || Items.Find(Item) < 0 || !Item->IsStackable())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddToItemCountLow: Incorrect parameter."));
		return;
	}

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, FString::Printf(TEXT("%s added to some inventory."), *Item->GetDisplayName().ToString()));

	Item->Count += Count;
	NormalizeLow(Item->GetClass());
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::NormalizeLow(TSubclassOf<AInventoryItem> Class)
{
	auto Filter = Items.FilterByPredicate([&Class](const AInventoryItem* Item) {
		return Item->IsStackable() && Item->GetClass() == Class;
	});

	if (!Filter.Num()) return;

	int32 totalCount = 0;
	for (const auto& Item : Filter) totalCount += Item->Count;

	for (const auto& Item : Filter)
	{
		// if we're out of items, remove the uneeded stacks
		if (totalCount <= 0)
		{
			Items.Remove(Item);
			continue;
		}

		// add as much to a stack as we can
		if (!Item->MaxCount)
		{
			Item->Count = totalCount;
		}
		else
		{
			if (totalCount > Item->MaxCount)
			{
				Item->Count = Item->MaxCount;
			}
			else
			{
				Item->Count = totalCount;
			}
		}
		totalCount -= Item->Count;
	}
	const auto CDO = Cast<AInventoryItem>(Class->GetDefaultObject());
	while (totalCount > 0)
	{
		const auto NewItem = SpawnNewItemLow(Class, totalCount > CDO->MaxCount ? CDO->MaxCount: totalCount);
		totalCount -= NewItem->Count;
	}
}

AInventoryItem* UInventoryComponent::SpawnNewItemLow(TSubclassOf<AInventoryItem> NewItemClass, int32 Count, const TArray<FInventoryItemStat>* Stats)
{
	if (Count <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SpawnNewItemLow: Attempted to spawn zero items or less."));
		return nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::SpawnNewItemLow: Spawning new item."));
	auto World = GetWorld();
	if (World)
	{
		AInventoryItem* NewItem = World->SpawnActor<AInventoryItem>(NewItemClass);
		NewItem->Count = Count;
		if (Stats) NewItem->Stats = *Stats;
		AddItemLow(NewItem);
		return NewItem;
	}
	return nullptr;
}

void UInventoryComponent::AddItem(AInventoryItem* NewItem)
{
	if (!IsValid(NewItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItem: Attempted to add an invalid item."));
		return;
	}

	if (NewItem->IsStackable())
	{
		const auto ItemClass = NewItem->GetClass();
		for (const auto& Item : Items)
		{
			if (Item->GetClass() == ItemClass && Item->IsStackable())
			{
				AddToItemCountLow(Item, NewItem->Count);
				NewItem->Destroy();
				return;
			}
		}
	}

	AddItemLow(NewItem);
}

void UInventoryComponent::AddItemByClass(TSubclassOf<AInventoryItem> NewItemClass, int32 Count)
{
	if (Count <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItemByClass: Attempted to add zero items or less."));
		return;
	}
	// TODO: Check if NewItemClass is stackable by default?
	const auto Item = FindItem(NewItemClass, true);
	if (Item)
	{
		AddToItemCountLow(Item, Count);
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AddItemByClass: Added an item without spawning."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AddItemByClass: Spawning new item."));
		SpawnNewItemLow(NewItemClass, Count);
	}
}

void UInventoryComponent::AddItemByClassWStats(TSubclassOf<AInventoryItem> NewItemClass, const TArray<FInventoryItemStat>& Stats, int32 Count)
{
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AddItemByClassWStats: Spawning new item."));
	SpawnNewItemLow(NewItemClass, Count, &Stats);
}

bool UInventoryComponent::HasItem(TSubclassOf<AInventoryItem> ItemClass, int32 Count) const
{
	if (Count <= 0) return true;

	for (const auto& Item : Items)
	{
		if (Item->GetClass()->IsChildOf(ItemClass))
		{
			Count -= Item->Count;
			if (Count <= 0) return true;
		}
	}
	return false;
}

AInventoryItem* UInventoryComponent::FindItem(TSubclassOf<AInventoryItem> ItemClass, bool bOnlyStackable) const
{
	for (const auto& Item : Items)
	{
		if (Item->GetClass()->IsChildOf(ItemClass) && (!bOnlyStackable || Item->IsStackable()))
		{
			return Item;
		}
	}
	return nullptr;
}

int32 UInventoryComponent::RemoveItemByClass(TSubclassOf<AInventoryItem> ItemClass, int32 Count, bool bDestroy)
{
	if (Count <= 0) return 0;

	TArray<AInventoryItem*> ItemsToRemove;
	int32 DeletedCount = 0;

	for (auto& Item : Items)
	{
		if (Item->GetClass()->IsChildOf(ItemClass))
		{
			if (Count - DeletedCount < Item->Count)
			{
				AddToItemCountLow(Item, DeletedCount - Count);
				DeletedCount = Count;
				break;
			}
			else
			{
				DeletedCount += Item->Count;
				ItemsToRemove.Add(Item);
				if (DeletedCount == Count) break;
			}
		}
	}

	for (auto& Item : ItemsToRemove)
	{
		RemoveItem(Item, bDestroy);
	}

	if (ItemsToRemove.Num())
	{
		OnInventoryUpdated.Broadcast();
	}


	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, FString::Printf(TEXT("%d items removed from some inventory."), DeletedCount));
	return DeletedCount;
}

bool UInventoryComponent::RemoveItem(AInventoryItem* Item, int32 Count, bool bDestroy)
{
	return RemoveItemAt(Items.Find(Item), Count, bDestroy);
}

bool UInventoryComponent::RemoveItemAt(int32 Index, int32 Count, bool bDestroy)
{
	if (!Items.IsValidIndex(Index) || Count < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveItemAt: Attempted to remove item that does not exist."));
		return false;
	}

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, FString::Printf(TEXT("%s removed from some inventory."), *Items[Index]->GetDisplayName().ToString()));

	if (!Count || Count >= Items[Index]->Count)
	{
		if (bDestroy)
		{
			Items[Index]->Destroy();
		}
		else
		{
			Items[Index]->Inventory = nullptr;
		}
		Items.RemoveAt(Index);
	}
	else
	{
		AddToItemCountLow(Items[Index], -Count);
	}
	OnInventoryUpdated.Broadcast();
	return true;
}

