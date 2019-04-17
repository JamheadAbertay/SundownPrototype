// Sundown Movement Spline

/** 
TO-DO...

PART 1

Create the spline itself (the spline shape) (DONE)
Create tools so that designers can place/edit points on the spline (DONE)
Get bird to follow spline, experiment with changing bird speed (DONE)
Setup a cinematic camera system for the spline (DONE)
Setup a movement bounds system and integrate into the movement spline w/ speed control (UNFINISHED)

**/

#include "BirdSpline.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "UnrealMath.h"

// Sets default values
ABirdSpline::ABirdSpline()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Create spline component
	MovementSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MovementSpline"));
	MovementSpline->SetupAttachment(RootComponent);

	// Camera Spline
	CameraSpline = CreateDefaultSubobject<USplineComponent>(TEXT("CameraSpline"));
	CameraSpline->SetupAttachment(RootComponent);
	
	// Create mesh used to trigger spline attach/detach
	StartCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collision Mesh"));
	StartCylinder->SetupAttachment(RootComponent);

	// Setup the mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("StaticMesh'/Engine/EngineMeshes/Cylinder.Cylinder'"));
	if (CylinderMeshAsset.Succeeded()) {
		StartCylinder->SetStaticMesh(CylinderMeshAsset.Object);
		StartCylinder->SetWorldScale3D(FVector(1.0f));
		StartCylinder->SetMobility(EComponentMobility::Movable);
		StartCylinder->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		StartCylinder->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
		StartCylinder->bVisible = true;
		StartCylinder->bCastDynamicShadow = false;
	}

	// Setup default camera transition paramaters
	MomentTransitionParams.BlendExp = 1.5f;
	MomentTransitionParams.BlendFunction = EViewTargetBlendFunction::VTBlend_EaseInOut;
	MomentTransitionParams.BlendTime = 1.5f;
	MomentTransitionParams.bLockOutgoing = false;
}

void ABirdSpline::BeginPlay() {
	Super::BeginPlay();

	// Get the player controller
	CinderControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

void ABirdSpline::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if ((SplineStarted) && (SplineDistance < MovementSpline->GetDistanceAlongSplineAtSplinePoint(MovementSpline->GetNumberOfSplinePoints() - 1) && (Cinder))) {
		// Temporary spline progress float - need a more enjoyable way of doing this
		SplineDistance += SplineSpeed;

		// Store rotation and location on spline at this distance along it
		FRotator SplineRotation = MovementSpline->GetRotationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
		FVector SplineLocation = MovementSpline->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);

		// Face the bird in the correct direction by changing the control rotation
		CinderControllerRef->SetControlRotation(SplineRotation);

		// Calculation to work out an appropriate velocity for following the spline
		FVector CurrentVelocity = CinderMoveCompRef->Velocity;
		FVector FollowVelocity = SplineLocation - Cinder->GetActorLocation();
		FVector NewVelocity = CurrentVelocity + FollowVelocity;

		// Using the movement component, set the velocity of cinder to NewVelocity - which is velocity in the direction of the next spline point
		Cinder->GetMovementComponent()->Velocity = NewVelocity;
		Cinder->SetActorRotation(SplineRotation);

		// Set camera to new point along camera spline
		FVector CameraLocation = CameraSpline->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
		MomentCam->SetActorLocation(CameraLocation);
	}
	else if (SplineStarted) { // In other words, after the spline has started, and after the above if condition is no longer being met
		
		// Set view target back to Cinder and set the "camera on camera spline?" bool to false (only once, which is what the if(bCameraOnSpline) is for)
		if (bCameraOnSpline) {
			CinderControllerRef->SetViewTarget(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), MomentTransitionParams);
			bCameraOnSpline = false;
		}

		// If this spline is repeatable - reset distance to 0, and set the start cylinder back to its default position
		if (bSplineRepeat) {
			SplineDistance = 0;
			StartCylinder->SetWorldLocation(MovementSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World));
		}
		// Else (if spline is not repeatable) - just disable collision on the cylinder asset
		else {
			StartCylinder->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			StartCylinder->SetCollisionProfileName(TEXT("OverlapAll"));
		}

		// Set the spline started bool to false so this code won't repeat
		SplineStarted = false;
	}
}

void ABirdSpline::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only if spline started bool is false will we consider collision with the starting cylinder
	if ((SplineStarted != true) && (Other->IsA(ACharacter::StaticClass()))) {
		CinderActor = Other;
		Cinder = Cast<ACharacter>(CinderActor);

		// If the bird is found, get the movement component, set the view target to the moment cam, and set the "camera on camera spline?" bool to true
		if (Cinder) {
			CinderMoveCompRef = Cinder->GetCharacterMovement();
			CinderControllerRef->SetViewTarget(MomentCam, MomentTransitionParams);
			bCameraOnSpline = true;
		}

		// Initiate the spline sequence using a boolean
		SplineStarted = true;
		// Place the moment cam at the start of this moment's camera spline
		MomentCam->SetActorLocation(CameraSpline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World));
		// Set the start cylinder location to the end of the spline to trigger Cinder's detachment (bool in BirdPawn class)
		StartCylinder->SetWorldLocation(MovementSpline->GetLocationAtSplinePoint(MovementSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World));
	}
}