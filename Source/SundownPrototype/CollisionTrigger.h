// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "CollisionTrigger.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API ACollisionTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollisionTrigger();

	//Overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
		void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//Reference to cinder
	ACharacter * Cinder;
	// Reference to player controller
	APlayerController* CinderControllerRef;
	//Collision box that will trigger sequence
	UPROPERTY()
		UBoxComponent* CollisionBox;
	// If player is close to ground
	bool bCloseToGround = false;

	float EnterZVal;

};