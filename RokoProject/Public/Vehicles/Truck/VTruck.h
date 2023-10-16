// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RokoProject/Public/Player/RokoProjectCharacter.h"
#include "WheeledVehicle.h"
#include "VTruck.generated.h"

/**
 * 
 */
UCLASS()
class ROKOPROJECT_API AVTruck : public AWheeledVehicle
{
	GENERATED_BODY()
	
public:

	AVTruck();

	// Movement

	/* Throttle */
	void applyThrottle(float value);

	/* Steering */
	void applySteering(float value);

	/* Look around */
	void LookUp(float value);
	void Turn(float value);

	/* Handbrake */
	void OnHandBrakePressed();
	void OnHandBrakeReleased();

	// Getting player inside 
	UFUNCTION()
	void getPlayerInside(ARokoProjectCharacter* occupiedPlayer);

	// Getting player out 
	UFUNCTION()
	void getPlayerOut();

	// Variable to check for player
	UPROPERTY(BlueprintReadOnly, Category = "CheckPlayer")
	bool isPlayerControlling;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/* Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	/** Camera Component that will be the viewpoint */
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;

	// While unpossesing spawn location of character
	UPROPERTY(BlueprintReadWrite, Category = "boardZoneLocation")
	FVector locZone;

	// Variable to hold Occupied Character
	UPROPERTY()
	ARokoProjectCharacter* driverPlayer;

};
