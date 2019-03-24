#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
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
	USplineComponent* Spline; // the spline
	UStaticMeshComponent* SplineBounds; // the spline bounds
	
	// Camera components
	UPROPERTY(EditAnywhere)
		USpringArmComponent* mCameraSpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UCameraComponent* mCamera;

	/** Spline movement bool, false by default */
	UPROPERTY(BlueprintReadOnly, Category = Spline)
		bool OnSpline = false;

protected:

	/** Bound to the control rotation pitch (camera too) */
	void PitchInput(float Val);
	float PitchAmount = 0.0f;
	/** Bound to the control rotation yaw (camera too) */
	void YawInput(float Val);
	float YawAmount = 0.0f;

	// Begin AActor overrides
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End AActor overrides

private:

	// FLYING MOVEMENT
	/** Flight Velocity Lift Multiplier Curve */
	UPROPERTY(EditAnywhere, Category = Flight)
		UCurveFloat* VelCurve;

	/** Flight Angle Lift Multiplier Curve */
	UPROPERTY(EditAnywhere, Category = Flight)
		UCurveFloat* AngCurve;

	/** The force of gravity */
	UPROPERTY(EditDefaultsOnly, Category = Flight)
		float GravityConstant;

	// This is used when calculating the inclination of the character (then used for Z velocity)
	float InclinationAmount;
	// This is used to control the lift of the bird (force against gravity)
	float LiftAmount;
	// This is the flyspeedHold variable for handling movement (default value is 1.0f)
	float SpeedHoldAmount = 1.0f;

	// Calculate flight function
	void CalculateFlight(float DeltaSeconds);

	// Calculate spline movement function with overloaded direction function
	void CalculateDirection(float DeltaSeconds);

	// Overloaded function for spline movement (direction)
	void CalculateDirection(float DeltaSeconds, FVector SplineInterpLocation);

	// FVector used to store the last location of important actors
	FVector lastLocation;

	// Float for storing delta time
	float deltatime;

protected:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
