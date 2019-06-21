// Developed by Jamhead Games

#include "BirdPawn.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Controller.h"
#include "EngineGlobals.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	mCameraSpringArm->TargetArmLength = DefaultSpringArmLength;

	// Create a follow camera
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	mCamera->SetupAttachment(mCameraSpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	mCamera->bUsePawnControlRotation = true;
}

void ABirdPawn::BeginPlay()
{
	Super::BeginPlay();

	// Set up references
	cMoveCompRef = GetCharacterMovement();

	// Set relevant character movement properties
	cMoveCompRef->SetMovementMode(EMovementMode::MOVE_Falling);
	cMoveCompRef->AirControl = 1.0f;
	cMoveCompRef->BrakingFrictionFactor = 1.0f;
	cMoveCompRef->FallingLateralFriction = 1.0f;
	cMoveCompRef->RotationRate = FRotator(360.0f, 360.0f, 360.0f);
	cMoveCompRef->MaxAcceleration = 500.0f;
	cMoveCompRef->MaxWalkSpeed = 2000.0f;

	// For changing turn rate
	DefaultPitchRate = PitchTurnRate;
	DefaultYawRate = YawTurnRate;

	// For boost timing
	BoostLTI.CallbackTarget = this;
	BoostLTI.ExecutionFunction = "BoostReady";
	BoostLTI.UUID = 123;
	BoostLTI.Linkage = 0;

	// Invert-Y
	if (bInvertCamY) {
		YCamMultiplier = -1.0f;
	}
	else if (!bInvertCamY) {
		YCamMultiplier = 1.0f;
	}
}

void ABirdPawn::Tick(float DeltaTime)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("PitchTurnRate: %f"), PitchTurnRate));

	// Call any parent class Tick implementation
	Super::Tick(DeltaTime);

	// Calculate the flight movement (Z velocity and forward speed)
	CalculateFlight(DeltaTime);
	// Calculate the direction of the bird (turning left/right is independent of the Z velocity and forward speed)
	CalculateDirection(DeltaTime);
	// Calculate the forward speed of the bird
	CalculateSpeed();
	// Calculate the camera's location
	CalculateCamera();
	// Calculate the turn rate of Cinder
	CalculateTurnRate();
	//
	//PerformLineTrace();
}


void ABirdPawn::PerformLineTrace() {
	FHitResult OutHit;
	FVector Start = GetActorLocation();

	FVector End = ((UKismetMathLibrary::GetForwardVector(GetControlRotation()) * 1000.0f) + Start);
	FCollisionQueryParams CollisionParams;

	CollisionParams.AddIgnoredActor(this);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1.0, 0, 1);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		if (OutHit.bBlockingHit)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetActor()->GetName()));
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Impact Point: %s"), *OutHit.ImpactPoint.ToString()));
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Normal Point: %s"), *OutHit.ImpactNormal.ToString()));

			APlayerController* CinderController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

			if (OutHit.ImpactNormal.Z > 0.0f) {
				FRotator NewRotation = FRotator(GetControlRotation().Pitch + 45.0f, GetControlRotation().Yaw, GetControlRotation().Roll);
				CinderController->SetControlRotation(UKismetMathLibrary::RInterpTo(GetControlRotation(), NewRotation, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 1.0f));
			}
		}
	}
}

void ABirdPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Other->GetClass()->IsChildOf(SplineClassType) && !OnSpline) { // If hit spline and not on spline
		OnSpline = true;
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Spline found!")));
	}
	else if (Other->GetClass()->IsChildOf(SplineClassType) && OnSpline) { // If hit spline and on spine
		OnSpline = false;
		Other->SetActorEnableCollision(false);  // disable collision on spline
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Spline left!")));
	}
}

void ABirdPawn::CalculateFlight(float DeltaTime) 
{	
	Super::Tick(DeltaTime);


	// Compute velocity of character movement component and seperate the components
	FVector vVelocity = cMoveCompRef->Velocity;
	float fXVel = vVelocity.X;
	float fYVel = vVelocity.Y;
	float fZVel = vVelocity.Z;
	////


	// Get the forward vector of the actor 
	FVector vActorForward = GetActorForwardVector();
	// Get the forward vector of the control rotation
	FVector vControlUpward = UKismetMathLibrary::GetUpVector(FRotator(GetControlRotation()));

	// Using the two vectors, create a value from -1 to 1 by getting the dot product
	fInclination = FVector::DotProduct(vControlUpward, GetActorForwardVector());


	// Fetch value from angle/curve graph
	float AngCurveVal = AngCurve->GetFloatValue(FMath::Acos(fInclination));

	// Clamp Z velocity within appropriate velocity as this deal with downwards movement
	float fZVelRange = FMath::Clamp<float>(fZVel, -2000, 0);

	// Fetch value from velocity/curve graph
	float VelCurveVal = VelCurve->GetFloatValue(vVelocity.Size());

	// Calculate lift normalized by multiplying the flight angle curve and velocity curve values
	fLiftAmount = VelCurveVal * AngCurveVal;
	////


	// Find resultant force against gravity based on character's mass, world gravity, and the amount of lift (fLiftAmount)
	float fGravityKryptonite = (cMoveCompRef->Mass * -980.0f * fLiftAmount);
	FVector vUpForce = FVector(0.0f, 0.0f, -fGravityKryptonite);
	cMoveCompRef->AddForce(vUpForce);
	////


	// First convert Z velocity value to be within the correct range
	FVector2D input = FVector2D(-500.0f, 0.0f);
	FVector2D output = FVector2D(2000.0f, 0.0f);
	// Get mapped value 
	float fZRangeClamped = FMath::GetMappedRangeValueClamped(input, output, fZVel);
	// FInterp character speed towards max speed
	if (GetCharacterMovement()->MaxWalkSpeed < 2000.0f) {
		GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeed + fZRangeClamped, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), FMath::Abs(fInclination) + 0.5f);
	}
	GetCharacterMovement()->MaxWalkSpeed = FMath::Clamp(GetCharacterMovement()->MaxWalkSpeed, 2000.0f, 2000.0f);
	// Get direction to fly in
	FVector vControlForward = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	// Add movement input (1.0f = max acceleration)
	AddMovementInput(vControlForward, 1.0f);
	////
}

void ABirdPawn::CalculateDirection(float DeltaSeconds) {
	// Calculate change in yaw rotation
	FVector XYVelocity = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0.0f); //XY Movement FVector
	FRotator XYRotation = UKismetMathLibrary::MakeRotationFromAxes(XYVelocity, UKismetMathLibrary::GetRightVector(GetControlRotation()), FVector(0.0f, 0.0f, 1.0f)); // Create rotator from control rotation and XY movement
	XYRotation = FRotator(0.0f, XYRotation.Yaw, 0.0f); // Create FRotator with just the Yaw
	SetActorRelativeRotation(XYRotation); // Set relative rotation - X and Z rotation won't change

										  // Apply Z velocity to character (upwards/downwards movement) - Set Z velocity based on the InclinationAmount (character steepness)
	float ZVelocity = FMath::FInterpTo(GetCharacterMovement()->Velocity.Z, (fInclination * -980.0f * FMath::Abs(fInclination)), UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 4);
	cMoveCompRef->Velocity.SetComponentForAxis(EAxis::Z, ZVelocity);
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
	FVector2D input = FVector2D(-500.0f, 0.0f);
	FVector2D output = FVector2D(DiveSpringArmLength, DefaultSpringArmLength);

	// Using -500.0f to 0.0, map value to correct dive camera distance
	fDiveCamClamped = FMath::GetMappedRangeValueClamped(input, output, GetCharacterMovement()->Velocity.Z);

	// Interp to camera distance
	mCameraSpringArm->TargetArmLength = UKismetMathLibrary::FInterpTo(mCameraSpringArm->TargetArmLength, fDiveCamClamped, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), DiveCameraInterpSpeed);
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
	AddControllerPitchInput(PitchAmount * YCamMultiplier);
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