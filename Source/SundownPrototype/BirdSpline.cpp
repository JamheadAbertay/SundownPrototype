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
	
	// Create mesh used to trigger spline
	StartCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collision Mesh"));
	StartCylinder->SetupAttachment(RootComponent);

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

	MomentTransitionParams.BlendExp = 1.5f;
	MomentTransitionParams.BlendFunction = EViewTargetBlendFunction::VTBlend_EaseInOut;
	MomentTransitionParams.BlendTime = 1.5f;
	MomentTransitionParams.bLockOutgoing = false;
}

void ABirdSpline::BeginPlay() {
	Super::BeginPlay();

	CinderControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (CinderControllerRef) {
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Controller found!")));
	}

	if (MomentCam) {
		MomentCam->SetActorLocation(CameraSpline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World));
	}
}

void ABirdSpline::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if ((SplineStarted) && (SplineDistance < MovementSpline->GetDistanceAlongSplineAtSplinePoint(MovementSpline->GetNumberOfSplinePoints() - 1) && (Cinder))) {
		// temporary spline progress float - need a smoother way of doing this
		SplineDistance += SplineSpeed;

		// store rotation and location on spline at this distance along it
		FRotator SplineRotation = MovementSpline->GetRotationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
		FVector SplineLocation = MovementSpline->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
		// get up vector for working out inclination of the character
		FVector SplineUpVector = UKismetMathLibrary::GetUpVector(SplineRotation);

		// control inclination ranges from -1 to 1 based on the rotational difference between spline up vector and actor forward vector (clamped to avoid total vertical up/down)
		float SplineInclinationAmount = FVector::DotProduct(SplineUpVector, GetActorForwardVector());
		SplineInclinationAmount = FMath::Clamp(SplineInclinationAmount, -0.95f, 0.95f);

		// face the bird in the correct direction by changing the control rotation
		CinderControllerRef->SetControlRotation(SplineRotation);

		// calculation to work out an appropriate velocity for following the spline
		FVector CurrentVelocity = CinderMoveCompRef->Velocity;
		FVector FollowVelocity = SplineLocation - Cinder->GetActorLocation();
		FVector NewVelocity = CurrentVelocity + FollowVelocity;

		// using the movement component, set the velocity of cinder to NewVelocity - which is velocity in the direction of the splineff
		Cinder->GetMovementComponent()->Velocity = NewVelocity;
		Cinder->SetActorRotation(SplineRotation);

		// CAMERA
		FVector CameraLocation = CameraSpline->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
		MomentCam->SetActorLocation(CameraLocation);
		}
	else {
		// else, in other words when the spline is finished
		CinderControllerRef->SetViewTarget(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), MomentTransitionParams);
		SplineStarted = false;

		// this code resets the spline
		SplineDistance = 0;
		StartCylinder->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		StartCylinder->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	}
}

void ABirdSpline::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only if spline bool is false will we consider collision with the starting cylinder
	if ((SplineStarted != true) && (Other->IsA(ACharacter::StaticClass()))) {
		CinderActor = Other;
		Cinder = Cast<ACharacter>(CinderActor);

		// If the bird is found, display debug message and then get the movement component of the bird
		if (Cinder) {
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Cinder found!")));
			CinderMoveCompRef = Cinder->GetCharacterMovement();
			CinderControllerRef->SetViewTarget(MomentCam, MomentTransitionParams);
		}

		// Initiate the spline sequence using a boolean
		SplineStarted = true;
		StartCylinder->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StartCylinder->SetCollisionProfileName(TEXT("OverlapAll"));
	}
}