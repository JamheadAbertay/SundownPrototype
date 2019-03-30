// Developed by Gary Whittle and Scott Douglas, based on Unreal's Flight Example Project

#include "BirdPawn.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "Engine.h"
#include "EngineGlobals.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "UnrealMath.h"
#include "UObject/ConstructorHelpers.h"

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
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
	GetCharacterMovement()->FallingLateralFriction = 1.0f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 90.0f);
	GetCharacterMovement()->MaxAcceleration = 3000.0f;
	GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;

	// For boost timing
	LatentActionInfo.CallbackTarget = this;
	LatentActionInfo.ExecutionFunction = "BoostReady";
	LatentActionInfo.UUID = 123;
	LatentActionInfo.Linkage = 0;
}

void ABirdPawn::Tick(float DeltaSeconds)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Speed: %f"), GetCharacterMovement()->MaxWalkSpeed));

	// Store deltatime for other functions
	deltatime = DeltaSeconds;
	
	// Calculate the flight movement (Z velocity and forward speed)
	CalculateFlight(DeltaSeconds);
	// Calculate the direction of the bird (turning left/right is independent of the Z velocity and forward speed)
	CalculateDirection(DeltaSeconds);
	// BOOST
	if (Boosting) {
		if (GetCharacterMovement()->MaxWalkSpeed < MaxSpeed * BoostSpeedMultiplier) {
			GetCharacterMovement()->MaxWalkSpeed *= BoostMultiplier;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
		}
	}
	// NOT BOOST
	else {
		if (GetCharacterMovement()->MaxWalkSpeed > DefaultSpeed) {
			GetCharacterMovement()->MaxWalkSpeed *= SlowdownMultiplier;
		}
		else if (GetCharacterMovement()->MaxWalkSpeed < DefaultSpeed) {
			GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;
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
	InclinationAmount = FMath::Clamp(InclinationAmount, -0.99f, 0.99f);
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
	FVector2D input = FVector2D(-250.0f, 0.0f);
	FVector2D output = FVector2D(DefaultSpeed, 0.0f);
	// mapRangeClamped represents the difference in the Z velocity of the character, but clamped within reasonable values to be used for SpeedHoldAmount)
	ZRangeClamped = FMath::GetMappedRangeValueClamped(input, output, GetCharacterMovement()->Velocity.Z);
	// B) FInterp SpeedHoldAmount towards mapRangeClamped
	if (GetCharacterMovement()->MaxWalkSpeed < MaxSpeed) {
		GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeed + ZRangeClamped, DeltaSeconds, FMath::Abs(InclinationAmount) + 0.5f);
	}
	// C) Get direction to fly in
	FVector flyForwardVector = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	// D) Add the movement input in the correct direction, using flyspeedHold as weighting
	AddMovementInput(flyForwardVector, 2.0f);
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

// Called to bind functionality to input
void ABirdPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("PitchInput", this, &ABirdPawn::PitchInput);
	PlayerInputComponent->BindAxis("YawInput", this, &ABirdPawn::YawInput);
	PlayerInputComponent->BindAction("BuildBoost", IE_Pressed, this, &ABirdPawn::BuildBoost);
}

void ABirdPawn::PitchInput(float Val) {
	PitchAmount = FMath::FInterpTo(PitchAmount, Val, deltatime, PitchInterpSpeed);
	AddControllerPitchInput(PitchAmount);
}

void ABirdPawn::YawInput(float Val) {
	YawAmount = FMath::FInterpTo(YawAmount, Val, deltatime, YawInterpSpeed);
	AddControllerYawInput(YawAmount);
}

void ABirdPawn::BuildBoost() {
	Boosting = true;
	UKismetSystemLibrary::Delay(GetWorld(), BoostDelaySeconds, LatentActionInfo);
}

void ABirdPawn::BoostReady() {
	Boosting = false;
}