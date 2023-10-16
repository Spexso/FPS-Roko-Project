// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Pickupable/PickupChair.h"

APickupChair::APickupChair() {

	// Chair object constructor
}

void APickupChair::Use_Implementation()
{

	UE_LOG(LogTemp, Warning, TEXT("Using chair object!"));

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("What did you expect to do with a chair dude ??"));
}
