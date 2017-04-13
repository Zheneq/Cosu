// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Private/BaseClasses/CosuCharacterBase.h"
#include "ToolBase.generated.h"

UCLASS()
class AToolBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AToolBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "Cosu")
		ACosuCharacterBase *Holder;

	UFUNCTION(BlueprintCallable, Category = "Cosu Inventory")
		void Use();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Use"), Category = "Cosu")
		void Use_BP();
	
};
