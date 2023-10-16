// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicles/Truck/Truck_FrontWheels.h"


UTruck_FrontWheels::UTruck_FrontWheels()
{
	ApplyTirePropertiesF();
}

void UTruck_FrontWheels::ApplyTirePropertiesF()
{
	/* Setting up properties of Tires */
	ShapeRadius = 35.0f;
	ShapeWidth = 22.0f;
	SteerAngle = 60.0f;
	bAffectedByHandbrake = false;

	if (TireConfig) {

		// Create instance of Custom Truck Tire config
		UTruck_TireConfig* customTruckConfig = NewObject<UTruck_TireConfig>();

		this->TireConfig = customTruckConfig;
	}
}
