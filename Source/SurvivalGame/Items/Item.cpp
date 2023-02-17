// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"


#define LOCTEXT_NAMESPACE "Item"

void UItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// this is telling the quantity value to be replicated
	// telling server to manage this item, when server changes the quantity, it's then sent to the network
	DOREPLIFETIME(UItem, Quantity);
}

// this is how unreal checks if we want our uobject to be networked, because typically they aren't
bool UItem::IsSupportedForNetworking() const
{
	return true;
}

#if WITH_EDITOR
void UItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UItem, Quantity))
	{
		Quantity = FMath::Clamp(Quantity, 1, bStackable ? MaxStackSize : 1);
	
	}
}
#endif

UItem::UItem()
{
	//by default, any item will be named 'item' 
	//we define ftext usign the loctext macro, loctext wants the key (itemname_ and then the text we want to display (item)  
	//in unreal engine, you can now go into localization dashboard and map the key to some translation, to set up translations within unreal game
	ItemDisplayName = LOCTEXT("ItemName", "Item");
	UseActionText = LOCTEXT("ItemUseActionText", "Use");
	Weight = 0.f; 
	bStackable = true;
	Quantity = 1;
	MaxStackSize = 2; 
	RepKey = 0; 
}

void UItem::OnRep_Quantity()
{
	OnItemModified.Broadcast();
}

void UItem::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		//clamp will set quantity to a max of the stackable amount, if not stackable then it will be set to 1
		Quantity = FMath::Clamp(NewQuantity, 0, bStackable ? MaxStackSize : 1);
		MarkDirtyForReplication();
	}
}

bool UItem::ShouldShowInInventory() const
{
	return true;
}

void UItem::Use(ASurivivalCharacter* Character)
{
}

void UItem::AddedToInventory(UInventoryComponent* Inventory)
{
}

void UItem::MarkDirtyForReplication()
{
}

#undef LOCTEXT_NAMESPACE