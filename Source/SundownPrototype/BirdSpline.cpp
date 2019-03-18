// Sundown Movement Spline

/** 
TO-DO...

PART 1

Create the spline itself (the spline shape) (DONE)
Create tools so that designers can place/edit points on the spline (DONE)
Get bird to follow spline, experiment with changing bird speed (DONE)
Setup a cinematic camera system for the spline (WORKING ON WITH CAVAN IN BLUEPRINT)
Setup a movement bounds system and integrate into the movement spline w/ speed control (UNFINISHED)

**/

#include "BirdSpline.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values
ABirdSpline::ABirdSpline()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Create spline component
	MovementSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MovementSpline"));
	MovementSpline->SetupAttachment(RootComponent);
	
	StartCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collision Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("StaticMesh'/Engine/EngineMeshes/Cylinder.Cylinder'"));
	if (CylinderMeshAsset.Succeeded()) {
		StartCylinder->SetStaticMesh(CylinderMeshAsset.Object);
		StartCylinder->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		StartCylinder->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
		StartCylinder->SetWorldScale3D(FVector(1.0f));
		StartCylinder->SetMobility(EComponentMobility::Movable);
		StartCylinder->bVisible = true;
		StartCylinder->bCastDynamicShadow = false;
	}
}

void ABirdSpline::BeginPlay() {
	Super::BeginPlay();
}

void ABirdSpline::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if ((SplineStarted) && (SplineDistance < MovementSpline->GetDistanceAlongSplineAtSplinePoint(MovementSpline->GetNumberOfSplinePoints() - 1))) {
		SplineDistance += SplineSpeed;
		//UE_LOG(LogTemp, Warning, TEXT("Spline should be moving!"));
		StartCylinder->SetWorldLocation(MovementSpline->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World), false);
		StartCylinder->SetRelativeRotation(MovementSpline->GetRotationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World), false);
		}
	else {
		SplineStarted = false;

		/*SplineDistance = 0;
		StartCylinder->SetWorldLocation(MovementSpline->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World), false);
		StartCylinder->SetRelativeRotation(MovementSpline->GetRotationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World), false);
		StartCylinder->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		StartCylinder->SetCollisionProfileName(TEXT("BlockAllDynamic"));*/
	}
}

void ABirdSpline::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit){
	if ((SplineStarted != true) && (Other->IsA(ACharacter::StaticClass()))) {
		SplineStarted = true;
		StartCylinder->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StartCylinder->SetCollisionProfileName(TEXT("OverlapAll"));
	}
}