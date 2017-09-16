// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "CosuPlayerControllerBase.h"
#include "CosuCharacterBase.h"


// Sets default values
ACosuCharacterBase::ACosuCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ThirdPersonCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonCameraBoom"));
	if (ThirdPersonCameraBoom)
	{
		ThirdPersonCameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
		ThirdPersonCameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
		ThirdPersonCameraBoom->bUsePawnControlRotation = true;
	}

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	if (ThirdPersonCamera)
	{
		ThirdPersonCamera->AttachToComponent(ThirdPersonCameraBoom, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
	}

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	if (FirstPersonCamera)
	{
		FirstPersonCamera->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
		FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
		FirstPersonCamera->bUsePawnControlRotation = true;
	}

	FirstPersonMesh = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	if (FirstPersonMesh)
	{
		FirstPersonMesh->AttachToComponent(FirstPersonCamera, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
		FirstPersonMesh->AlwaysLoadOnClient = true;
		FirstPersonMesh->AlwaysLoadOnServer = true;
		FirstPersonMesh->bOwnerNoSee = false;
		FirstPersonMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
		FirstPersonMesh->bCastDynamicShadow = true;
		FirstPersonMesh->bAffectDynamicIndirectLighting = true;
		FirstPersonMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		FirstPersonMesh->SetupAttachment(RootComponent);
		FirstPersonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FirstPersonMesh->bGenerateOverlapEvents = false;
		FirstPersonMesh->SetCanEverAffectNavigation(false);
	}

	CameraMaxShift = FVector(200.f, 50.f, 10.f);
	CameraMinDistancePercent = .3f;
	CameraZoomStepCount = 5;
	CameraZoomSpeed = 10.f;

	DesiredCameraAlpha = 0.f;
	CurrentCameraAlpha = 0.f;
	CameraBoomBaseHeight = ThirdPersonCameraBoom->RelativeLocation.Z;
	CameraStep = 1.f / CameraZoomStepCount;
	bTickCameraAlpha = true;
	bIsInThirdPersonView = false;

	InteractionDistance = 150.f;
}

// Called when the game starts or when spawned
void ACosuCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (CameraZoomStepCount <= 0) CameraZoomStepCount = 1;
	CameraStep = 1.f / CameraZoomStepCount;
	CameraBoomBaseHeight = ThirdPersonCameraBoom->RelativeLocation.Z;

	UpdateCameraView(true);
}

// Called every frame
void ACosuCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraPosition(DeltaTime);
}

// Called to bind functionality to input
void ACosuCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACosuCharacterBase::CameraZoom(bool bForward)
{
	if (bForward)
	{
		DesiredCameraAlpha = FMath::Clamp(DesiredCameraAlpha - CameraStep, 0.f, 1.f);
		if (DesiredCameraAlpha < CameraMinDistancePercent)
		{
			DesiredCameraAlpha = 0.f;
		}
	}
	else
	{
		DesiredCameraAlpha = FMath::Clamp(DesiredCameraAlpha + CameraStep, CameraMinDistancePercent, 1.f);
		UpdateCameraView();
	}

	bTickCameraAlpha = true;
}

void ACosuCharacterBase::UpdateCameraPosition(float DeltaTime)
{
	if (!ThirdPersonCameraBoom) return;

	if (bTickCameraAlpha)
	{
		CurrentCameraAlpha = FMath::FInterpConstantTo(CurrentCameraAlpha, DesiredCameraAlpha, DeltaTime, CameraZoomSpeed);
		ThirdPersonCameraBoom->TargetArmLength = CurrentCameraAlpha * CameraMaxShift.X;

		if (FMath::IsNearlyEqual(CurrentCameraAlpha, DesiredCameraAlpha))
		{
			UpdateCameraView();
			bTickCameraAlpha = false;
		}
	}

	FVector l = GetActorLocation();
	const float angle = -FMath::DegreesToRadians(ThirdPersonCamera->GetComponentRotation().Yaw);
	l.X += FMath::Sin(angle) * CameraMaxShift.Y * FMath::Sqrt(CurrentCameraAlpha);
	l.Y += FMath::Cos(angle) * CameraMaxShift.Y * FMath::Sqrt(CurrentCameraAlpha);
	l.Z += CameraBoomBaseHeight + CameraMaxShift.Z * FMath::Sqrt(CurrentCameraAlpha);

	ThirdPersonCameraBoom->SetWorldLocation(l);
}

void ACosuCharacterBase::UpdateCameraView(bool bInitial)
{
	auto ThirdPersonMesh = GetMesh();
	if (!ThirdPersonMesh || !FirstPersonMesh || !ThirdPersonCamera || !FirstPersonCamera) return;

	if ((bInitial || bIsInThirdPersonView) && FMath::IsNearlyZero(CurrentCameraAlpha))
	{
		ThirdPersonMesh->SetOwnerNoSee(true);
		FirstPersonMesh->SetOwnerNoSee(false);
		ThirdPersonCamera->Deactivate();
		FirstPersonCamera->Activate();
		bUseControllerRotationYaw = true;

		bIsInThirdPersonView = false;
		OnCameraSwitched();
		// UE_LOG(LogTemp, Log, TEXT("ACosuCharacterBase::UpdateCameraView: 1P view"));
	}
	else if (bInitial || (!bIsInThirdPersonView && !FMath::IsNearlyZero(DesiredCameraAlpha)))
	{
		FirstPersonMesh->SetOwnerNoSee(true);
		ThirdPersonMesh->SetOwnerNoSee(false);
		FirstPersonCamera->Deactivate();
		ThirdPersonCamera->Activate();
		bUseControllerRotationYaw = false;

		bIsInThirdPersonView = true;
		OnCameraSwitched();
		// UE_LOG(LogTemp, Log, TEXT("ACosuCharacterBase::UpdateCameraView: 3P view"));
	}
}

bool ACosuCharacterBase::GetAimedActor(FHitResult& outHit)
{
	auto World = GetWorld();
	auto PC = Cast<ACosuPlayerControllerBase>(GetController());
	auto Capsule = GetCapsuleComponent();
	if (!World || !PC || !Capsule)
	{
		return false;
	}

	bool res = PC->TraceView(outHit, InteractionDistance * 5.f, FCollisionObjectQueryParams::InitType::AllDynamicObjects);

	if (res)
	{
		FVector Vector = outHit.Location - GetActorLocation();
		if (FMath::Abs(Vector.Z) < Capsule->GetScaledCapsuleHalfHeight()) Vector.Z = 0.f;
		res = Vector.Size() <= InteractionDistance;

		DrawDebugLine(World, outHit.Location, GetActorLocation(), res ? FColor::Green : FColor::Red, false, .1f);
	}

	// GEngine->AddOnScreenDebugMessage(INDEX_NONE, .1f, FColor::Black, FString::Printf(TEXT("Trace: %s."), res ? *outHit.Actor->GetName() : TEXT("None")));

	return res;
}
