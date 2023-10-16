// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "Interactable/Pickupable/pickupBaseObject.h"
#include "Medkit.generated.h"

/**
 * 
 */
UCLASS()
class ROKOPROJECT_API AMedkit : public ApickupBaseObject
{
	GENERATED_BODY()

	// Amount of heal that will be applied to character when used
	float HealAmount = 15.0f;

protected:

	/** Healing Sound Effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Effect")
	USoundCue* MedkitSoundFX;

	// Helper variable for playing Sound Cue
	UAudioComponent* MedkitSound;

public:
	AMedkit();

	virtual void Use_Implementation() override;
	
};
