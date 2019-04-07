#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "GameFramework/Actor.h"
#include "Runtime/CinematicCamera/Public/CineCameraActor.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Engine/StaticMesh.h"
#include "Components/BoxComponent.h"
#include "Trigger.generated.h"

UCLASS(Blueprintable)
class SUNDOWNPROTOTYPE_API ATrigger : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// constructor sets default values for this actor's properties
	ATrigger();

	//Brazier mesh
	UPROPERTY(EditAnywhere, Category = Brazier)
		AActor* Brazier;

	// The new camera
	UPROPERTY(EditAnywhere, Category = Camera)
		ACineCameraActor* BrazierCamera;

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

	//Collision box that will trigger sequence
	UPROPERTY()
		UBoxComponent* CollisionBox;

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

	// To safely store acceleration of Cinder for after brazier
	float AccelerationStored;
};