// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Matinee/MatineeActor.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "MatineeTrigger.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API AMatineeTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMatineeTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	//Collision box that will trigger matinee
	UPROPERTY()
		UBoxComponent* CollisionBox;

	//Level sequence player used to play fade out
	UPROPERTY()
		ULevelSequencePlayer* SequencePlayer;

	//Actor used to play fade out
	UPROPERTY()
		ALevelSequenceActor* SequenceActor;

	//Level sequence asset played when pawn enters trigger box, can be set in trigger box details 
	UPROPERTY(EditAnywhere, Category = SequenceToPlay)
	    ULevelSequence* SequenceToPlay;
};
