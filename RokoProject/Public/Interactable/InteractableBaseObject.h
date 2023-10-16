// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBaseObject.generated.h"

UCLASS()
class ROKOPROJECT_API AInteractableBaseObject : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AInteractableBaseObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// All interactable objects has mesh
	UPROPERTY(EditAnywhere, Category = "Interactable Properties")
	class UStaticMeshComponent* InteractableMesh;

	// All interactable objects has help text attached to it 
	UPROPERTY(EditAnywhere, Category = "Interactable Properties")
	FString InteractableHelpText;

public:	

	// Get Interactable help text 
	FString getHelpText();

	// Function to interact with the object using Blueprint
	// Not used
	UFUNCTION(BlueprintNativeEvent)
	void Interact();

	// Function to inherit for all interactible objects
	virtual void Interact_Implementation();
};
