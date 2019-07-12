// Fill out your copyright notice in the Description page of Project Settings.

#include "Ship.h"
#include "Engine.h"


// Sets default values
AShip::AShip()
{
	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &AShip::OnOverlapBegin);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	//
	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ship mesh"));
	ShipMesh->SetupAttachment(RootComponent);

	//
	SailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sail mesh"));
	SailMesh->SetupAttachment(ShipMesh);

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
	DynMaterial = UMaterialInstanceDynamic::Create(SailMaterial, this);
	// Set parameters
	DynMaterial->SetScalarParameterValue("Fire amount", 0.0f);

	//
	SailMesh->SetMaterial(0, DynMaterial);
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

//Called when character overlaps with collision box
void AShip::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	DynMaterial->SetScalarParameterValue("Fire amount", 1.0f);
	FVector CinderLocation = OtherActor->GetActorLocation();
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Sail on fire at: %s"), *CinderLocation.ToString()));
}