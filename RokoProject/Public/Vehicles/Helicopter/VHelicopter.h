// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RokoProject/Public/Player/RokoProjectCharacter.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "VHelicopter.generated.h"

/*DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelegate);*/

UCLASS()
class ROKOPROJECT_API AVHelicopter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVHelicopter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Getting player inside 
	UFUNCTION()
	void getPlayerInside(ARokoProjectCharacter* occupiedPlayer);

	// Variable to check for player
	UPROPERTY(BlueprintReadOnly, Category = "CheckPlayer")
	bool isPlayerControlling;

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	float getRotationSpeed();

	/* Event dispatcher try */
// 	UPROPERTY(BlueprintAssignable, Category = "Test")
// 	FTestDelegate onEnterEvent;


protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Getting player out 
	UFUNCTION()
	void getPlayerOut();

	// Variable to hold Occupied Character
	UPROPERTY()
	ARokoProjectCharacter* driverPlayer;

	UPROPERTY(BlueprintReadWrite, Category = "boardZoneLocation")
	FVector locZone;


private:

	UFUNCTION()
	void updatePrevValues();

	//Helicopter Power
	UFUNCTION()
	void ThrottleH(float value);

	//Helicopter Turn
	UFUNCTION()
	void TurnH(float value);

	//Helicopter Pitch
	void pitchH(float value);

	//Prevent turn action under some speed
	UFUNCTION()
	float getTurnSpeed();

	UFUNCTION()
	void rotateBlades(float DeltaSeconds);

	UFUNCTION()
	void processDeltaTime(float DeltaSeconds);

	UFUNCTION()
	float getCurrentLift();

	/* Target Rotation Speed of Blades */
	UPROPERTY()
	float TargetBladeRotationSpeed;
	
	/* Rotation Speed of Blades */
	UPROPERTY()
	float CurrentBladeRotationSpeed;

	/* Velocity of vehicle */
	UPROPERTY()
	FVector Velocity;

	/* Acceleration of vehicle */
	UPROPERTY()
	FVector Acceleration;

	/* Previous velocity of vehicle */
	UPROPERTY()
	FVector PrevVelocity;

	/* Previous acceleration of vehicle */
	UPROPERTY()
	FVector PrevAcceleration;

	/* Previous position of vehicle */
	UPROPERTY()
	FVector Prevposition;

	/* Amount of turn that will be applied to */
	UPROPERTY()
	float CurrentTurn;

	/* Amount of pitch that applies to */
	UPROPERTY()
	float CurrentPitch;

	// Constants Values that used on Movement Component
	static const float MaxBladeRotationSpeed;
	static const float ThrottleUpSpeed;
	static const float TurnSpeed;
	static const float BlurBladeSpeed;

	// Helper variable to operate natural movement
	UPROPERTY()
	UCurveFloat* LiftCurve;

	/* Spring arm that will offset the camera */
	UPROPERTY(Category = "HelicopterSpringArm", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* springArmMainH;

	/** Camera Component that will be the viewpoint */
	UPROPERTY(Category = "HelicopterCam", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* mainCamH;


};
