// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Pickupable/pickupBaseObject.h"
#include "RokoProject/Public/Player/RokoProjectCharacter.h"
#include "Kismet/GameplayStatics.h"

ApickupBaseObject::ApickupBaseObject()
{
	// Set up mesh of item, set item name , Declare help text and value
	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	InteractableMesh->SetSimulatePhysics(true);

	ItemName = FString("ExampleItemName");
	InteractableHelpText = FString("Press H to pick up the Item");
	value = 0;
	IsGun = false;
}

void ApickupBaseObject::BeginPlay()
{
	InteractableHelpText = FString::Printf(TEXT("s%: Press H to pick up the Item"), *ItemName);


}

// Base Interact function for all intractable items
void ApickupBaseObject::Interact_Implementation()
{
	// Get reference of player character
	ARokoProjectCharacter* playerRef = Cast<ARokoProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));


	if (playerRef->addItemToInventory(this)) {

		onPickedUp();
		UE_LOG(LogTemp, Warning, TEXT("%s added to inventory"), *ItemName);
	}
	
}


void ApickupBaseObject::Use_Implementation()
{
	GLog->Log("PickupBase: Use() // Not be able to see here!");
}

void ApickupBaseObject::onPickedUp()
{
	// Hide the item from level , stop physics of it, close collision
	InteractableMesh->SetVisibility(false);
	InteractableMesh->SetSimulatePhysics(false);
	InteractableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UE_LOG(LogTemp, Warning, TEXT("Hide the Mesh"));
}

bool ApickupBaseObject::getIsGun()
{
	return IsGun;
}

void ApickupBaseObject::setIsGun(bool kind)
{
	// Set kind of collectible
	IsGun = kind;
}
