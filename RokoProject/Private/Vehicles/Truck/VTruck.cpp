// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicles/Truck/VTruck.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"

/* Default naming for throttle and steering */
static const FName NAME_SteerInp("MoveRight");
static const FName NAME_ThrottleInp("MoveForward");

AVTruck::AVTruck()
{
	UWheeledVehicleMovementComponent4W* Veh4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	/* Setting up tire turn */
	Veh4W->MinNormalizedTireLoad = 0.0f;
	Veh4W->MinNormalizedTireLoadFiltered = 0.2f;
	Veh4W->MaxNormalizedTireLoad = 2.0f;
	Veh4W->MaxNormalizedTireLoadFiltered = 2.0f;

	/* Setting up Vehicle Torque Curve */
	Veh4W->MaxEngineRPM = 5000.0f;
	Veh4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Veh4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	Veh4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1900.0f, 500.0f);
	Veh4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5600.0f, 400.0f);

	/* Setting up Steering Curve */
	Veh4W->SteeringCurve.GetRichCurve()->Reset();
	Veh4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	Veh4W->SteeringCurve.GetRichCurve()->AddKey(40.0f, 0.7f);
	Veh4W->SteeringCurve.GetRichCurve()->AddKey(130.0f, 0.6f);

	/* Setting up wheel slip */
	Veh4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
	Veh4W->DifferentialSetup.FrontRearSplit = 0.65;

	/* Setting up Auto Gearbox */
	Veh4W->TransmissionSetup.bUseGearAutoBox = true;
	Veh4W->TransmissionSetup.GearSwitchTime = 0.15f;
	Veh4W->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	/* Creating Spring arm for Camera */
	SpringArm = CreateEditorOnlyDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 250.0f;
	SpringArm->bUsePawnControlRotation = true;

	/* Setting up Chase Camera */
	Camera = CreateEditorOnlyDefaultSubobject<UCameraComponent>(TEXT("ChaseCam"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->FieldOfView = 90.f;

	isPlayerControlling = false;
}

void AVTruck::applyThrottle(float value)
{
	GetVehicleMovementComponent()->SetThrottleInput(value);
}

void AVTruck::applySteering(float value)
{
	GetVehicleMovementComponent()->SetSteeringInput(value);
}

void AVTruck::LookUp(float value)
{
	if (value != 0.f) {

		AddControllerPitchInput(value);
	}
}

void AVTruck::Turn(float value)
{
	if (value != 0.f) {

		AddControllerYawInput(value);
	}
}

void AVTruck::OnHandBrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AVTruck::OnHandBrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AVTruck::getPlayerInside(ARokoProjectCharacter* occupiedPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("SUV car c++"));

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
		UE_LOG(LogTemp, Warning, TEXT("Character possessed to Car"));


		// Attach player to helicopter
		occupiedPlayer->AttachToActor(this, generalRuleForPossessing, socketName);


		isPlayerControlling = true;
	}
}

void AVTruck::getPlayerOut()
{
	/* Stop car movement before leaving */
	GetVehicleMovementComponent()->SetThrottleInput(0);
	GetVehicleMovementComponent()->SetSteeringInput(0);

	// Set check for player control to false
	isPlayerControlling = false;

	UE_LOG(LogTemp, Warning, TEXT("Getting player out (car c++)"));

	// Variable to prevent collision with body of car
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


void AVTruck::BeginPlay()
{
	Super::BeginPlay();

	//
}

void AVTruck::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//
}

void AVTruck::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	/* Setting up Player inputs */
	PlayerInputComponent->BindAxis(NAME_ThrottleInp, this, &AVTruck::applyThrottle);
	PlayerInputComponent->BindAxis(NAME_SteerInp, this, &AVTruck::applySteering);
	PlayerInputComponent->BindAxis("LookUp", this, &AVTruck::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AVTruck::Turn);

	/* Handbrake */
	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AVTruck::OnHandBrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AVTruck::OnHandBrakeReleased);

	/* Getting out of vehicle (Works reversed since same input used on player side) */
	PlayerInputComponent->BindAction("EnterVehicle", IE_Pressed, this, &AVTruck::getPlayerOut);

}
