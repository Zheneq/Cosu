#pragma once

#include "InteractInterface.generated.h"

UINTERFACE(BlueprintType)
class COSU_API UInteractInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class COSU_API IInteractInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cosu")
		bool RequestInteraction(float& outDuration, ACharacter* Character);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cosu")
		void InteractionSuccess(ACharacter* Character);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cosu")
		void InteractionFailure(ACharacter* Character);

};