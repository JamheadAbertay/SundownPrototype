// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "BirdSpline.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API ABirdSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABirdSpline();

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(EditAnywhere, Category = SplineSettings)
		USplineComponent* MovementSpline;

	/** Starting cylinder to attach to spline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SplineSettings)
		UStaticMeshComponent* StartCylinder;

	AActor* CinderActor; // cinder's actor reference
	ACharacter* Cinder; // cinder character functionality reference 
	UCharacterMovementComponent* CinderMoveCompRef; // cinder's movement component reference
	APlayerController* CinderControllerRef; // player controller reference

	UPROPERTY(BlueprintReadOnly, Category = SplineSettings)
		bool SplineStarted = false; // Whether spline is being travelled along based on collision with the StartCylinder

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SplineSettings)
		float SplineSpeed; // Speed at which to move along spline - note that this can be edited/viewed in blueprint too so level blueprint can access

	UPROPERTY(BlueprintReadOnly)
		float SplineDistance = 0.0f; // Distance along spline

protected:

	/** Override tick */
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds);
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	/** End overrides */
};
