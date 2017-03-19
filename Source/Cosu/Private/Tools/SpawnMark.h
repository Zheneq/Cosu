// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnMark.generated.h"

UCLASS()
class ASpawnMark : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* StaticMeshComponent;

	
public:	
	// Sets default values for this actor's properties
	ASpawnMark();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UStaticMeshComponent* GetStaticMesh() const;
};

FORCEINLINE UStaticMeshComponent* ASpawnMark::GetStaticMesh() const { return StaticMeshComponent; }