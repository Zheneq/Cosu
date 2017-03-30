// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "CosuPlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class ACosuPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
	
public:
	ACosuPlayerControllerBase();

	UFUNCTION(BlueprintCallable, Category = "Cosu")
		bool GetAimedActor(FHitResult& outHit);
	
	UPROPERTY(EditDefaultsOnly, Category = "Cosu")
		float InteractionDistance;
};
