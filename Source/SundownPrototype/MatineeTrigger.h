// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Matinee/MatineeActor.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
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

	//Matinee to play when bird enters trigger box
	UPROPERTY(EditDefaultsOnly, Category = MatineeToPlay) 
		AMatineeActor* MatineeToPlay;

	
	
};
