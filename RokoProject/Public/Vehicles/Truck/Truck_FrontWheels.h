// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Truck_TireConfig.h"
#include "VehicleWheel.h"
#include "Truck_FrontWheels.generated.h"

/**
 * 
 */
UCLASS()
class ROKOPROJECT_API UTruck_FrontWheels : public UVehicleWheel
{
	GENERATED_BODY()
	
public:
	UTruck_FrontWheels();

	UFUNCTION()
	void ApplyTirePropertiesF();
};
