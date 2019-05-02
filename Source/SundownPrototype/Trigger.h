#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/TriggerVolume.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Runtime/CinematicCamera/Public/CineCameraActor.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Trigger.generated.h"

UCLASS(Blueprintable)
class SUNDOWNPROTOTYPE_API ATrigger : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Constructor sets default values for this actor's properties
	ATrigger();

	//Brazier mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Brazier)
		UStaticMeshComponent* BrazierMesh;

	// The new camera
	UPROPERTY(EditAnywhere, Category = Camera)
		ACineCameraActor* BrazierCamera;

	// Collision box that will trigger sequence
	UPROPERTY(EditAnywhere, Category = Trigger)
		USphereComponent* CollisionSphere;

	// Fire particle effect
	UPROPERTY(EditAnywhere, Category = Brazier)
		UParticleSystemComponent* FireParticles;

	//Overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	//Function to make cinder sit on the brazier
	UFUNCTION()
		void SitOnBrazier();

	UFUNCTION()
		void LeaveBrazier();

	UPROPERTY(BlueprintReadWrite)
		bool inBrazierZone = false;

private:

	//Reference to cinder
	ACharacter* Cinder;

	// Reference to player controller
	APlayerController* CinderControllerRef;

	// Transition parameters
	FViewTargetTransitionParams BrazierTransition;

	//Level sequence player used to play fade out
	UPROPERTY()
		ULevelSequencePlayer* SequencePlayer;

	//Actor used to play fade out
	UPROPERTY()
		ALevelSequenceActor* SequenceActor;

	//Level sequence asset played when pawn enters trigger box, can be set in trigger box details 
	ULevelSequence* FadeOut;

	// Approach brazier timer handle
	UPROPERTY()
		FTimerHandle BeginBrazierTimer;

	// Leave brazier timer handle
	UPROPERTY()
		FTimerHandle LeaveBrazierTimer;

	//New location to set cinder to (location of brazier - Z value to hide Cinder underground)
	FVector NewLocation;

	//New rotation to set cinder to (rotation of brazier)
	UPROPERTY(EditAnywhere, Category = LeaveBrazierProperties)
		FRotator NewRotation;

	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, Category = WidgetToShow)
		TSubclassOf<class UUserWidget> Widget;

	// Variable to hold the widget After Creating it.
	UUserWidget* WidgetToShow;

	// To safely store acceleration of Cinder for after brazier
	float AccelerationStored;
};