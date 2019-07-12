// Fill out your copyright notice in the Description page of Project Settings.

#include "Ship.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AShip::AShip()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	//
	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ship mesh"));
	ShipMesh->SetupAttachment(RootComponent);

	//
	ShipSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Ship spline"));
	ShipSpline->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AShip::BeginPlay()
{
	Super::BeginPlay();
	
	// Place ship at the start
	ShipMesh->SetWorldLocation(ShipSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World));
	ShipMesh->SetWorldRotation(ShipSpline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::World));


	// Create dynamic material instance
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(SailMaterial, this);
	// Set parameters
	DynMaterial->SetScalarParameterValue("Fire amount", fFireAmount);

	//
	ShipMesh->SetMaterial(0, DynMaterial);
}

// Called every frame
void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Increase distance along spline
	fSplineProgress += 5.0f;

	// Check if ditance is past next point. If so, set it to the distance at that point
	if (fSplineProgress > ShipSpline->GetDistanceAlongSplineAtSplinePoint(ShipSpline->GetNumberOfSplinePoints() - 1)) {
		fSplineProgress = 0.0f;
	}

	FRotator rCorrection = FRotator(0.0f, 90.0f, 0.0f);

	// Set the book mesh to be at the correct location and orientation
	ShipMesh->SetWorldLocation(ShipSpline->GetLocationAtDistanceAlongSpline(fSplineProgress, ESplineCoordinateSpace::World));
	ShipMesh->SetWorldRotation(ShipSpline->GetRotationAtDistanceAlongSpline(fSplineProgress, ESplineCoordinateSpace::World) + rCorrection);
}