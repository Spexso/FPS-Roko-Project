// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RokoProjectHUD.generated.h"

UCLASS()
class ARokoProjectHUD : public AHUD
{
	GENERATED_BODY()

public:
	ARokoProjectHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;


private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

