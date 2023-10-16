// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Truck_TireConfig.h"
#include "VehicleWheel.h"
#include "Truck_BackWheels.generated.h"

/**
 * 
 */
UCLASS()
class ROKOPROJECT_API UTruck_BackWheels : public UVehicleWheel
{
	GENERATED_BODY()

public:
	UTruck_BackWheels();

	UFUNCTION()
	void ApplyTirePropertiesB();
	
};
