// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ToolBase.h"
#include "Private/BaseClasses/BuildableBase.h"
#include "ToolSpawner.generated.h"

/**
 * 
 */
UCLASS()
class AToolSpawner : public AToolBase
{
	GENERATED_BODY()

public:
	AToolSpawner();

	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	virtual void Use_Implementation() override;

	UPROPERTY(BlueprintReadOnly, Category = "Cosu")
		ABuildableBase *Building;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cosu")
		float SnapDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cosu")
		float MaxPlaceDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cosu")
		bool bSnapIsOn;

	UFUNCTION(BlueprintCallable, Category = "Cosu")
		void SetBuildable(ABuildableBase* NewBuildable);

	UFUNCTION(BlueprintCallable, Category = "Cosu")
		void Deactivate();
	
private:
	/**
	* Snap the buildable according to its settings.
	* @return true if successfully snapped.
	*/
	bool Snap(FTransform& OutNewTransform) const;

	bool CheckFoundation(AActor* Foundation);

	bool GetBestSocketTransformInRadiusByTag(FTransform& OutTransform, FVector Center, float Radius, const FString& Tag, bool bRequireFoundation = false) const;
	// TArray<FTransform> GetAllSocketTransformsInRadiusByTag(FVector Center, float Radius, const FString& Tag) const;
};