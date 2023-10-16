// Fill out your copyright notice in the Description page of Project Settings.


#include "Hazards/Fire.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RokoProjectCharacter.h"

// Sets default values
AFire::AFire()
{

	// Capsule Component initialization 
	BurnArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Burn Area"));
	BurnArea->InitCapsuleSize(44.f, 60.f);
	BurnArea->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	BurnArea->SetupAttachment(GetRootComponent());

	// Mesh initialization
	FireMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fire Mesh"));
	FireMesh->SetupAttachment(BurnArea);
	FireMesh->SetSimulatePhysics(true);

	// Sound component initialization
	FireDmgSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Fire Damage Sound"));
	FireDmgSound->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AFire::BeginPlay()
{
	Super::BeginPlay();

	BurnArea->OnComponentBeginOverlap.AddDynamic(this, &AFire::OnBurnAreaBeginOverlap);

	if (FireParticle != nullptr) {

		UGameplayStatics::SpawnEmitterAttached(FireParticle, FireMesh);
	}

	
	
}



void AFire::OnBurnAreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Get reference of player character
	ARokoProjectCharacter* PlayerRef = Cast<ARokoProjectCharacter>(OtherActor);

	if (PlayerRef != nullptr) {

		// Give Damage and Play Sound FX
		PlayerRef->giveDamage(10.0f);

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Character Burned"));

		// Play the Burn Sound FX
		if (FireDmgSoundFX->IsValidLowLevel()) {

			FireDmgSound->SetSound(FireDmgSoundFX);
			FireDmgSound->Play();
		}
	}

}

// Called every frame
void AFire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

