// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/Pickupable/pickupBaseObject.h"
#include "AssaultRifle.generated.h"

/**
 * 
 */
UCLASS()
class ROKOPROJECT_API AAssaultRifle : public ApickupBaseObject
{
	GENERATED_BODY()
	
protected:
	
	// Skeletal Weapon Mesh, Set in Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assets")
	USkeletalMesh* Interactable_SK_Mesh;

public:
	
	// Constructor
	AAssaultRifle();

	// Use in inventory 
	virtual void Use_Implementation() override;
};
