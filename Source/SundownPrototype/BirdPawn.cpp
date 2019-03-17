// Developed by Gary Whittle and Scott Douglas, based on Unreal's Flight Example Project

#include "BirdPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "UnrealMath.h"
#include "Engine.h"
#include "EngineGlobals.h"
#include "EngineUtils.h"

// Sets default values
ABirdPawn::ABirdPawn()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	mCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("mCameraSpringArm"));
	mCameraSpringArm->SetupAttachment(RootComponent);
	mCameraSpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	mCameraSpringArm->bEnableCameraLag = false;

	//// Create a follow camera
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	mCamera->SetupAttachment(mCameraSpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	mCamera->bUsePawnControlRotation = true;
}

void ABirdPawn::BeginPlay()
{
	Super::BeginPlay();

	// Set relevant character movement properties
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	GetCharacterMovement()->AirControl = 1.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 2.0f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 90.0f);
	GetCharacterMovement()->MaxAcceleration = 3000.0f;
}

void ABirdPawn::Tick(float DeltaSeconds)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SpeedHoldAmount: %f"), SpeedHoldAmount));
	deltatime = DeltaSeconds;
	// Quick state machine for flight and spline movement
	if (OnSpline == false) {
		CalculateFlight(DeltaSeconds);
		CalculateDirection(DeltaSeconds);
	}
	else {
		if (lastLocation != SplineBounds->GetComponentLocation()) {
			CalculateDirection(DeltaSeconds, lastLocation);
			//CalculateFlight(DeltaSeconds);
			lastLocation = SplineBounds->GetComponentLocation();
		}
		else {
			OnSpline = false;
		}
	}
	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void ABirdPawn::CalculateFlight(float DeltaSeconds) 
{
	// Step 1: Calculate LiftAmount
	// A) Calculate inclination of Cinder using dot product of control up vector and actor forward vector 
	FVector controlUpVec = UKismetMathLibrary::GetUpVector(FRotator(GetControlRotation()));
	// control inclination ranges from -1 to 1 based on the rotational difference between camera up vector and actor forward vector (clamped to avoid total vertical up/down)
	InclinationAmount = FVector::DotProduct(controlUpVec, GetActorForwardVector()); 
	InclinationAmount = FMath::Clamp(InclinationAmount, -0.95f, 0.95f);
	// B) Get value from angle curve using Inclination Amount (takeaway 90 degrees to get the correct angle)
	float AngCurveVal = AngCurve->GetFloatValue(FMath::Acos(InclinationAmount) - 90.0f);
	// C) Get value from velocity curve
	FVector VelocityVec = GetCharacterMovement()->Velocity;
	// Clamp Z within appropriate velocity as this deal with downwards movement - change the negative value to allow higher "down" velocity amounts to influence LiftAmount
	VelocityVec.Z = FMath::Clamp<float>(VelocityVec.Z, -2000, 0); 
	float VelCurveVal = VelCurve->GetFloatValue(VelocityVec.Size());
	// E) Calculate lift normalized by multiplying the flight angle curve and velocity curve values
	LiftAmount = VelCurveVal * AngCurveVal;

	// Step 2: GRAVITY!
	// A) Create force against gravity
	float x = GetCharacterMovement()->Mass * GravityConstant * LiftAmount;
	FVector force = FVector(0.0f, 0.0f, x);
	// B) Add force
	GetCharacterMovement()->AddForce(force);

	// Step 3: Add movement input in the correct direction (based on SpeedHoldAmount and controlrotation forward vector)
	// A) Calculate ZRangeClamped (Z downwards velocity, clamped between some appropriate movement values)
	float ZRangeClamped;
	// First convert Z velocity value to be within the correct range
	FVector2D input = FVector2D(-500.0f, 0.0f);
	FVector2D output = FVector2D(2.0f, 1.0f);
	// mapRangeClamped represents the difference in the Z velocity of the character, but clamped within reasonable values to be used for SpeedHoldAmount)
	ZRangeClamped = FMath::GetMappedRangeValueClamped(input, output, GetCharacterMovement()->Velocity.Z);
	// B) FInterp SpeedHoldAmount towards mapRangeClamped
	SpeedHoldAmount = FMath::FInterpTo(SpeedHoldAmount, ZRangeClamped, DeltaSeconds, FMath::Abs(InclinationAmount) + 0.5f);
	// C) Get direction to fly in
	FVector flyForwardVector = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	// D) Add the movement input in the correct direction, using flyspeedHold as weighting
	AddMovementInput(flyForwardVector, SpeedHoldAmount);
}

void ABirdPawn::CalculateDirection(float DeltaSeconds) {
	// Calculate change in yaw rotation
	FVector XYVelocity = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0.0f); //XY Movement FVector
	FRotator XYRotation = UKismetMathLibrary::MakeRotationFromAxes(XYVelocity, UKismetMathLibrary::GetRightVector(GetControlRotation()), FVector(0.0f, 0.0f, 1.0f)); // Create rotator from control rotation and XY movement
	XYRotation = FRotator(0.0f, XYRotation.Yaw, 0.0f); // Create FRotator with just the Yaw
	SetActorRelativeRotation(XYRotation); // Set relative rotation - X and Z rotation won't change

	// Apply Z velocity to character (upwards/downwards movement) - Set Z velocity based on the InclinationAmount (character steepness)
	float ZVelocity = FMath::FInterpTo(GetCharacterMovement()->Velocity.Z, (InclinationAmount * -980 * FMath::Abs(InclinationAmount)), DeltaSeconds, 4);
	FVector newVel = FVector(0.0f, 0.0f, ZVelocity);
	GetCharacterMovement()->Velocity.SetComponentForAxis(EAxis::Z, newVel.Z);
}

void ABirdPawn::CalculateDirection(float DeltaSeconds, FVector SplineInterpLocation) {
	// Calculate change in yaw rotation
	//FVector SplineVector = SplineInterpLocation - GetActorLocation();
	//FVector XYVelocity = SplineInterpLocation - GetActorLocation(); //FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0.0f); //XY Movement FVector
	//AddMovementInput(XYVelocity, 1.0f);
	
	FRotator MeshCorrection = FRotator(0.0f, -90.0f, 00.0f);
	SetActorRotation(SplineBounds->GetComponentRotation() + MeshCorrection);
	SetActorLocation(SplineBounds->GetComponentLocation());
}

void ABirdPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Other->GetClass()->IsChildOf(SplineClassType)) {
		SplineBounds = Cast<UStaticMeshComponent>(Hit.GetComponent());
		lastLocation = SplineBounds->GetComponentLocation(); // Need to set this to avoid reference issue
		OnSpline = true;
	}
}

// Called to bind functionality to input
void ABirdPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("PitchInput", this, &ABirdPawn::PitchInput);
	PlayerInputComponent->BindAxis("YawInput", this, &ABirdPawn::YawInput);
}

void ABirdPawn::PitchInput(float Val) {
	PitchAmount = FMath::FInterpTo(PitchAmount, Val, deltatime, 2.5);
	AddControllerPitchInput(PitchAmount);
}

void ABirdPawn::YawInput(float Val) {
	YawAmount = FMath::FInterpTo(YawAmount, Val, deltatime, 4);
	AddControllerYawInput(YawAmount);
}