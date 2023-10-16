// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Pickupable/Medkit.h"
#include "Player/RokoProjectCharacter.h"
#include "Kismet/GameplayStatics.h"

AMedkit::AMedkit()
{
	// Set if is a gun 
	setIsGun(false);
	
	// Set the healing sound FX
	MedkitSound = CreateDefaultSubobject<UAudioComponent>(TEXT("HealingSound"));
	MedkitSound->bAutoActivate = false;
}

void AMedkit::Use_Implementation()
{
	// Get reference of player character
	ARokoProjectCharacter* playerRef = Cast<ARokoProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (playerRef != nullptr) {

		// Change the already set up FP_GUN's skeletal mesh as this Gun
		playerRef->heal(HealAmount);

		// Play the Healing Sound FX
		if (MedkitSoundFX->IsValidLowLevel()) {

			MedkitSound->SetSound(MedkitSoundFX);
			MedkitSound->Play();
		}

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Medkit Used"));
	}
	
}
