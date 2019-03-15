// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "GameFramework/Character.h"
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
	UPROPERTY(EditAnywhere, Category = hoop)
		UStaticMeshComponent* Hoop;

	//Collision box that will trigger sequence
	UPROPERTY()
		UBoxComponent* CollisionBox;

	//Fire particle system that will play when bird flies through hoop
	UParticleSystemComponent* OurParticleSystem;
	
};