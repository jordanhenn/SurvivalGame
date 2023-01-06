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

void UInteractionComponent::BeginFocus(ASurvivalCharacter* Character)
{
	//if not active, if we do not have an owner, or character is null -- just return, we dont want to do antthing 
	if (!IsActive() || !GetOwner() || !Character)
	{
		return;
	}

	//broadcasting delegate -- allows interaction to do something custom 
	OnBeginFocus.Broadcast(Character);

	//show the UI
	SetHiddenInGame(false);

	//if you are not the server, not doing on server because server doesnt have anyone playing the game 
	if (!GetOwner()->HasAuthority())
	{
		//grab any visual primtive components
		for (auto& VisualComp : GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass()))
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				//set render ucstom depth to to true, enables outline around interactable object
				Prim->SetRenderCustomDepth(true);
			}
		}
	}
}

void UInteractionComponent::EndFocus(ASurvivalCharacter* Character)
{
}

void UInteractionComponent::BeginInteract(ASurvivalCharacter* Character)
{
}

void UInteractionComponent::EndInteract(ASurvivalCharacter* Character)
{
}

void UInteractionComponent::Interact(ASurvivalCharacter* Character)
{
	OnInteract.Broadcast(Character);
}
