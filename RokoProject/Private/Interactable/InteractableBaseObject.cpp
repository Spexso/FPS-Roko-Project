// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/InteractableBaseObject.h"

// Sets default values
AInteractableBaseObject::AInteractableBaseObject()
{
	InteractableHelpText = FString("Press H to interact with item");

// 	this->InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interactable Mesh"));
// 	this->InteractableMesh->AttachTo(GetRootComponent());

// 	this->Interactable_SK_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Interactable Skeletal Mesh"));
// 	this->Interactable_SK_Mesh->AttachTo(GetRootComponent());

// 
// 	static ConstructorHelpers::FObjectFinder<UStaticMesh>InteractableMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
// 	this->InteractableMesh->SetStaticMesh(InteractableMeshAsset.Object);
}

// Called when the game starts or when spawned
void AInteractableBaseObject::BeginPlay()
{
	Super::BeginPlay();
	
}

FString AInteractableBaseObject::getHelpText()
{
	return InteractableHelpText;
}

// Interact function that will be inherited
void AInteractableBaseObject::Interact_Implementation()
{	 
	GLog->Log("Interactable Base Object: Interact() // Not be able to see here!");
}

