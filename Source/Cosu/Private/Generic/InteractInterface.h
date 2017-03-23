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
		bool BeginInteraction(bool& outBLatent, ACharacter* Character);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cosu")
		void EndInteraction(ACharacter* Character);

};