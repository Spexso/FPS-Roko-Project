// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/InteractableBaseObject.h"
#include "pickupBaseObject.generated.h"

/**
 * 
 */
UCLASS()
class ROKOPROJECT_API ApickupBaseObject : public AInteractableBaseObject
{
	GENERATED_BODY()

private:

	// To check if picked up object is gun or not
	bool IsGun;
	
public:

	// Constructor
	ApickupBaseObject();

	virtual void BeginPlay() override;
	
	virtual void Interact_Implementation() override;

	UFUNCTION(BlueprintNativeEvent)
	void Use();

	// Use in inventory 
	virtual void Use_Implementation();

	// Thumbnail of item, Set in editor
	UPROPERTY(EditAnywhere, Category = "Pickup Properties")
	UTexture2D* ItemThumbnail;

	// Name of item, Set in editor
	UPROPERTY(EditAnywhere, Category = "Pickup Properties")
	FString ItemName;

	// Value of item, Set in editor
	// Not used
	UPROPERTY(EditAnywhere, Category = "Pickup Properties")
	int32 value;

	// Clean the item from level and executes necessary functions after item picked up
	void onPickedUp();

	// Call for check if collectible object is gun or not 
	bool getIsGun();

	// Call to set if collectible is gun or not
	void setIsGun(bool kind);
};