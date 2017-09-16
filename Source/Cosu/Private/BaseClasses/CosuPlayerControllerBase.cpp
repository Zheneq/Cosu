// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "CosuCharacterBase.h"
#include "CosuPlayerControllerBase.h"

ACosuPlayerControllerBase::ACosuPlayerControllerBase()
{
}

bool ACosuPlayerControllerBase::GetAimedActor(FHitResult& outHit)
{
	auto C = Cast<ACosuCharacterBase>(GetPawn());
	return C && C->GetAimedActor(outHit);
}

bool ACosuPlayerControllerBase::TraceView(FHitResult& outHit, float TraceDistance, enum FCollisionObjectQueryParams::InitType QueryType)
{
	auto World = GetWorld();
	if (!World || !PlayerCameraManager)
	{
		return false;
	}

	FVector Start;
	FRotator Rot;
	PlayerCameraManager->GetCameraViewPoint(Start, Rot);
	const FVector End = Start + Rot.Vector() * TraceDistance;

	FCollisionQueryParams TraceParams(NAME_None, false, GetPawn());

	bool res = World->LineTraceSingleByObjectType(outHit, Start, End, FCollisionObjectQueryParams(QueryType), TraceParams);


	DrawDebugLine(World, Start, End, FColor::Red, false, .1f);
	if (res)
		DrawDebugPoint(World, outHit.Location, 25.f, FColor::Green, false, .1f);

	return res;
}
