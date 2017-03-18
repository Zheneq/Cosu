// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "InventoryDroppedItem.generated.h"

class UInventoryComponent;

UCLASS()
class AInventoryDroppedItem : public AActor
{
	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
		UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* StaticMeshComponent;

public:	
	// Sets default values for this actor's properties
	AInventoryDroppedItem();

	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
