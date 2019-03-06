// Developed by Gary Whittle and Scott Douglas, based on Unreal's Flight Example Project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "BirdFlockMember.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API ABirdFlockMember: public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABirdFlockMember();

	// Soft collision sphere
	UPROPERTY(EditAnywhere, Category = Collision)
		UStaticMeshComponent* CollisionSphere;

	// Skeletal mesh for bird
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* BirdMesh;

protected:

	/** Bound to the movement bool */
	void Move();

	/** Bound to the thrust axis */
	/*void MoveForwardInput(float Val);*/

	/** Bound to the vertical axis */
	void MoveUpInput(float Val);

	/** Bound to the horizontal axis */
	void MoveRightInput(float Val);

	// Begin AActor overrides
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End AActor overrides

private:

	/** Move is set to false by default */
	bool move = false;

	/** How quickly forward speed changes */
	UPROPERTY(Category = Movement, EditAnywhere)
		float Acceleration;

	/** How quickly pawn can steer */
	UPROPERTY(Category = Movement, EditAnywhere)
		float TurnSpeed;

	/** Max forward speed */
	UPROPERTY(Category = Movement, EditAnywhere)
		float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category = Movement, EditAnywhere)
		float MinSpeed;

	/** Current forward speed */
	UPROPERTY(Category = Movement, EditAnywhere)
		float CurrentForwardSpeed;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current roll speed */
	float CurrentRollSpeed;

protected:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
