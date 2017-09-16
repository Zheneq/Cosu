// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CosuFL.generated.h"

/**
 * 
 */
UCLASS()
class UCosuFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	template<class EEnum>
	static inline bool HasFlag(int32 Data, EEnum Flag);

	template<class EEnum>
	static inline bool HasOnlyFlag(int32 Data, EEnum Flag);
	
	static void EnableActor(AActor* Actor, bool bNewEnabled = true);
};

template<class EEnum>
inline bool UCosuFL::HasFlag(int32 Data, EEnum Flag)
{
	return (Data & (1 << (int32)Flag)) != 0;
}

template<class EEnum>
inline bool UCosuFL::HasOnlyFlag(int32 Data, EEnum Flag)
{
	return Data == (1 << (int32)Flag);
}
