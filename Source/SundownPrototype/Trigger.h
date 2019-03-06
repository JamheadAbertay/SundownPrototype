#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "GameFramework/Actor.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
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

	/** Brazier mesh */
	UPROPERTY(EditAnywhere, Category = Brazier)
		UStaticMeshComponent* BrazierMesh;

	//Overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	//Function to make bird sit on the brazier
	UFUNCTION()
		void SitOnBrazier();

	//Function to open new level if being uused as an open level trigger
	UFUNCTION()
		void OpenNewLevel();

	//Reference to player
	ACharacter* Cinder;

	//Collision box that will trigger sequence
	UPROPERTY(EditAnywhere, Category = CollisionBox)
	UBoxComponent* CollisionBox;

	//New level to open if being used as an open level trigger
	UPROPERTY(EditAnywhere, Category = LevelToOpen)
		FName levelToOpen;

	//Level sequence player used to play fade out
	UPROPERTY()
		ULevelSequencePlayer* SequencePlayer;

	//Actor used to play fade out
	UPROPERTY()
		ALevelSequenceActor* SequenceActor;

	//Level sequence asset played when pawn enters trigger box, can be set in trigger box details 
	ULevelSequence* FadeOut;

	//Set location timer handle
	UPROPERTY()
		FTimerHandle TimerHandle;

	//New location to set the bird to (location of brazier)
	UPROPERTY()
		FVector NewLocation;

	//New rotation to set the bird to (rotation of brazier)
	UPROPERTY()
		FRotator NewRotation;
};