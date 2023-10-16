// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicles/Truck/Truck_BackWheels.h"

UTruck_BackWheels::UTruck_BackWheels()
{
	ApplyTirePropertiesB();
}

void UTruck_BackWheels::ApplyTirePropertiesB()
{
	/* Setting up properties of Tires */
	ShapeRadius = 35.0f;
	ShapeWidth = 22.0f;
	SteerAngle = 0.0f;
	bAffectedByHandbrake = true;

	if (TireConfig) {

		// Create instance of Custom Truck Tire config
		UTruck_TireConfig* customTruckConfig = NewObject<UTruck_TireConfig>();

		this->TireConfig = customTruckConfig;
	}
}
