// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "Widgets/InteractionWidget.h"
#include "Player/SurvivalCharacter.h"

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
	InteractableNameText = NewNameText;
	RefreshWidget();
}

void UInteractionComponent::SetInteractableActionText(const FText& NewActionText)
{
	InteractableActionText = NewActionText;
	RefreshWidget();
}

void UInteractionComponent::Deactivate()
{
	//call super function because we inherit from widget component which has its own deactivate to so we have to call that as well
	Super::Deactivate();

	//grab all interactors
	for (int32 i = Interactors.Num() - 1; i >= 0; --i)
	{
		if (ASurvivalCharacter* Interactor = Interactors[i])
		{
			//tell them to stop focusing and stop interacting 
			EndFocus(Interactor);
			EndInteract(Interactor);
		}
	}
	//clear all interactors
	Interactors.Empty();
}

bool UInteractionComponent::CanInteract(ASurvivalCharacter* Character) const
{
	//if we dont allow multiple interactors and there is more than one interactor 
	const bool bPlayerAlreadyInteracting = !bAllowMultipleInteractors && Interactors.Num() >= 1;
	//return true if not interacting, and is active, owner is not nullptr, character is not nullptr
	//return false if interacting, not active, owner is nullptr, or character is nullptr
	return !bPlayerAlreadyInteracting && IsActive() && GetOwner() != nullptr && Character != nullptr;
}

void UInteractionComponent::RefreshWidget()
{
	//make sure interaction card is not hidden and that we are not the server as server has no UI
	if (!bHiddenInGame && GetOwner()->GetNetMode() != NM_DedicatedServer)
	{
		//make sure the widget is initialized, and that we are displaying the right values (these may have changed)
		if (UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
		{
			InteractionWidget->UpdateInteractionWidget(this);
		}
	}
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

	RefreshWidget();
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
