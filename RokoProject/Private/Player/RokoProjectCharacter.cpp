// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/RokoProjectCharacter.h"
#include "Vehicles/Helicopter/VHelicopter.h"
#include "Game/RokoProjectGameMode.h"
#include "Vehicles/Truck/VTruck.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gun/RokoProjectProjectile.h"
#include "Controller/ModePlayerController.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ARokoProjectCharacter

ARokoProjectCharacter::ARokoProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	//FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
 	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	// Set default sprint info
	isSprinting = false;

	// Set reach distance of character for interacting
	reachDistance = 250.0f;

	// Construct sound FX and get reference to sound cue 
	static ConstructorHelpers::FObjectFinder<USoundCue> RunningSoundFX(TEXT("'/Game/FirstPerson/Audio/Running_SoundFX.Running_SoundFX'"));
	RunningAudioCue = RunningSoundFX.Object;

	// Create Audio component to attach sound cue 
	RunningAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("RunningAudioComp"));
	// Set it to false not play
	RunningAudioComponent->bAutoActivate = false;
	// Attach Running Audio Component to player
	RunningAudioComponent->SetupAttachment(GetRootComponent());
	// Set location of source sound FX
	RunningAudioComponent->SetRelativeLocation( FVector(-100.0f, 0.0f, 0.0f) );

	// Set Full auto Gun firing speed 
	TimeBetweenShoots = 0.20f;

	// Set Health and MaxHealth
	MaxHealth = 100.0f;
	Health = 100.0f;

}

void ARokoProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Set the inventory capacity
	Inventory.SetNum(4);

	// Set Interactable to null pointer for preventing errors 
	currentInteractable = nullptr;

	// Set Sound of Running Audio Component as Running Sound Cue 
	if (RunningAudioCue->IsValidLowLevelFast()) {

		RunningAudioComponent->SetSound(RunningAudioCue);
	}

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}


}

void ARokoProjectCharacter::Tick(float deltaTime) {

	Super::Tick(deltaTime);

	/* Not fully implemented yet */
// 	if (Health == 0.0f) {
// 
// 		// Player dies
// 		ARokoProjectGameMode* GameModeRef = Cast<ARokoProjectGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
// 
// 		if (GameModeRef != nullptr) {
// 
// 			GameModeRef->RespawnCharacter(this);
// 		}
// 	}


	if (isSprinting) {

		// Drain stamina through running action
		drainStamina();

	}

	if (stamina != staminaMax && !isSprinting) {

		// Refresh stamina while player not running
		regainStamina();

	}

	CheckforIntractable();
}

void ARokoProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	/*PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ARokoProjectCharacter::OnFire);*/
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ARokoProjectCharacter::startFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ARokoProjectCharacter::stopFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ARokoProjectCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ARokoProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARokoProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARokoProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARokoProjectCharacter::LookUpAtRate);


	/* Additional */

	/* Bind entering vehicle action */
	PlayerInputComponent->BindAction("EnterVehicle", IE_Pressed, this, &ARokoProjectCharacter::getInsideVehicle);

	/* Bind Character Sprinting */
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ARokoProjectCharacter::startSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ARokoProjectCharacter::stopSprint);

	/* Bind Inventory operations */
	PlayerInputComponent->BindAction("ToggleInventory", IE_Pressed, this, &ARokoProjectCharacter::ToggleInventory);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ARokoProjectCharacter::Interact);
}


//////////////////////////////////////////////////////////////////////////
// Input

/* Gun fire System */
////////////////////////////////////////////////////////////////////////////////
void ARokoProjectCharacter::OnFire()
{
	// Check if character sprinting
	if (isSprinting) {
		// Cant fire while sprinting
		UE_LOG(LogTemp, Warning, TEXT("Can not fire the gun while sprinting!"));
		return;
	}

	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<ARokoProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<ARokoProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void ARokoProjectCharacter::startFire()
{
	fireShot();

	// Set timer to keep calling fireShot function for automatic weapon functioning
	GetWorldTimerManager().SetTimer(TH_HandleFire, this, &ARokoProjectCharacter::fireShot, TimeBetweenShoots, true);

}

void ARokoProjectCharacter::stopFire()
{

	GetWorldTimerManager().ClearTimer(TH_HandleFire);
}

void ARokoProjectCharacter::fireShot()
{
	// Check if character sprinting
	if (isSprinting) {
		// Cant fire while sprinting
		UE_LOG(LogTemp, Warning, TEXT("Can not fire the gun while sprinting!"));
		return;
	}

	///////////////////////////////////////////////////////////////////////////////
	
	FHitResult Hit;

	// Weapon fire range
	const float BulletRange = 20000.f;
	
	// Variables to set range of bullets
	const FVector StartBulletTrace = GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector EndBulletTrace = (GetFirstPersonCameraComponent()->GetForwardVector() * BulletRange + StartBulletTrace);

	// Ignore firing character mesh
	FCollisionQueryParams QueryPrm = FCollisionQueryParams(SCENE_QUERY_STAT(WeaponTrace), false, this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartBulletTrace, EndBulletTrace, ECC_Visibility, QueryPrm)) {

		if (ImpactParticles) {

			/* On hit Spawn Impact Particles */
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint));
		}

	}

	if (MuzzleParticles) {

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleParticles, FP_Gun->GetSocketTransform(FName("Muzzle")) );
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////

USkeletalMeshComponent* ARokoProjectCharacter::GetFirstPersonCameraMesh()
{
	return Mesh1P;
}

USkeletalMeshComponent* ARokoProjectCharacter::GetGunMesh()
{
	return VR_Gun;
}

USkeletalMeshComponent* ARokoProjectCharacter::GetGunSK()
{
	return FP_Gun;
}

// Unreal Default Methods
void ARokoProjectCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ARokoProjectCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		fireShot();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ARokoProjectCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ARokoProjectCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ARokoProjectCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ARokoProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ARokoProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARokoProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ARokoProjectCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ARokoProjectCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ARokoProjectCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ARokoProjectCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

/* Inventory System */
////////////////////////////////////////////////////////////////////////////////
void ARokoProjectCharacter::ToggleInventory()
{
	/* Check Player's HUD State , based upon state close or open HUD Inventory */

	// Get Game mode reference 
	ARokoProjectGameMode* GMReference = Cast<ARokoProjectGameMode>(GetWorld()->GetAuthGameMode());

	if (GMReference != nullptr) {

		if (GMReference->GetHUDState() == GMReference->HS_InGame) {

			// Set to Inventory HUD
			GMReference->setHUDState(GMReference->HS_Inventory);

			// Disable Looking around with mouse 
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetIgnoreLookInput(true);
			//UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeUIOnly());
		}
		else {

			// Set to In game HUD
			GMReference->setHUDState(GMReference->HS_InGame);

			// Enable Looking around with mouse
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetIgnoreLookInput(false);
			//UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeGameOnly());
		}
	}
}

void ARokoProjectCharacter::Interact()
{
	if (currentInteractable != nullptr) {

		currentInteractable->Interact_Implementation();
	}
}

void ARokoProjectCharacter::CheckforIntractable()
{
	// Create line trace by getting start and end traces
	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	FVector EndTrace = (FirstPersonCameraComponent->GetForwardVector() * reachDistance) + StartTrace;

	// HitResult declaration to store raycast hit in
	FHitResult HitResult;

	// Initialize the query params - ignore the actor
	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);

	// Cast the line trace 
	GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_WorldDynamic, CQP);
	
	// Get hit actor 
	AInteractableBaseObject* potentialInteractable = Cast<AInteractableBaseObject>(HitResult.GetActor());
	
	// No object to interact with
	if (potentialInteractable == NULL) {
		
		helpText = FString("");
		currentInteractable = NULL;
		return;
	}
	else {

		currentInteractable = potentialInteractable;
		helpText = potentialInteractable->getHelpText();
	}
}

// Not used
void ARokoProjectCharacter::updateGold(int32 AmountChange)
{
	// Withdraws the amount or adds to it
	GoldAmount += GoldAmount;
}

bool ARokoProjectCharacter::addItemToInventory(ApickupBaseObject* Item)
{
	if (Item != nullptr) {

		// Find an empty slot which is a null pointer 
		const int32 AvailableSlot = Inventory.Find(nullptr);

		if (AvailableSlot != INDEX_NONE) {

			// Store item at available slot of inventory
			Inventory[AvailableSlot] = Item;
			return true;
		}
		else {

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Inventory has reached maximum capacity!"));
			return false;
		}
	}
	else
		return false;
}

UTexture2D* ARokoProjectCharacter::getThumbnailSlot(int32 slotNumber)
{
	if (Inventory[slotNumber] != NULL) {

		return Inventory[slotNumber]->ItemThumbnail;
	}
	else
		return false;
}

FString ARokoProjectCharacter::getItemNameSlot(int32 slotNumber)
{
	if (Inventory[slotNumber] != NULL) {

		return Inventory[slotNumber]->ItemName;
	}
	else
		return FString("None");
}

void ARokoProjectCharacter::useItemAtSlot(int32 slotNumber)
{
	if (Inventory[slotNumber] != NULL) {

		Inventory[slotNumber]->Use_Implementation();
		Inventory[slotNumber] = NULL; // Deletes the item from inventory after using it 
	}

}
////////////////////////////////////////////////////////////////////////////////


/* Stamina System */
////////////////////////////////////////////////////////////////////////////////
void ARokoProjectCharacter::startSprint()
{
	int32 depletedStamina = 0;
	
	if (stamina == depletedStamina){
		UE_LOG(LogTemp, Warning, TEXT("No stamina for sprinting"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Sprinting initiated"));
	
	/* Setting walk speed for sprinting */
	this->GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;

	// Play running sound FX
	RunningAudioComponent->Play();

	// Set sprint info
	isSprinting = true;
}

void ARokoProjectCharacter::stopSprint()
{
	UE_LOG(LogTemp, Warning, TEXT("Sprinting stopped"));
	
	/* Setting walk speed for default */
	this->GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	
	// Stop running sound FX
	RunningAudioComponent->Stop();

	// Set sprint info
	isSprinting = false;
}

void ARokoProjectCharacter::drainStamina()
{
	int32 singleDrainValue = 1;
	int32 depletedStamina = 0;

	/* Subtract drain value from current stamina of character */
	stamina -= singleDrainValue;

	stamina = FMath::Clamp(stamina, depletedStamina, staminaMax);

	if (stamina == depletedStamina) {

		stopSprint();
	}
}

void ARokoProjectCharacter::regainStamina()
{
	int32 singleGainValue = 1;
	int32 depletedStamina = 0;

	/* Subtract drain value from current stamina of character */
	stamina += singleGainValue;

	stamina = FMath::Clamp(stamina, depletedStamina, staminaMax);
}

int32 ARokoProjectCharacter::getStamina()
{
	// Returns the stamina 
	return stamina;
}

int32 ARokoProjectCharacter::getMaxStamina()
{
	return staminaMax;
}
////////////////////////////////////////////////////////////////////////////////

/* Health System */
////////////////////////////////////////////////////////////////////////////////
void ARokoProjectCharacter::giveDamage(float DamageAmount)
{
	// Decreases the Health variable & Checks if it reaches Min or Max Health
	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
}

float ARokoProjectCharacter::getMaxHealth()
{
	return MaxHealth;
}

float ARokoProjectCharacter::getHealth()
{
	return Health;
}

void ARokoProjectCharacter::heal(float HealAmount)
{
	// Increases the Health variable & Checks if reaches Min & Max health
	Health = FMath::Clamp(Health + HealAmount, 0.0f, MaxHealth);
}
////////////////////////////////////////////////////////////////////////////////


/* Vehicle System */
////////////////////////////////////////////////////////////////////////////////
/* Gets overlapped Actors, if its Target Vehicle casts character to Vehicle then */
void ARokoProjectCharacter::getInsideVehicle() {

	// Testing function functionality
	UE_LOG(LogTemp, Warning, TEXT("Player trying to get into vehicle!"));

	// Variable to prevent collision with body of helicopter 
	FName collisionKey = "NoCollision";
	FName socketName = "Pawn";


	// List of overlapping actors 
	TArray<AActor*> overlappedActors;

	/* Get Helicopter that player is inside Entering Zone */
	GetOverlappingActors(overlappedActors, AVHelicopter::StaticClass());

	if (overlappedActors.Num() != 0 && !isInsideVehicle) {

		/* Get Helicopter that player tries to get into */
		AVHelicopter* occupiedHelicopter = Cast<AVHelicopter>(overlappedActors[0]);

		// Check for valid object
		if (occupiedHelicopter != nullptr) {

			/* Remove collision of player to prevent overlapping inside helicopter */
			GetCapsuleComponent()->SetCollisionProfileName(collisionKey, true);

			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			/* Getting inside Vehicle as Character */
			occupiedHelicopter->getPlayerInside(this);
			isInsideVehicle = true;
		}

	}

	/* Get Car that player is inside Entering Zone */
	GetOverlappingActors(overlappedActors, AVTruck::StaticClass());

	if (overlappedActors.Num() != 0 && !isInsideVehicle) {

		/* Get Car that player tries to get into */
		AVTruck* occupiedCar = Cast<AVTruck>(overlappedActors[0]);

		// Check for valid object
		if (occupiedCar != nullptr) {

			/* Remove collision of player to prevent overlapping inside car */
			GetCapsuleComponent()->SetCollisionProfileName(collisionKey, true);

			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			/* Getting inside Vehicle as Character */
			occupiedCar->getPlayerInside(this);
			isInsideVehicle = true;
		}

	}

}

/* Sets up the character for leave the vehicle */
void ARokoProjectCharacter::getOutOfVehicle()
{
	// Variable to prevent collision with body of helicopter 
	FName socketName = "Pawn";

	// Testing function functionality
	UE_LOG(LogTemp, Warning, TEXT("Player trying to get out of vehicle!"));

	if (isInsideVehicle) {

		GetCapsuleComponent()->SetCollisionProfileName(socketName, true);
		isInsideVehicle = false;
	}

}
////////////////////////////////////////////////////////////////////////////////
