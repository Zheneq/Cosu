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
