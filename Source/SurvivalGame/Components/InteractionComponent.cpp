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

void UInteractionComponent::SetInteractableNameText(const FText& NewNameText)
{
}

void UInteractionComponent::SetInteractableActionText(const FText& NewActionText)
{
}

void UInteractionComponent::Deactivate()
{
}

bool UInteractionComponent::CanInteract(ASurvivalCharacter* Character) const
{
	return false;
}

void UInteractionComponent::RefreshWidget()
{
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
	//broadcasting delegate -- allows interaction to do something custom 
	OnEndFocus.Broadcast(Character);
	
	//hide the UI
	SetHiddenInGame(true);

	//if you are not the server, not doing on server because server doesnt have anyone playing the game 
	if (!GetOwner()->HasAuthority())
	{
		//grab any visual primtive components
		for (auto& VisualComp : GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass()))
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				//disable outline around interactable object
				Prim->SetRenderCustomDepth(false);
			}
		}
	}
}

void UInteractionComponent::BeginInteract(ASurvivalCharacter* Character)
{
	if (CanInteract(Character))
	{
		Interactors.AddUnique(Character);
		OnBeginInteract.Broadcast(Character);
	}
}

void UInteractionComponent::EndInteract(ASurvivalCharacter* Character)
{
	Interactors.RemoveSingle(Character);
	OnEndInteract.Broadcast(Character);
}

void UInteractionComponent::Interact(ASurvivalCharacter* Character)
{
	if (CanInteract(Character))
	{
		OnInteract.Broadcast(Character);
	}
}

float UInteractionComponent::GetInteractionPercentage()
{
	//check that there are any interactors in the array
	if (Interactors.IsValidIndex(0))
	{
		//get first interactor, which is us -- only the server keeps track of everyone
		// on our machine, its just us in the interactors array 
		if (ASurvivalCharacter* Interactor = Interactors[0])
		{
			//check that we are interacting 
			if (Interactor && Interactor->IsInteracting())
			{
				return 1.f - FMath::Abs(Interactor->GetRemainingInteractTime() / InteractionTime);
			}
		}
	}
	return 0.f;
}
