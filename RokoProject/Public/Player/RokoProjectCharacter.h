// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RokoProject/Public/Interactable/InteractableBaseObject.h"
#include "RokoProject/Public/Interactable/Pickupable/pickupBaseObject.h"
#include "Sound/SoundCue.h"
#include "RenderCore.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "RokoProjectCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ARokoProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* L_MotionController;


	/* Default Speed of walking */
	UPROPERTY()
	float walkSpeed = 600.0f;

	/* Default Speed of sprinting */
	UPROPERTY()
	float sprintSpeed = 1000.0f;

	/* Maximum Default Health */
	UPROPERTY()
	float MaxHealth;

	/* Health of Character */
	UPROPERTY()
	float Health;

	/* Default Stamina of character */
	UPROPERTY()
	int32 stamina = 200;

	/* Max Stamina of character */
	UPROPERTY()
	int32 staminaMax = 200;

	/* Running sound FX audio cue */
	USoundCue* RunningAudioCue;

	/* Running sound FX audio component*/
	UAudioComponent* RunningAudioComponent;

public:
	ARokoProjectCharacter();

	/* Getting out of vehicle */
	UFUNCTION()
	void getOutOfVehicle();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ARokoProjectProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Seconds to wait for shoots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float TimeBetweenShoots;

	/** Particle Effect for Muzzle particles from gun */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UParticleSystem* MuzzleParticles;

	/** Particle Effect for Impact of Bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UParticleSystem* ImpactParticles;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

protected:

	/* Variable to track if player is inside vehicle */
	// Starts outside of any vehicle
	UPROPERTY(BlueprintReadOnly)
	bool isInsideVehicle = false;
	
	/* Entering vehicle action */
	UFUNCTION()
	void getInsideVehicle();


	/* Stamina Functionality */
	///////////////////////////////////////////////////////////////////////
	/* Sprint Action Start */
	UFUNCTION()
	void startSprint();

	/* Sprint Action Stop */
	UFUNCTION()
	void stopSprint();

	/* Stamina draining function for sprinting */
	UFUNCTION()
	void drainStamina();

	/* Stamina regaining function after stamina depleted */
	UFUNCTION()
	void regainStamina();

	/* Returns current stamina of Character */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	int32 getStamina();

	/* Returns the maximum stamina of Character */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	int32 getMaxStamina();

	/* Holder for sprinting info */
	UPROPERTY(BlueprintReadOnly, Category = "Sprint")
	bool isSprinting;

	/* Return the first person camera mesh */
	UFUNCTION(BlueprintCallable, Category = "Component Call")
	USkeletalMeshComponent* GetFirstPersonCameraMesh();

	/* Return the gun mesh */
	UFUNCTION(BlueprintCallable, Category = "Component Call")
	USkeletalMeshComponent* GetGunMesh();


	// Original Unreal Source
	/** Fires a projectile. */
	void OnFire();

	/* Custom Fire Start function for auto Gun */
	void startFire();

	/* Custom Fire Stop function for auto Gun */
	void stopFire();

	/* Every shot while firing Gun */
	void fireShot();

	// Timer variable to handle every shot of weapon
	FTimerHandle TH_HandleFire;

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);
	
/* Inventory Functionality */
///////////////////////////////////////////////////////////////////////
private:
	
	/* Players reach capacity */
	float reachDistance;

	/* Open the inventory HUD */
	void ToggleInventory();

	/* Interact with the provided intractable class object */
	void Interact();

	/* Checks for intractable objects directly in front of player using a line trace, works on per tick basis */
	void CheckforIntractable();

	// Interactable object that player looking at
	AInteractableBaseObject* currentInteractable;

	// Inventory of Player that defined as array of Pickable Base Object
	UPROPERTY(EditAnywhere)
	TArray<ApickupBaseObject*> Inventory;

public:

	/* Return the gun mesh for c++ */
	USkeletalMeshComponent* GetGunSK();
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/* Health Functionality */
	///////////////////////////////////////////////////////////////////////
	/* Give Damage to Player */
	UFUNCTION(BlueprintCallable, Category = "Health System")
	void giveDamage(float DamageAmount);

	/* Returns the Max Health of Player */
	UFUNCTION(BlueprintCallable, Category = "Health System")
	float getMaxHealth();

	/* Returns the Current Health of Player */
	UFUNCTION(BlueprintCallable, Category = "Health System")
	float getHealth();

	/* Heals the Character by increasing Health */
	UFUNCTION(BlueprintCallable, Category = "Health System")
	void heal(float HealAmount);
	

	/* Inventory Functionality */
	///////////////////////////////////////////////////////////////////////
	/* Help text for interacting object*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory HUD")
	FString helpText;

	/* The Amount of Gold that player has */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory HUD")
	int32 GoldAmount;

	/* Update Gold amount of player 
	@param Amount of gold change (positive or negative) 
	*/
	UFUNCTION(BlueprintCallable, Category = "Inventory Operations")
	void updateGold(int32 AmountChange);

	/* Adds an item to Inventory */
	UFUNCTION(BlueprintPure, Category = "Inventory Operations")
	bool addItemToInventory(ApickupBaseObject* Item);

	/* Get the thumbnail for single inventory slot */
	UFUNCTION(BlueprintPure, Category = "Inventory Operations")
	UTexture2D* getThumbnailSlot(int32 slotNumber);

	/* Get item name for single inventory slot */
	UFUNCTION(BlueprintPure, Category = "Inventory Operations")
	FString getItemNameSlot(int32 slotNumber);

	/* Use function for item at an inventory slot */
	UFUNCTION(BlueprintCallable, Category = "Inventory Operations")
	void useItemAtSlot(int32 slotNumber);

};

