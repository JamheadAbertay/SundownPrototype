#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "BirdPawn.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API ABirdPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABirdPawn();

	// Spline reference variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spline)
		TSubclassOf<class AActor>  SplineClassType;
	UStaticMeshComponent* SplineBounds; // the spline bounds

	// Camera components
	UPROPERTY(EditAnywhere)
		USpringArmComponent* mCameraSpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UCameraComponent* mCamera;

	// Collision cone
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* mCollisionCone;

	/** Spline movement bool, false by default */
	UPROPERTY(BlueprintReadWrite, Category = Spline)
		bool OnSpline = false;

	/** Boost bool for handling when to boost */
	UPROPERTY(BlueprintReadWrite)
		bool Boosting;

	/** Invert camera bool */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bInvertCamY;

protected:

	/** Bound to the control rotation pitch (camera too) */
	void PitchInput(float Val);
	float PitchAmount = 0.0f;
	/** Bound to the control rotation yaw (camera too) */
	void YawInput(float Val);
	float YawAmount = 0.0f;
	/** Bound to the boost ability */
	void BuildBoost();
	UFUNCTION(BlueprintCallable, Category = "Boost")
		void BoostReady();
	/** Bound to the faster turning button */
	void TurnFaster(float Val);
	
	// Begin AActor overrides
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End AActor overrides

	// Line trace function for collision
	void PerformLineTrace(); // called on tick

private:
	// FORWARD VECTOR FOR COLLISION AND FLYING
	FVector flyForwardVector;

	// SPLINE LAST LOCATION FOR CHECKING IF SPLINE IS FINISHED
	FVector LastLocation;

	// MOVEMENT FUNCTIONS / VARIABLES

	/** Calculate flight function */
	void CalculateFlight(float DeltaSeconds);
	/** Calculate spline movement function with overloaded direction function */
	void CalculateDirection(float DeltaSeconds);
	/** Calculate speed of the bird */
	void CalculateSpeed();
	/** Calculate the camera position */
	void CalculateCamera();
	/** Calculate the turn rate */
	void CalculateTurnRate();
	
	// TURNING FLOATS

	UPROPERTY(EditAnywhere, Category = Turning)
		float YawTurnRate = 15.0f;
	UPROPERTY(EditAnywhere, Category = Turning)
		float PitchTurnRate = 22.5f;
	UPROPERTY(EditAnywhere, Category = Turning)
		float MaxYawTurnRate = 25.0f;
	UPROPERTY(EditAnywhere, Category = Turning)
		float MaxPitchTurnRate = 25.0f;
	// For increasing turn rate based on input
	float TurnRateFloat = 0.0f;
	//
	float DefaultPitchRate;
	float DefaultYawRate;

	// FLIGHT FLOATS

	/** This is used when calculating the inclination of the character (then used for Z velocity) */
	float InclinationAmount;
	/** This is used to control the lift of the bird (force against gravity) */
	float LiftAmount;
	/** The force of gravity */
	UPROPERTY(EditDefaultsOnly, Category = Flight)
		float GravityConstant = -980.0f;

	// FLIGHT MULTIPLIER CURVES

	/** Flight Velocity Lift Multiplier Curve */
	UPROPERTY(EditAnywhere, Category = Flight)
		UCurveFloat* VelCurve;
	/** Flight Angle Lift Multiplier Curve */
	UPROPERTY(EditAnywhere, Category = Flight)
		UCurveFloat* AngCurve;

	// DEFAULT SPEEDS

	/** Speed to go to when boosting (and also the speed you accelerate to when diving) */
	UPROPERTY(EditAnywhere, Category = Boost)
		float MaxSpeed = 650.0f;
	/** Default speed variable */
	UPROPERTY(EditAnywhere, Category = MovementSpeed)
		float DefaultSpeed = 375.0f;

	// BOOSTING (FORWARD FLAP THING)

	/** Bool for knowing when player can boost again */
	bool bBoostReady = true;
	/** Amount to multiply MaxWalkSpeed by during boost each frame */
	UPROPERTY(EditAnywhere, Category = Boost)
		float BoostMultiplier = 1.035f;
	/** Amount to multiply MaxWalkSpeed by after boosting each frame until speed is back to normal */
	UPROPERTY(EditAnywhere, Category = Boost)
		float SlowdownMultiplier = 0.998f;
	/** Time to delay between boosts (in seconds) */
	UPROPERTY(EditAnywhere, Category = Boost)
		float BoostDelaySeconds = 0.875f;

	// CAMERA MANIPULATION (DIVE)
public:
	UPROPERTY(BlueprintReadWrite, Category = Camera)
		float DefaultSpringArmLength = 100.0f;
	UPROPERTY(BlueprintReadWrite, Category = Camera)
		float DiveSpringArmLength = 60.0f;
private:
	UPROPERTY(EditAnywhere, Category = Camera)
		float DiveCameraInterpSpeed = 0.75f;

	// CAMERA MANIPULATION (CLAMP Y AXIS)
	UPROPERTY(EditAnywhere, Category = Camera)
		float MaxYRot = 45.0f;
	UPROPERTY(EditAnywhere, Category = Camera)
		float MinYRot = 300.0f;

	float YCamMultiplier = 1.0f;
	
public:
	UPROPERTY(BlueprintReadWrite)
		float DiveRangeClamped = 100.0f;

private:
	/** For using delay to create cooldowns */
	FLatentActionInfo BoostLTI;

protected:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
