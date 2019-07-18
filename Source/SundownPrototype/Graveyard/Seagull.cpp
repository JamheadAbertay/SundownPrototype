// Fill out your copyright notice in the Description page of Project Settings.

#include "Seagull.h"

// Sets default values
ASeagull::ASeagull()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASeagull::BeginPlay()
{
	Super::BeginPlay();
	
	//GetCharacterMovement()->MaxAcceleration = 400.0f;
	//GetCharacterMovement()->MaxWalkSpeed = 1200.0f;

	////centre of coordinate system
	//vC = GetActorLocation();
 
	////maybe random new direction
	//FRandomStream rStream;
	//vMove = rStream.GetUnitVector();

	//vMove.X = 0.6f;
	//vMove.Y = 0.3f;
	//vMove.Z = 0.0f;
	////clamps in between 2 values so the bird wouldn't go up and down like crazy
	//vMove.Z = FMath::Clamp(vMove.Z, 0.0f, 0.0f);
}

// Called every frame
void ASeagull::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Frame(DeltaTime);

}

void ASeagull::Frame(float DeltaTime)
{
	////getting the distance
	//XDistance = FMath::Abs(GetActorLocation().X - vC.X);
	//YDistance = FMath::Abs(GetActorLocation().Y - vC.Y);
	////ZDistance = FMath::Abs(GetActorLocation().Z - vC.Z);

	////if its out of bounds and hasnt turned yet
	//if ((XDistance > Hreach || YDistance > Hreach) && !bTurn)
	//{
	//	bTurn = true;
	//}

	//if (bTurn) 
	//{
	//	//gets the direction so that the bird can follow that direction
	//	//vTurn = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), vC);
	//	////turns seagull around
	//	//FVector2D XYRotate = FMath::Vector2DInterpTo(FVector2D(vMove.X, vMove.Y), FVector2D(vTurn.X, vTurn.Y), DeltaTime, 20.0f);
	//	//vMove = FVector(XYRotate.X, XYRotate.Y, 0.0f);
	//	//fRotatorSeagull = UKismetMathLibrary::MakeRotationFromAxes(GetCharacterMovement()->Velocity, GetActorRightVector(), GetActorUpVector());
	//	//SetActorRotation(fRotatorSeagull);
	//	if ((XDistance < Hreach && YDistance < Hreach))
	//	{
	//		bTurn = false;
	//	}
	//}

	//actually moves it on a screen
	//AddMovementInput(vMove, accelerationAmount);
}

//FVector ASeagull::ReturnDirection()
//{
//	return FVector();
//}


// Called to bind functionality to input
void ASeagull::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}