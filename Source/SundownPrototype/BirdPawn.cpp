// Developed by Gary Whittle and Scott Douglas, based on Unreal's Flight Example Project

#include "BirdPawn.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "EngineGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "UnrealMath.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ABirdPawn::ABirdPawn()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	mCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("mCameraSpringArm"));
	mCameraSpringArm->SetupAttachment(RootComponent);
	mCameraSpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	mCameraSpringArm->bEnableCameraLag = true;
	mCameraSpringArm->TargetArmLength = DefaultSpringArmLength;

	// Create a follow camera
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

	// For changing turn rate
	DefaultPitchRate = PitchTurnRate;
	DefaultYawRate = YawTurnRate;

	// For boost timing
	BoostLTI.CallbackTarget = this;
	BoostLTI.ExecutionFunction = "BoostReady";
	BoostLTI.UUID = 123;
	BoostLTI.Linkage = 0;
}

void ABirdPawn::Tick(float DeltaSeconds)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("PitchTurnRate: %f"), PitchTurnRate));

	// Calculate the flight movement (Z velocity and forward speed)
	CalculateFlight(DeltaSeconds);
	// Calculate the direction of the bird (turning left/right is independent of the Z velocity and forward speed)
	CalculateDirection(DeltaSeconds);
	// Calculate the forward speed of the bird
	CalculateSpeed();
	// Calculate the camera's location
	CalculateCamera();
	// Calculate the turn rate of Cinder
	CalculateTurnRate();
	

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void ABirdPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Other->GetClass()->IsChildOf(SplineClassType) && !OnSpline) {
		OnSpline = true;
	}
	else if (Other->GetClass()->IsChildOf(SplineClassType) && OnSpline) {
		OnSpline = false;
	}
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
	GetCharacterMovement()->MaxWalkSpeed = FMath::Clamp(GetCharacterMovement()->MaxWalkSpeed, DefaultSpeed, MaxSpeed);
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

void ABirdPawn::CalculateSpeed() {
	// BOOST
	if (Boosting) {
		if (GetCharacterMovement()->MaxWalkSpeed < MaxSpeed) {
			GetCharacterMovement()->MaxWalkSpeed *= BoostMultiplier;
		}
	}
	// NOT BOOST
	else {
		if (GetCharacterMovement()->MaxWalkSpeed > DefaultSpeed) {
			GetCharacterMovement()->MaxWalkSpeed *= SlowdownMultiplier;
		}
	}
	GetCharacterMovement()->MaxWalkSpeed = FMath::Clamp(GetCharacterMovement()->MaxWalkSpeed, DefaultSpeed, MaxSpeed);
}

void ABirdPawn::CalculateCamera() {
	// First convert Z velocity value to be within the correct range
	FVector2D input = FVector2D(-375.0f, 0.0f);
	FVector2D output = FVector2D(DiveSpringArmLength, DefaultSpringArmLength);

	DiveRangeClamped = FMath::GetMappedRangeValueClamped(input, output, GetCharacterMovement()->Velocity.Z);

	mCameraSpringArm->TargetArmLength = UKismetMathLibrary::FInterpTo(mCameraSpringArm->TargetArmLength, DiveRangeClamped, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), DiveCameraInterpSpeed);
}

void ABirdPawn::CalculateTurnRate() {
	FVector2D input = FVector2D(0.0f, 1.0f);
	FVector2D yawOutput = FVector2D(DefaultYawRate, MaxYawTurnRate);
	FVector2D pitchOutput = FVector2D(DefaultPitchRate, MaxPitchTurnRate);

	YawTurnRate = FMath::GetMappedRangeValueClamped(input, yawOutput, TurnRateFloat);
	PitchTurnRate = FMath::GetMappedRangeValueClamped(input, pitchOutput, TurnRateFloat);
}

// Called to bind functionality to input
void ABirdPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("PitchInput", this, &ABirdPawn::PitchInput);
	PlayerInputComponent->BindAxis("YawInput", this, &ABirdPawn::YawInput);
	// Bind our boost action
	PlayerInputComponent->BindAction("BuildBoost", IE_Pressed, this, &ABirdPawn::BuildBoost);
	// Bind our turn faster function to the right trigger
	PlayerInputComponent->BindAxis("TurnFaster", this, &ABirdPawn::TurnFaster);
}

void ABirdPawn::PitchInput(float Val) {
	PitchAmount = UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * PitchTurnRate * Val;
	AddControllerPitchInput(PitchAmount);
}

void ABirdPawn::YawInput(float Val) {
	YawAmount = UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * YawTurnRate * Val;
	AddControllerYawInput(YawAmount);
}

void ABirdPawn::BuildBoost() {
	if (bBoostReady && !OnSpline) {
		Boosting = true;
		bBoostReady = false;
		UKismetSystemLibrary::Delay(GetWorld(), BoostDelaySeconds, BoostLTI);
	}
}

void ABirdPawn::BoostReady() {
	Boosting = false;
	bBoostReady = true;
}

void ABirdPawn::TurnFaster(float Val) {
	if (Val > 0) {
		TurnRateFloat += 0.1f;
	} else {
		TurnRateFloat = 0.0f;
	}
	TurnRateFloat = FMath::Clamp(TurnRateFloat, 0.0f, 1.0f);
}