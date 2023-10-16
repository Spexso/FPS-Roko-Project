// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicles/Helicopter/VHelicopter.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Misc/App.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Rotator.h"
#include "Kismet/GameplayStatics.h"

// Initializing default variables
const float AVHelicopter::MaxBladeRotationSpeed = 1500.0f;
const float AVHelicopter::ThrottleUpSpeed = 200.0f;
const float AVHelicopter::TurnSpeed = 0.1f;
const float AVHelicopter::BlurBladeSpeed = 900.0f;



// Sets default values
AVHelicopter::AVHelicopter()
{
	// Bind event to the getPlayerInside Function 
	/*onEnterEvent.AddDynamic(this, &AVHelicopter::getPlayerInside);*/

// 	  	Create Spring arm, setup root
// 				springArmMainH = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Main"));
// 			 	springArmMainH->SetupAttachment(Body);
// 			 	springArmMainH->TargetArmLength = 1000.0f;
// 				springArmMainH->bUsePawnControlRotation = true;
// 		
// 				// Create Camera and attach
// 				mainCamH = CreateDefaultSubobject<UCameraComponent>(TEXT("Main Camera"));
// 				mainCamH->SetupAttachment(springArmMainH, USpringArmComponent::SocketName);
// 				mainCamH->bUsePawnControlRotation = false;

	// Set default values to zero for safety measure
	Velocity = FVector::ZeroVector;
	Acceleration = FVector::ZeroVector;
	PrevVelocity = FVector::ZeroVector;
	PrevAcceleration = FVector::ZeroVector;
	Prevposition = FVector::ZeroVector;
	CurrentPitch = 0.0f;
	CurrentTurn = 0.0f;
	isPlayerControlling = false;
}

// Called when the game starts or when spawned
void AVHelicopter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Testing c++ heli"));

	/*OnEnterEvent.AddDynamic(this, &AVHelicopter::getPlayerInside);*/
}

void AVHelicopter::ThrottleH(float value)
{

	// Debug purposes -> UE_LOG(LogTemp, Warning, TEXT("Power engage helicopter"));

	// Gets time delta in seconds
	double deltaT = FApp::GetDeltaTime();

	// Calculate moving speed 
	float BladeMovSpeed = deltaT * value * ThrottleUpSpeed;

	// Sum targeted blade speed for clamp
	float tempBladeSpeed = BladeMovSpeed + TargetBladeRotationSpeed;

	// Set new blade rotation speed
	TargetBladeRotationSpeed = FMath::Clamp(tempBladeSpeed, 0.0f, MaxBladeRotationSpeed);

}

void AVHelicopter::TurnH(float value) {

	// Interpretation speed
	float interpSpeed = 2.0f;

	// Set Current turn
	CurrentTurn = FMath::FInterpTo(CurrentTurn, (getTurnSpeed() * value), UGameplayStatics::GetWorldDeltaSeconds(this), interpSpeed);

	// Create rotator for Actor rotation
	FRotator newRotation = FRotator(0.0f, CurrentTurn, 0.0f);
	
	// Set World Rotation
	this->AddActorWorldRotation(newRotation);

	/* Set Actor Rotation */
	float turnSensivity = CurrentTurn * 40.0f;
	
	// Create new rotation based on sensivity, keep Pitch & Yaw of rotation
	float YawRotation = this->GetActorRotation().Yaw;
	float PitchRotation = this->GetActorRotation().Pitch;
	FRotator newActorRotation = FRotator(PitchRotation, YawRotation, turnSensivity);

	// Set new rotation as Actor's rotation
	SetActorRotation(newActorRotation);


}

void AVHelicopter::pitchH(float value)
{
	// Variable to check pitch's range
	bool isInRange = false;
	
	// Interpretation speed
	float interpSpeed = 2.0f;

	// Set Current turn
	CurrentPitch = FMath::FInterpTo(CurrentPitch, (getTurnSpeed() * value), UGameplayStatics::GetWorldDeltaSeconds(this), interpSpeed);

	// Get Current Pitch
	FVector currentRotation = GetActorRotation().Vector();

	// Check pitch range
	if (currentRotation.Y + CurrentPitch >= -89.0f && currentRotation.Y + CurrentPitch <= 89.0f) {

		isInRange = true;
	}

	// If pitch is not more than 90 degrees Add Actor Rotation 	
	if (isInRange) {

		FRotator newRotation = FRotator(CurrentPitch, 0.0f, 0.0f);

		this->AddActorLocalRotation(newRotation);
	}
	
}

float AVHelicopter::getTurnSpeed()
{
	// Minimum turn speed set up for turn action of helicopter 
	FVector2D inRange = FVector2D(0.0f, 400.0f);
	FVector2D outRange = FVector2D(0.0f, 1.0f);

	// Get Current lift for comparison
	float currLift = getCurrentLift();

	// Process current lift 
	float MinLift = FMath::GetMappedRangeValueClamped(inRange, outRange, currLift);

	/* Return minimum turn speed for turning */
	return (TurnSpeed * MinLift);
}

void AVHelicopter::rotateBlades(float DeltaSeconds)
{
	// References of static mesh components
	UStaticMeshComponent* rotorRef = nullptr;
	UStaticMeshComponent* rotorRearRef = nullptr;
	UStaticMeshComponent* bladesTopRef = nullptr;

	// Get Components related to given class
	TArray<UStaticMeshComponent*> rotorArray;
	GetComponents<UStaticMeshComponent>(rotorArray);

	// Tags of components
	FName rotor = "RotorTopMotor";
	FName rotorRear = "RotorRearMotor";


		// Find rotor blade that will be rotated
		for (UStaticMeshComponent* Comp : rotorArray) {

			if (IsValid(Comp)) {

				// Check if component has right tag
				if (Comp->ComponentHasTag(rotor)) {

					rotorRef = Comp;
					//UE_LOG(LogTemp, Warning, TEXT("RotorTopMotor found"));
				}
				if (Comp->ComponentHasTag(rotorRear)) {

					rotorRearRef = Comp;
					//UE_LOG(LogTemp, Warning, TEXT("RotorRearMotor found"));
				}

			}

		}

		//Reference null check
		if (rotorRearRef == nullptr || rotorRef == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Could not get rotor references!"));
			return;
		}

		// Calculation of rotation speed
		float DeltaZrotation = CurrentBladeRotationSpeed * DeltaSeconds;

		// Set up FRotator variables to add relative rotation
		FRotator Zrotation = FRotator(0.0f, DeltaZrotation, 0.0f);
		FRotator Yrotation = FRotator(2 * DeltaZrotation, 0.0f, 0.0f);								// 2X Increased speed for rear rotation 

		rotorRef->AddRelativeRotation(Zrotation);

		rotorRearRef->AddLocalRotation(Yrotation);

}

void AVHelicopter::getPlayerOut()
{

	// Set check for player control to false
	isPlayerControlling = false;

	/* Set the blade rotation speed to zero */
	TargetBladeRotationSpeed = 0;

	
	UE_LOG(LogTemp, Warning, TEXT("Getting player out (helicopter c++)"));

	// Variable to prevent collision with body of helicopter 
	FName socketName = "Pawn";

	/* Detach player from vehicle */
	driverPlayer->DetachFromActor(FDetachmentTransformRules::FDetachmentTransformRules::KeepWorldTransform);

	/* Set its new location before fall from vehicle */
	driverPlayer->SetActorLocation(locZone, false, false);


	/* Possess to current controller */
	this->GetController()->Possess(driverPlayer);

	/* Revert back to previous collision settings*/
	driverPlayer->getOutOfVehicle();
	
}

void AVHelicopter::updatePrevValues() {

	Prevposition = GetActorLocation();

	PrevVelocity = Velocity;

	PrevAcceleration = Acceleration;
}


void AVHelicopter::processDeltaTime(float DeltaSeconds)
{
	FVector gravityValue = FVector(0.0f, 0.0f, -981.0f);
	
	FVector upVector;
	float Xup;
	float Yup;

	// Updating Blade rotation speed 
	CurrentBladeRotationSpeed = FMath::FInterpTo(CurrentBladeRotationSpeed, TargetBladeRotationSpeed, DeltaSeconds, 1.0f);

	// Call of the function that will rotate top and rear blades(through rotating motor)
	rotateBlades(DeltaSeconds);

	// Calculate Velocity
	Velocity = DeltaSeconds * (GetActorLocation() - Prevposition);

	// Calculate Acceleration
	Acceleration = DeltaSeconds * (Velocity - PrevVelocity);

	/***************************************************************/
	/* Update prev values to prevent invalid calculations */
	updatePrevValues();
	// Smoothing movement by adding gravity effect to acceleration
	Acceleration = Acceleration + gravityValue;

	// Lift movement
	Acceleration = Acceleration + (this->GetActorUpVector() * getCurrentLift());

	// Forward Movement while lifting
	upVector = this->GetActorUpVector();
	Xup = upVector.X;
	Yup = upVector.Y;
	FVector FbuildVector(Xup, Yup, 0.0f);

	Acceleration = Acceleration + (2 * getCurrentLift() * FbuildVector);

	// Move Helicopter in world
	FVector newLocation = Velocity + (Acceleration * DeltaSeconds);
	AddActorWorldOffset(newLocation, true);

}

float AVHelicopter::getCurrentLift()
{
	// Create a rich curve
	auto richCurve = new FRichCurve();
	
	// Add keys to curve
	richCurve->AddKey(300.0f, 0.0f);
	richCurve->AddKey(15000.0f, 13000.0f);

	// Create a CurveFloat 
	auto curve = NewObject<UCurveFloat>();

	// Assign curve to curvefloat
	curve->FloatCurve = *richCurve;

	/* Return the float value of curve */
	return curve->GetFloatValue(CurrentBladeRotationSpeed);
}

void AVHelicopter::getPlayerInside(ARokoProjectCharacter* occupiedPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("Helicopter c++"));

	if (!IsValid(occupiedPlayer))
		return;

	if (occupiedPlayer != nullptr)
	{

		// Variable to prevent collision with body of helicopter 
		FName socketName = "pilot_seat";
		
		// Copying reference of player that is inside helicopter
		driverPlayer = occupiedPlayer;

		// Setting up Attachment Rules
		FAttachmentTransformRules generalRuleForPossessing = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

		// Transfer controller from Pawn player to Helicopter
		occupiedPlayer->GetController()->Possess(this);
		UE_LOG(LogTemp, Warning, TEXT("Character possessed to Helicopter"));


		// Attach player to helicopter
		occupiedPlayer->AttachToActor(this, generalRuleForPossessing, socketName);


		isPlayerControlling = true;
	}
}


float AVHelicopter::getRotationSpeed()
{

	return TargetBladeRotationSpeed;
}

// Called every frame
void AVHelicopter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	processDeltaTime(DeltaTime);
}



// Called to bind functionality to input
void AVHelicopter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/* Bind Controls of helicopter such as Entering vehicle, movement, etc. */
	PlayerInputComponent->BindAction("EnterVehicle", IE_Pressed, this, &AVHelicopter::getPlayerOut);
	PlayerInputComponent->BindAxis("HThrottle", this, &AVHelicopter::ThrottleH);
	PlayerInputComponent->BindAxis("HTurn", this, &AVHelicopter::TurnH);
	PlayerInputComponent->BindAxis("HPitch", this, &AVHelicopter::pitchH);
}

