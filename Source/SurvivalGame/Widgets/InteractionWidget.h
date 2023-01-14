// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	//c++ callable function that will be called after we change interaction card in some way
	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void UpdateInteractionWidget(class UInteractionComponent* Interaction Component);

	//doing some blueprint implementation to update the card
	UFUNCTION(BlueprintImplementableEvent)
		void OnUpdateInteractionWidget();

	//the interaction component the card is using 
	UFUNCTION(BlueprintReadOnly, Category = "Interaction", meta = (ExposeonSpawn))
		class UInteractionComponent* OwningInteractionComponent; 
	
};
