// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "HoopCourseMember.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API AHoopCourseMember : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHoopCourseMember();

	//Overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Hoop object
	UPROPERTY()
		UStaticMeshComponent* Hoop;

	//Collision box that will trigger sequence
	UPROPERTY()
		UBoxComponent* CollisionBox;

	//Next hoop actor to spawn when bird flies through current hoop
	UPROPERTY(EditAnywhere, Category = HoopToSpawn)
		AHoopCourseMember* HoopToSpawn;

	//Sound to play when bird flies through hoop
	UPROPERTY()
		USoundCue* HoopSound;

	//Sound to play when fire starts
	UPROPERTY()
		USoundCue* FireSound;

	//Fire particle system to spawn when bird flies through hoop
	UPROPERTY()
		UParticleSystemComponent* HoopFire;

	//Whether this hoop is active on the course
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isHoopActive = false;
	
};
