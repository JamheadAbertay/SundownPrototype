// Developed by Jamhead Games

#include "BirdPawn.h"
#include "DrawDebugHelpers.h"
#include "EngineGlobals.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealMath.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"

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

	// Create a collision cone mesh
	smCollisionConeUp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionConeUpRight"));
	smCollisionConeUp->SetupAttachment(mCamera);
	smCollisionConeUp->SetCollisionProfileName("OverlapAllDynamic");
	smCollisionConeUp->bGenerateOverlapEvents = true;
	// Create a collision cone mesh
	smCollisionConeLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionConeUpLeft"));
	smCollisionConeLeft->SetupAttachment(mCamera);
	smCollisionConeLeft->SetCollisionProfileName("OverlapAllDynamic");
	smCollisionConeLeft->bGenerateOverlapEvents = true;
	// Create a collision cone mesh
	smCollisionConeRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionConeDownRight"));
	smCollisionConeRight->SetupAttachment(mCamera);
	smCollisionConeRight->SetCollisionProfileName("OverlapAllDynamic");
	smCollisionConeRight->bGenerateOverlapEvents = true;
	// Create a collision cone mesh
	smCollisionConeDown = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionConeDownLeft"));
	smCollisionConeDown->SetupAttachment(mCamera);
	smCollisionConeDown->SetCollisionProfileName("OverlapAllDynamic");
	smCollisionConeDown->bGenerateOverlapEvents = true;

	// Curve float
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("CurveFloatCurveFloat'/Game/Characters/PlayerCharacter/Blueprints/BoostCurve.BoostCurve'"));
	check(Curve.Succeeded());

	FloatCurve = Curve.Object;
}

void ABirdPawn::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat onTimelineCallback;
	FOnTimelineEventStatic onTimelineFinishedCallback;

	if (FloatCurve != NULL)
	{
		MyTimeline = NewObject<UTimelineComponent>(this, FName("TimelineAnimation"));
		MyTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript; // Indicate it comes from a blueprint so it gets cleared when we rerun construction scripts
		this->BlueprintCreatedComponents.Add(MyTimeline); // Add to array so it gets saved
		MyTimeline->SetNetAddressable();	// This component has a stable name that can be referenced for replication

		MyTimeline->SetPropertySetObject(this); // Set which object the timeline should drive properties on
		MyTimeline->SetDirectionPropertyName(FName("TimelineDirection"));

		MyTimeline->SetLooping(false);
		MyTimeline->SetTimelineLength(1.5f);
		MyTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

		MyTimeline->SetPlaybackPosition(0.0f, false);

		//Add the float curve to the timeline and connect it to your timelines's interpolation function
		onTimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
		onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
		MyTimeline->AddInterpFloat(FloatCurve, onTimelineCallback);
		MyTimeline->SetTimelineFinishedFunc(onTimelineFinishedCallback);

		MyTimeline->RegisterComponent();
	}

	// Set up references
	cMoveCompRef = GetCharacterMovement();
	cControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// Set relevant character movement properties
	cMoveCompRef->SetMovementMode(EMovementMode::MOVE_Falling);
	cMoveCompRef->AirControl = 1.0f;
	cMoveCompRef->BrakingFrictionFactor = 1.0f;
	cMoveCompRef->FallingLateralFriction = 1.0f;
	cMoveCompRef->RotationRate = FRotator(1.0f, 1.0f, 1.0f);
	cMoveCompRef->MaxAcceleration = 600.0f;
	cMoveCompRef->MaxWalkSpeed = fDefaultFlightSpeed;

	// Invert-Y
	if (bInvertCamY) {
		YCamMultiplier = -1.0f;
	}
	else if (!bInvertCamY) {
		YCamMultiplier = 1.0f;
	}

	// Collision cone overlap begin
	smCollisionConeUp->OnComponentBeginOverlap.AddDynamic(this, &ABirdPawn::OnOverlapUp);
	// End
	smCollisionConeUp->OnComponentEndOverlap.AddDynamic(this, &ABirdPawn::OnEndOverlapUp);

	// Collision cone overlap begin
	smCollisionConeLeft->OnComponentBeginOverlap.AddDynamic(this, &ABirdPawn::OnOverlapLeft);
	// End
	smCollisionConeLeft->OnComponentEndOverlap.AddDynamic(this, &ABirdPawn::OnEndOverlapLeft);

	// Collision cone overlap begin
	smCollisionConeRight->OnComponentBeginOverlap.AddDynamic(this, &ABirdPawn::OnOverlapRight);
	// End
	smCollisionConeRight->OnComponentEndOverlap.AddDynamic(this, &ABirdPawn::OnEndOverlapRight);

	// Collision cone overlap begin
	smCollisionConeDown->OnComponentBeginOverlap.AddDynamic(this, &ABirdPawn::OnOverlapDown);
	// End
	smCollisionConeDown->OnComponentEndOverlap.AddDynamic(this, &ABirdPawn::OnEndOverlapDown);


}

void ABirdPawn::Tick(float DeltaTime)
{
	

	// Call any parent class Tick implementation
	Super::Tick(DeltaTime);

	// Calculate the flight movement (Z velocity and forward speed)
	CalculateFlight(DeltaTime);
	// Calculate the direction of the bird (turning left/right is independent of the Z velocity and forward speed)
	CalculateDirection(DeltaTime);
	// Calculate the camera's location
	CalculateCamera();
	// Update cone collision for crash avoidance
	CheckConeColliders();
}


//void ABirdPawn::PerformLineTrace() {
//	FHitResult OutHit;
//	FVector Start = GetActorLocation();
//
//	FVector End = ((UKismetMathLibrary::GetForwardVector(GetControlRotation()) * 1000.0f) + Start);
//	FCollisionQueryParams CollisionParams;
//
//	CollisionParams.AddIgnoredActor(this);
//	//DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1.0, 0, 1);
//
//	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
//	{
//		if (OutHit.bBlockingHit)
//		{
//			APlayerController* CinderController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
//
//			if (OutHit.ImpactNormal.Z > 0.0f) {
//				FRotator NewRotation = FRotator(GetControlRotation().Pitch + 45.0f, GetControlRotation().Yaw, GetControlRotation().Roll);
//				CinderController->SetControlRotation(UKismetMathLibrary::RInterpTo(GetControlRotation(), NewRotation, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 1.0f));
//			}
//		}
//	}
//}

void ABirdPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

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

	// Using the two vectors, create a value from -1 to 1 by getting the dot product (-1 = upwards, 1 = downwards)
	fInclination = FVector::DotProduct(vControlUpward, GetActorForwardVector());


	// Fetch value from angle/curve graph
	float AngCurveVal = AngCurve->GetFloatValue(FMath::Acos(fInclination));

	// Clamp Z velocity within appropriate velocity as this deal with downwards movement
	float fZVelRange = FMath::Clamp<float>(fZVel, -2000, 0);

	// Fetch value from velocity/curve graph
	float VelCurveVal = VelCurve->GetFloatValue(fZVelRange);

	// Calculate lift normalized by multiplying the flight angle curve and velocity curve values
	fLiftAmount = VelCurveVal * AngCurveVal;
	////


	// Find resultant force against gravity based on character's mass, world gravity, and the amount of lift (fLiftAmount)
	float fGravityKryptonite = (cMoveCompRef->Mass * GravityConstant * fLiftAmount);
	FVector vUpForce = FVector(0.0f, 0.0f, fGravityKryptonite);
	if (FMath::Abs(fInclination) < 0.5f) {
		cMoveCompRef->AddForce(vUpForce);
	}
	////

	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Inclination: %f"), float(fInclination)));


	/*GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("fInclination: %f fLiftAmount: %f"), fInclination, fLiftAmount));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Movement input: %f"), float(fAcceleration + fLiftAmount)));*/

	// First convert Z velocity value to be within the correct range
	FVector2D input = FVector2D(-500.0f, 0.0f);
	FVector2D output = FVector2D(4.0f, 0.0f);
	// Get mapped value 
	float fZRangeClamped = FMath::GetMappedRangeValueClamped(input, output, fZVel);
	fMomentumAmount = FMath::FInterpTo(fMomentumAmount, fZRangeClamped, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), FMath::Abs(fInclination) * 0.5f);
	// Get direction to fly in
	FVector vControlForward = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	// Add movement input
	AddMovementInput(vControlForward, fAcceleration + fMomentumAmount);
	////
}

void ABirdPawn::CalculateDirection(float DeltaSeconds) {
	// Calculate change in yaw rotation
	FVector XYVelocity = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0.0f); //XY Movement FVector
	FRotator XYRotation = UKismetMathLibrary::MakeRotationFromAxes(XYVelocity, UKismetMathLibrary::GetRightVector(GetControlRotation()), FVector(0.0f, 0.0f, 1.0f)); // Create rotator from control rotation and XY movement
	XYRotation = FRotator(0.0f, XYRotation.Yaw, 0.0f); // Create FRotator with just the Yaw
	SetActorRelativeRotation(XYRotation); // Set relative rotation - X and Z rotation won't change

	// initialize float
	float ZVelocity;

	// Apply Z velocity to character (upwards/downwards movement) - Set Z velocity based on the InclinationAmount (character steepness)
	if (fInclination < 0.0f) {
		ZVelocity = FMath::FInterpTo(GetCharacterMovement()->Velocity.Z, (fInclination * GravityConstant * FMath::Abs(fInclination + 1.0)), UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 4) + fAcceleration +-fInclination * 200.0f;
	}
	else {
		ZVelocity = FMath::FInterpTo(GetCharacterMovement()->Velocity.Z, (fInclination * GravityConstant * FMath::Abs(fInclination)) * 3.0f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 4);
	}

	// Set the Z velocity
	cMoveCompRef->Velocity.SetComponentForAxis(EAxis::Z, ZVelocity);

	// Find the dot product between character forward axis and control forward to disallow abnormally tight/fast turning
	FRotator rControlRotation = UKismetMathLibrary::MakeRotationFromAxes(UKismetMathLibrary::GetForwardVector(GetControlRotation()), UKismetMathLibrary::GetRightVector(GetControlRotation()), FVector(0.0f, 0.0f, 1.0f)); // Create rotator from control rotation and XY movement
	rControlRotation = FRotator(0.0f, rControlRotation.Yaw, 0.0f); // Create FRotator with just the Yaw

	fTurnDotP = FVector::DotProduct(GetActorForwardVector(), UKismetMathLibrary::GetForwardVector(rControlRotation));

	// Create a mapped value to help stop turning stutter
	FVector2D input = FVector2D(0.50f, 1.0f);
	FVector2D output = FVector2D(0.0f, 1.0f);
	FVector2D input2 = FVector2D(0.55f, 0.50F);
	FVector2D output2 = FVector2D(1.0f, 0.0f);
					
	fTurnAcceleration = FMath::GetMappedRangeValueClamped(input, output, fTurnDotP);
	fTurnSmoothingMult = FMath::GetMappedRangeValueClamped(input2, output2, fTurnDotP);
}

void ABirdPawn::CalculateCamera() {
	// First convert Z velocity value to be within the correct range
	FVector2D input = FVector2D(-2000.0f, 0.0f);
	FVector2D output = FVector2D(DiveSpringArmLength, DefaultSpringArmLength); // target arm length
	FVector2D output2 = FVector2D(117.5f, 105.0f); // fov

	// Using -500.0f to 0.0, map value to correct dive camera distance
	fDiveCamClamped = FMath::GetMappedRangeValueClamped(input, output, GetCharacterMovement()->Velocity.Z);

	// Interp to camera distance
	mCameraSpringArm->TargetArmLength = UKismetMathLibrary::FInterpTo(mCameraSpringArm->TargetArmLength, fDiveCamClamped, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), DiveCameraInterpSpeed);

	////
	//if (fInclination > 0.0f) {
	//	mCameraSpringArm->TargetOffset = FMath::VInterpTo(mCameraSpringArm->TargetOffset, FVector(0.0f, 0.0f, fInclination * 5.0f), UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 1.0f);
	//}

	// 
	fFovRangeClamped = FMath::GetMappedRangeValueClamped(input, output2, GetCharacterMovement()->Velocity.Z);
	mCamera->SetFieldOfView(fFovRangeClamped);
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
	PlayerInputComponent->BindAction("BuildBoost", IE_Pressed, this, &ABirdPawn::PlayTimeline);
	// Bind speed controls
	PlayerInputComponent->BindAction("SpeedUp", IE_Repeat, this, &ABirdPawn::SpeedUp);
	PlayerInputComponent->BindAction("SlowDown", IE_Released, this, &ABirdPawn::SlowDown);
	PlayerInputComponent->BindAction("Stop", IE_Repeat, this, &ABirdPawn::Stop);

}

void ABirdPawn::TimelineCallback(float val)
{
	cMoveCompRef->AddImpulse(UKismetMathLibrary::UKismetMathLibrary::GetForwardVector(GetControlRotation()) * val);
}

void ABirdPawn::TimelineFinishedCallback()
{
	Boosting = false;
}

void ABirdPawn::PlayTimeline()
{
	if (MyTimeline != NULL && !MyTimeline->IsActive())
	{
		MyTimeline->PlayFromStart();
		Boosting = true;
	}
}



void ABirdPawn::PitchInput(float Val) {
	PitchAmount = UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * PitchTurnRate * Val;
	AddControllerPitchInput(PitchAmount * -YCamMultiplier);
}

void ABirdPawn::YawInput(float Val) {
	YawAmount = UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * YawTurnRate * Val;
	AddControllerYawInput(YawAmount);

	if (fTurnDotP > 0.50f) {
		AddMovementInput(UKismetMathLibrary::GetRightVector(GetControlRotation()), fTurnAcceleration * fTurnSmoothingMult * Val, true);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Dot product: %f"), float(fTurnDotP)));
	}
}

void ABirdPawn::SpeedUp() {
	fAcceleration = 2.0f;
}

void ABirdPawn::SlowDown() {
	fAcceleration = 0.5f;
}

void ABirdPawn::Stop() {
	fAcceleration = 0.0f;
}

void ABirdPawn::OnOverlapUp(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Cone up found!")));
	bConeUp = true;
}

void ABirdPawn::OnEndOverlapUp(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	bConeUp = false;
}

void ABirdPawn::OnOverlapLeft(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Cone left found!")));
	bConeLeft = true;
	
}

void ABirdPawn::OnEndOverlapLeft(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	bConeLeft = false;
}

void ABirdPawn::OnOverlapRight(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Cone right found!")));
	bConeRight = true;
	
}

void ABirdPawn::OnEndOverlapRight(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	bConeRight = false;
}

void ABirdPawn::OnOverlapDown(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Cone down found!")));
	bConeDown = true;
	
}

void ABirdPawn::OnEndOverlapDown(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	bConeDown = false;
}

void ABirdPawn::CheckConeColliders() {
	if (bConeUp) {
		FRotator NewRotation = FRotator(GetControlRotation().Pitch - 1.0f, GetControlRotation().Yaw, GetControlRotation().Roll);
		cControllerRef->SetControlRotation(UKismetMathLibrary::RInterpTo(GetControlRotation(), NewRotation, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 10.0f));
	}
	if (bConeLeft) {
		FRotator NewRotation = FRotator(GetControlRotation().Pitch, GetControlRotation().Yaw + 1.0f, GetControlRotation().Roll);
		cControllerRef->SetControlRotation(UKismetMathLibrary::RInterpTo(GetControlRotation(), NewRotation, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 10.0f));
	}
	if (bConeRight) {
		FRotator NewRotation = FRotator(GetControlRotation().Pitch, GetControlRotation().Yaw - 1.0f, GetControlRotation().Roll);
		cControllerRef->SetControlRotation(UKismetMathLibrary::RInterpTo(GetControlRotation(), NewRotation, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 10.0f));
	}
	if (bConeDown) {
		FRotator NewRotation = FRotator(GetControlRotation().Pitch + 5.0f, GetControlRotation().Yaw, GetControlRotation().Roll);
		cControllerRef->SetControlRotation(UKismetMathLibrary::RInterpTo(GetControlRotation(), NewRotation, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 10.0f));
	}
}