// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"

UInteractionComponent::UInteractionComponent()
{
	SetComponentTickEnabled(false);

	InteractionTime = 0.f; 
	InteractionDistance = 200.f;
	InteractableNameText = FText::FromString("Interactable Object");
	InteractableActionText = FText::FromString("Interact");
	bAllowMultipleInteractors = true; 

	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(400, 100);
	bDrawAtDesiredSize = true; 

	SetActive(true);
	SetHiddenInGame(true);

}

