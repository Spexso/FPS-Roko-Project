// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/RokoProjectGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/RokoProjectHUD.h"
#include "Player/RokoProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARokoProjectGameMode::ARokoProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ARokoProjectHUD::StaticClass();

	// Set default HUD state to Ingame HUD state 
	HUDState = EHUDState::HS_InGame;
}


void ARokoProjectGameMode::applyHUDChanges()
{
	// Remove the previous HUD
	if (CurrentOnScreenWidget != nullptr)
	{
		CurrentOnScreenWidget->RemoveFromParent();
	}

	switch (HUDState)
	{

	case EHUDState::HS_InGame:
		applyHUD(InGameHUDClass, false, false);
	break;

	case EHUDState::HS_Inventory:
		applyHUD(InventoryHUDClass, true, true);
	break;

	case EHUDState::HS_Shop:
		applyHUD(ShopHUDClass, true, true);
	break;

	default:
		applyHUD(InGameHUDClass, false, false);
	break;
	
	}
}

uint8 ARokoProjectGameMode::GetHUDState()
{
	return HUDState;
}

void ARokoProjectGameMode::setHUDState(uint8 newState)
{
	HUDState = newState;

	applyHUDChanges();

}

bool ARokoProjectGameMode::applyHUD(TSubclassOf<class UUserWidget> widgetToApply, bool bShowCursor, bool enableClick)
{
	// Set Widget Name
	//FName widgetName = "Current Applied Widget";

	// Get reference to both Player & Controller
	ARokoProjectCharacter* player = Cast<ARokoProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	APlayerController* controller = GetWorld()->GetFirstPlayerController();

	// Check if references valid 
	if (player != nullptr && controller != nullptr) {


		// Check if widget that will be applied is valid
		if (widgetToApply != nullptr) {

			// Set cursor and click based on params
			controller->bShowMouseCursor = bShowCursor;
			controller->bEnableClickEvents = enableClick;

			// Create the Widget
			CurrentOnScreenWidget = CreateWidget<UUserWidget>(GetWorld(), widgetToApply);

			// Check validness
			if (CurrentOnScreenWidget != nullptr) {

				CurrentOnScreenWidget->AddToViewport();
				return true;
			}
		}
	}

	return false;
}

/* Not yet implemented */
void ARokoProjectGameMode::RespawnCharacter(ARokoProjectCharacter* Player)
{
// 	FTransform RespawnTransform;
// 	FActorSpawnParameters RespawnParams;
// 
// 	// Respawn the player 
// 	UWorld::SpawnActor(Player->GetClass(), RespawnTransform, RespawnParams);
}

void ARokoProjectGameMode::BeginPlay() {

	// When player spawn, set HUD to screen
	applyHUDChanges();
}