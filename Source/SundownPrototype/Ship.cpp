// Fill out your copyright notice in the Description page of Project Settings.

#include "Ship.h"
#include "Engine.h"
#include "UObject/ConstructorHelpers.h"


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
	SailMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sail mesh"));
	SailMesh->SetupAttachment(ShipMesh);
	/*static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("SkeletalMesh'/Game/BlueprintMechanics/SK_Sail.SK_Sail'"));
	if (SkeletalMeshAsset.Succeeded()) {
		SailMesh->SetSkeletalMesh(SkeletalMeshAsset.Object);
		SailMesh->SetWorldScale3D(FVector(1.0f));
		SailMesh->SetMobility(EComponentMobility::Movable);
		SailMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SailMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
		SailMesh->bVisible = true;
		SailMesh->bCastDynamicShadow = true;
	}*/

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
	DynMaterial->SetScalarParameterValue("Amount", 0.0f);
	DynMaterial->SetScalarParameterValue("Boost", 1.406528f);
	DynMaterial->SetScalarParameterValue("Width", 0.04762f);

	//
	SailMesh->SetMaterial(0, DynMaterial);
}

// Called every frame
void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFired) {
		fFireAmount = FMath::FInterpTo(fFireAmount, 1.0f, DeltaTime, 0.2f);
		DynMaterial->SetScalarParameterValue("Amount", fFireAmount);
	}

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
	bFired = true;
	FVector CinderLocation = OtherActor->GetActorLocation();
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Sail on fire at: %s"), *CinderLocation.ToString()));
}