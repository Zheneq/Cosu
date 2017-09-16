// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BuildableBase.generated.h"

UENUM(BlueprintType)
enum class EBuildingStageEnum : uint8
{
	B_Placing		UMETA(DisplayName = "Placing"),
	B_InProgress	UMETA(DisplayName = "In Progress"),
	B_Finished		UMETA(DisplayName = "Finished"),
};

UENUM(BlueprintType, meta = (Bitflags))
enum class EBuildingFoundationType : uint8
{
	F_Landscape		UMETA(DisplayName = "Landscape"),
	F_Floor			UMETA(DisplayName = "Floor"),
	F_Socket		UMETA(DisplayName = "Socket"),
};
ENUM_CLASS_FLAGS(EBuildingFoundationType);

UCLASS()
class ABuildableBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildableBase();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* Mesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// These bitflags describes what this object can be based on
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosu", meta = (Bitmask, BitmaskEnum = "EBuildingFoundationType"))
		uint8 CanBeFoundedOn;

	// Socket to place this object onto
	// Used only when CanBeFoundedOn has F_Socket as true
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosu")
		FString FoundationSocketTag;

	// If true, some other buildables can be placed upon this one
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosu")
		bool bCanBeFoundation;

	// This tag describes what this object can be a foundation for
	// Some objects may be based on objects with specific FoundationType (e.g. floor)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosu")
		EBuildingFoundationType FoundationType;

	// Whether collisions should be ignored while placing this object or not
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosu")
		bool bIgnoreCollisions;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Cosu")
		EBuildingStageEnum BuildingStage;

	UFUNCTION(BlueprintCallable, Category = "Cosu")
		bool Place();

	UFUNCTION(BlueprintCallable, Category = "Cosu")
		void UpdateState();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Cosu")
		bool IsValidLocation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Cosu")
		void UpdateMaterial();

	void SetCanBePlaced(bool bNewCanBePlaced);

	UStaticMeshComponent* GetMesh() const;

private:
	bool bCanBePlaced;
};

FORCEINLINE UStaticMeshComponent* ABuildableBase::GetMesh() const { return Mesh; }