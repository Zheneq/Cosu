// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "CosuFL.h"

void UCosuFL::EnableActor(AActor* Actor, bool bNewEnabled)
{
	if (IsValid(Actor))
	{
		Actor->SetActorHiddenInGame(!bNewEnabled);
		Actor->SetActorEnableCollision(bNewEnabled);
		Actor->SetActorTickEnabled(bNewEnabled);
	}
}


