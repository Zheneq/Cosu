// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "Net/UnrealNetwork.h"
#include "InventoryItem.h"
#include "InventoryDroppedItem.h"
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

	const auto& Item = Items[Index];

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

void UInventoryComponent::AddItem(AInventoryItem* NewItem)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, FString::Printf(TEXT("%s added to some inventory."), *NewItem->GetDisplayName().ToString()));

	if (NewItem->IsStackable())
	{
		const auto ItemClass = NewItem->GetClass();
		for (const auto& Item : Items)
		{
			if (Item->GetClass() == ItemClass && Item->IsStackable())
			{
				Item->Count += NewItem->Count;
				NewItem->Destroy();
				OnInventoryUpdated.Broadcast();
				return;
			}
		}
	}

	Items.Add(NewItem);
	NewItem->Inventory = this;
	OnInventoryUpdated.Broadcast();
}

bool UInventoryComponent::HasItem(TSubclassOf<AInventoryItem> ItemClass, int32 Count) const
{
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

int32 UInventoryComponent::RemoveItemByClass(TSubclassOf<AInventoryItem> ItemClass, int32 Count, bool bDestroy)
{
	TArray<AInventoryItem*> ItemsToRemove;
	int32 DeletedCount = 0;

	for (auto& Item : Items)
	{
		if (Item->GetClass()->IsChildOf(ItemClass))
		{
			if (Count - DeletedCount < Item->Count)
			{
				Item->Count -= Count - DeletedCount;
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
	if (!Items.IsValidIndex(Index))
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
		Items[Index]->Count -= Count;
	}
	OnInventoryUpdated.Broadcast();
	return true;
}

