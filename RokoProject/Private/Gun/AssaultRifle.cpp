// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun/AssaultRifle.h"
#include "Player/RokoProjectCharacter.h"
#include "Kismet/GameplayStatics.h"

AAssaultRifle::AAssaultRifle() {

	setIsGun(true);
}

void AAssaultRifle::Use_Implementation()
{
	
	// Get reference of player character
	ARokoProjectCharacter* playerRef = Cast<ARokoProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	
	if (playerRef != nullptr) {
		
		// Change the already set up FP_GUN's skeletal mesh as this Gun
		playerRef->GetGunSK()->SetSkeletalMesh(Interactable_SK_Mesh);

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Gun Equipped"));
	}
	
}
