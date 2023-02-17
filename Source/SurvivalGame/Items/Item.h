// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemModified);

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	IR_Common UMETA(DisplayName = "Common"),
	IR_Uncommon UMETA(DisplayName = "Uncommon"),
	IR_Rare UMETA(DisplayName = "Rare"),
	IR_VeryRare UMETA(DisplayName = "Very Rare"),
	IR_Legendary UMETA(DisplayName = "Legendary")
};


/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class SURVIVALGAME_API UItem : public UObject
{
	GENERATED_BODY()

protected: 
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override; 

//dont want following function packaged with game, if WITH_EDITOR means it is only packaged/used in editor environment
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override; 
#endif	
public:
	UItem();

	// Mesh to display for this item pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	class UStaticMesh* PickupMesh;

	// Thumbnail for the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UTexture2D* Thumbnail; 

	// The display name for this item in the inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText ItemDisplayName;

	// Optional description for item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
	FText ItemDescription; 

	// The text for using hte item (equip, eat, etc)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText UseActionText; 

	// rarity of item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	EItemRarity Rarity; 

	// weight of item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (ClampMin = 0.0))
	float Weight; 

	// whether or not item can be stacked
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	bool bStackable;

	// the maximum size that a stack of items can be
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (ClamPin = 2, EditCondition = bStackable))
	int32 MaxStackSize; 

	// the tooltip in the inenvtory for this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	TSubclassOf<class UItemTooltip> ItemTooltip;

	// in quantity, this is the function that is called (ReplicateUsing) whe na quantity value gets replicated to a client
	UFUNCTION()
	void OnRep_Quantity();

	// the inventory that owns this item
	UPROPERTY()
	class UInventoryComponent* OwningInventory; 

	// used to efficienty replicate inventory items
	// because we are using uobject, we need a repkey (wouldnt need this for a normal actor)
	// repkey is a key that we change whenever we change something about the item
	// when quantity is updated we change the repkey, and thats how the server knows it needs to send an update to client
	UPROPERTY()
	int32 RepKey;

	UPROPERTY(BlueprintAssignable)
	FOnItemModified OnItemModified;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetQuantity(const int32 NewQuantity);

	// helper function that returns the weight of the sOtack 
	UFUNCTION(BlueprintCallable, Category = "Intem")
	FORCEINLINE float GetStackWeight() const { return Quantity * Weight; };

	// certain items, such as clothing, shouldn't show up in the inventory after you equip them 
	// this function gives us a way to implement that behavior 
	UFUNCTION(BlueprintPure, Category = "Item")
	virtual bool ShouldShowInInventory() const;

	//every item is going to have a different use functionality -- food will heal, weapons will be wielded, etc
	//this is why use function is marked as virtual
	virtual void Use(class ASurivivalCharacter* Character);
	virtual void AddedToInventory(class UInventoryComponent* Inventory);

	// mark the object as needing replication, we must call this internally after modifying any replicated properties
	void MarkDirtyForReplication();

protected:
	// amount of item we currently have
	UPROPERTY(ReplicatedUsing = OnRep_Quantity, EditAnywhere, Category = "Item", meta = (UIMin = 1, EditCondition = bStackable))
	int32 Quantity;
};
