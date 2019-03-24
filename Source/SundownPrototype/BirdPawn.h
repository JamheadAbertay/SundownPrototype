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
	/** Bound to the boost ability */
	void BuildBoost();
	UFUNCTION(BlueprintCallable, Category = "Boost")
		void BoostReady();

	// Begin AActor overrides
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End AActor overrides

private:
	// MOVEMENT FUNCTIONS / VARIABLES
	/** Calculate flight function */
	void CalculateFlight(float DeltaSeconds);
	/** Calculate spline movement function with overloaded direction function */
	void CalculateDirection(float DeltaSeconds);
	//
	/** This is used when calculating the inclination of the character (then used for Z velocity) */
	float InclinationAmount;
	/** This is used to control the lift of the bird (force against gravity) */
	float LiftAmount;
	/** This is the variable for handling acceleration and if desired momentum (default value is 1.0f) */
	float SpeedHoldAmount = 1.0f;
	/** The force of gravity */
	UPROPERTY(EditDefaultsOnly, Category = Flight)
		float GravityConstant = 9.8f;
	/** Flight Velocity Lift Multiplier Curve */
	UPROPERTY(EditAnywhere, Category = Flight)
		UCurveFloat* VelCurve;
	/** Flight Angle Lift Multiplier Curve */
	UPROPERTY(EditAnywhere, Category = Flight)
		UCurveFloat* AngCurve;
	/** Boost bool for handling when to boost */
	bool Boosting;
	/** Speed to go to when boosting */
	UPROPERTY(EditAnywhere, Category = Boost)
		float BoostSpeed = 650.0f;
	/** Amount to multiply MaxWalkSpeed by during boost each frame */
	UPROPERTY(EditAnywhere, Category = Boost)
		float BoostMultiplier = 1.05f;
	/** Amount to multiply MaxWalkSpeed by after boosting each frame until speed is back to normal */
	UPROPERTY(EditAnywhere, Category = Boost)
		float SlowdownMultiplier = 0.998f;
	/** Time to delay between boosts (in seconds) */
	UPROPERTY(EditAnywhere, Category = Boost)
		float BoostDelaySeconds = 1.25f;
	/** For using delay to create a boost cooldown */
	FLatentActionInfo LatentActionInfo;

	// SPLINE FUNCTIONS / VARIABLES
	// Overloaded function for spline movement
	void CalculateDirection(float DeltaSeconds, FVector SplineInterpLocation);
	//
	// FVector used to store the last location of the spline movement mesh
	FVector lastLocation;

	// OTHER 
	// Float for storing delta time
	float deltatime;

protected:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
