// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "CosuCharacterBase.generated.h"

UCLASS()
class ACosuCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACosuCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* ThirdPersonCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* FirstPersonMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	// CAMERA

	// X is max distance behind the character, Y is to the right, Z is upwards
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		FVector CameraMaxShift;

	// Below this percent will switch to 1P camera (to avoid 3P camera going into character model)
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		float CameraMinDistancePercent;

	// Number of steps between 1P camera and 3P camera being the farthest away
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		int32 CameraZoomStepCount;

	// How fast camera will move when zooming
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		float CameraZoomSpeed;

	UFUNCTION(BlueprintCallable, Category = "Camera")
		void CameraZoom(bool bForward);

	UFUNCTION(BlueprintCallable, Category = "Camera")
		FORCEINLINE bool IsInThirdPersonView() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Camera")
		void OnCameraSwitched();

	UFUNCTION(BlueprintCallable, Category = "Cosu")
		bool GetAimedActor(FHitResult& outHit);

	UPROPERTY(EditDefaultsOnly, Category = "Cosu")
		float InteractionDistance;

private:
	float DesiredCameraAlpha;
	float CurrentCameraAlpha;
	float CameraBoomBaseHeight;
	float CameraStep;

	bool bTickCameraAlpha;
	bool bIsInThirdPersonView;

	void UpdateCameraPosition(float DeltaTime);
	void UpdateCameraView(bool bInitial = false);
};

bool ACosuCharacterBase::IsInThirdPersonView() const
{
	return bIsInThirdPersonView;
}
