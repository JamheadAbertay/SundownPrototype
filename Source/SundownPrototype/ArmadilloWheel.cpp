// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmadilloWheel.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

// Sets default values
AArmadilloWheel::AArmadilloWheel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
}

// Called when the game starts or when spawned
void AArmadilloWheel::BeginPlay()
{
	Super::BeginPlay();




}

// Called every frame
void AArmadilloWheel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 
	if (!WheelTurning && WheelSpeed > 0.0f) {
		WheelSpeed = FMath::FInterpTo(WheelSpeed, 0.0f, DeltaTime, 0.7f);
		TheArrow->AddLocalRotation(FRotator(0.0f, 0.0f, (WheelSpeed / 15)));
	}
}

void AArmadilloWheel::SpinWheel(float speed, UArrowComponent* RotationArrow, UArrowComponent* DotPArrow) 
{
	// Get vector to rotate wheel around
	FVector WheelDirection = RotationArrow->GetForwardVector();
	// Get vector to use for dot product calculations
	FVector DotPArrowDirection = DotPArrow->GetForwardVector();
	// Armadillo reference
	ACharacter* Armadillo = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	// Get armadillo forward vector for dot product calculations
	FVector ArmyDirection = Armadillo->GetActorForwardVector();

	// Initialize dot product float
	float dotp = FVector::DotProduct(DotPArrowDirection, ArmyDirection);
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("DotP: %f"), float(dotp)));

	// Wheel turning programming
	if (FMath::Abs(dotp) > 0.7f) { // Absolute value of dot product will only be over 0.7 when facing enough left/right
		if (dotp < 0.0f) { // If negative
			RotationArrow->AddLocalRotation(FRotator(0.0f, 0.0f, (speed / 15))); // Rotate positively
			if (!WheelTurning) {
				WheelTurning = true;
			}
			WheelSpeed = speed;
		}
		else if (dotp > 0.0f) { // If positive
			RotationArrow->AddLocalRotation(FRotator(0.0f, 0.0f, -(speed / 15))); // Rotate negatively
			if (!WheelTurning) {
				WheelTurning = true;
			}
			WheelSpeed = speed;
		}
	}
}