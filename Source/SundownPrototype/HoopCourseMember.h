// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "Components/PointLightComponent.h"
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
	UPROPERTY(EditAnywhere, Category = Hoop)
		UStaticMeshComponent* Hoop;

	//Glowing hoop inside main hoop
	UPROPERTY(EditAnywhere, Category = GlowingHoop)
		UStaticMeshComponent* GlowingHoop;

	//Collision box that will trigger sequence
	UPROPERTY(EditAnywhere, Category = Box)
		UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Category = HoopsLight)
		UPointLightComponent* HoopLight;

	//Next hoop actor to spawn when cinder flies through current hoop
	UPROPERTY(EditAnywhere, Category = HoopToLight)
		AHoopCourseMember* HoopToLight;

	//Sound to play when cinder flies through hoop
	UPROPERTY()
		USoundCue* HoopSound;

	//Whether this hoop is active on the course
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isHoopActive = false;
	
};
