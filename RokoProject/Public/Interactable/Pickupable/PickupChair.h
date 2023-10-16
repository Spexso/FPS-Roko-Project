// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/Pickupable/pickupBaseObject.h"
#include "PickupChair.generated.h"

/**
 * 
 */
UCLASS()
class ROKOPROJECT_API APickupChair : public ApickupBaseObject
{
	GENERATED_BODY()
	
public: 
	
	APickupChair();

	virtual void Use_Implementation() override;
};
