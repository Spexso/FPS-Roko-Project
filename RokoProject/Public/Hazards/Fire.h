// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Actor.h"
#include "Fire.generated.h"

UCLASS()
class ROKOPROJECT_API AFire : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Capsule Component for overlap, Set in Editor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Burn Damage Area")
	UCapsuleComponent* BurnArea;

	// Mesh variable, Set in Editor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* FireMesh;

	// Fire Particle Effect, Set in Editor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particle Effect")
	UParticleSystem* FireParticle;

	// Fire Damage Sound Effect, Set in Editor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound Effect")
	USoundCue* FireDmgSoundFX;

	// Helper variable for playing Sound Cue
	UAudioComponent* FireDmgSound;

	UFUNCTION()
	void OnBurnAreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
