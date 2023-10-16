// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Player/RokoProjectCharacter.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameModeBase.h"
#include "HAL/Platform.h"
#include "RokoProjectGameMode.generated.h"

UCLASS(minimalapi)
class ARokoProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

	uint8 HUDState;

	void BeginPlay() override;

public:
	ARokoProjectGameMode();

	enum EHUDState : uint8
	{
		HS_InGame,
		HS_Inventory,
		HS_Shop
	};

	/* Checks the HUD's state and calls applyHUD to bring correct HUD to screen*/
	void applyHUDChanges();

	/* Returns the State of HUD */
	uint8 GetHUDState();

	/* Sets the new HUD state, calls applyHUDChanges */
	UFUNCTION(BlueprintCallable, Category = "Inventory HUD")
	void setHUDState(uint8 newState);

	/* Applies a new HUD and returns if successful, false otherwise */
	bool applyHUD(TSubclassOf<class UUserWidget> widgetToApply, bool bShowCursor, bool enableClick);

	/* Respawn Player */
	void RespawnCharacter(ARokoProjectCharacter* Player);

protected:

	// HUD that is showed in game
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory HUD", Meta = (Blueprintprotected = "true"))
	TSubclassOf<class UUserWidget> InGameHUDClass;

	// HUD that is showed in inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory HUD", Meta = (Blueprintprotected = "true"))
	TSubclassOf<class UUserWidget> InventoryHUDClass;

	// HUD that is showed in shop
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory HUD", Meta = (Blueprintprotected = "true"))
	TSubclassOf<class UUserWidget> ShopHUDClass;

	UPROPERTY()
	class UUserWidget* CurrentOnScreenWidget;
};



