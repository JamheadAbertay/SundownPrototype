// BookSpline class for the narrative slides

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "BookSpline.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API ABookSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABookSpline();

	// The book follows this spline
	UPROPERTY(EditAnywhere)
		USplineComponent* StorySpline;

	// THe book mesh 
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* BookMesh;

	// The book particle
	UPROPERTY(EditAnywhere)
		UParticleSystemComponent* BookParticle;

	// The book trigger
	UPROPERTY(EditAnywhere)
		UBoxComponent* BookTrigger;

	//Overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

private:

	// StorySpline Framework
	void CheckStoryPoint(); // Update next point
	int CurrentStoryPoint; // Current story point
	//
	UPROPERTY(EditAnywhere)
		int FirstStoryPoint = 0;
	UPROPERTY(EditAnywhere)
		int SecondStoryPoint;
	UPROPERTY(EditAnywhere)
		int ThirdStoryPoint;
	//
	UPROPERTY(EditAnywhere)
		int SplineSpeed = 10;
	float SplineDistance = 0;
	//
	bool bBookMoving = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	
};
