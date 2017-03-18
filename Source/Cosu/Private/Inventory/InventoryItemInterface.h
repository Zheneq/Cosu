#pragma once

#include "InventoryItem.h"
#include "InventoryItemInterface.generated.h"

UINTERFACE(BlueprintType)
class COSU_API UInventoryItemInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class COSU_API IInventoryItemInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cosu Inventory")
		bool ApplyStats(const TArray<FInventoryItemStat>& InStats);
};