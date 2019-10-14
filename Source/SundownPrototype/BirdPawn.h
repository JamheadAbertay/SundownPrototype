#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BirdPawn.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API ABirdPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABirdPawn();


	// Camera components
	UPROPERTY(EditAnywhere)
		USpringArmComponent* mCameraSpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UCameraComponent* mCamera;

	// Collision cone component
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* smCollisionConeUp;
	// Collision cone component
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* smCollisionConeLeft;
	// Collision cone component
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* smCollisionConeRight;
	// Collision cone component
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* smCollisionConeDown;

	/** Boost bool for handling when to boost */
	UPROPERTY(BlueprintReadWrite)
		bool Boosting;

	/** Invert camera bool */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bInvertCamY;

	//
	UFUNCTION()
		void OnOverlapUp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnEndOverlapUp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//
	UFUNCTION()
		void OnOverlapLeft(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnEndOverlapLeft(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//
	UFUNCTION()
		void OnOverlapRight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnEndOverlapRight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//
	UFUNCTION()
		void OnOverlapDown(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnEndOverlapDown(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:

	/** Bound to the control rotation pitch (camera too) */
	void PitchInput(float Val);
	float PitchAmount = 0.0f;
	/** Bound to the control rotation yaw (camera too) */
	void YawInput(float Val);
	float YawAmount = 0.0f;
	/** Speed controls */
	void SpeedUp();
	void SlowDown();
	void Stop();

	// Begin AActor overrides
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End AActor overrides

public:

	/** Collision bools */
	bool bConeUp = false;
	bool bConeRight = false;
	bool bConeLeft = false;
	bool bConeDown = false;

	/** Calculate flight function */
	void CalculateFlight(float DeltaTime);
	/** Calculate spline movement function with overloaded direction function */
	void CalculateDirection(float DeltaTime);
	/** Calculate the camera position */
	void CalculateCamera();
	// Line trace function for collision
	//void PerformLineTrace();
	/** Update the collision cone state */
	void CheckConeColliders();

	// Movement component reference
	UCharacterMovementComponent* cMoveCompRef;
	APlayerController* cControllerRef;
	
	// Turning rates
	UPROPERTY(EditAnywhere, Category = Turning)
		float YawTurnRate = 15.0f;
	UPROPERTY(EditAnywhere, Category = Turning)
		float PitchTurnRate = 22.5f;

	/** Default max speed */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flight)
	//float fDefaultFlightSpeed = 3600.0f;

	// Flight floats
	/** This is used when calculating the inclination of the character (then used for Z velocity) */
	float fInclination;
	/** This is used to control the lift of the bird (force against gravity) */
	float fLiftAmount;
	/** Used for maintaining momentum */
	float fMomentumAmount = 1.0f;
	/** To fix the bayblading issue */
	float fTurnDotP;
	/** The force of gravity */
	UPROPERTY(EditDefaultsOnly, Category = Flight)
		float GravityConstant = -980.0f;

	// Flight curves
	// Curves for translating velocity into character "sway"
	UCurveFloat* xVelCurve;
	UCurveFloat* yVelCurve;
	// Curve for fixing the offset if stick remains idle (sway correction)
	UCurveFloat* offsetCurve;
	/** Flight Velocity Lift Multiplier Curve */
	UPROPERTY(EditAnywhere)
		UCurveFloat* VelCurve;
	/** Flight Angle Lift Multiplier Curve */
	UPROPERTY(EditAnywhere)
		UCurveFloat* AngCurve;

	float fAcceleration = 0.25f;
	float fTurnAcceleration = 1.0f;
	float fTurnSmoothingMult = 1.0f;

public:
	// Camera manipulation (dive)
	UPROPERTY(BlueprintReadWrite, Category = Camera)
		float DefaultSpringArmLength = 50.0f;
	UPROPERTY(BlueprintReadWrite, Category = Camera)
		float DiveSpringArmLength = 30.0f;
	
	// For storing the 
	UPROPERTY(BlueprintReadOnly)
		float fDiveCamClamped;
	// 
	UPROPERTY(BlueprintReadOnly)
		float fFovRangeClamped;

private:
	UPROPERTY(EditAnywhere, Category = Camera)
		float DiveCameraInterpSpeed = 1.0f;

	// Used for invert-Y
	float YCamMultiplier = 1.0f;

protected:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY()
		UTimelineComponent* MyTimeline;

	UPROPERTY()
		UCurveFloat* FloatCurve;

	UFUNCTION()
		void TimelineCallback(float val);

	UFUNCTION()
		void TimelineFinishedCallback();

		void PlayTimeline();
};