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

	// Create mesh used for the wheel
	WheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hamster wheel"));

	// Setup the mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WheelMeshAsset(TEXT("StaticMesh'/Game/BlueprintMechanics/HamsterWheel/wheelstandin.wheelstandin'"));
	if (WheelMeshAsset.Succeeded()) {
		WheelMesh->SetStaticMesh(WheelMeshAsset.Object);
		WheelMesh->SetWorldScale3D(FVector(1.0f));
		WheelMesh->SetMobility(EComponentMobility::Movable);
		WheelMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		WheelMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
		WheelMesh->bVisible = true;
		WheelMesh->bCastDynamicShadow = true;
	}
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

	if (!WheelTurning && WheelSpeed > 0.0f) {
		WheelSpeed = FMath::FInterpTo(WheelSpeed, 0.0f, DeltaTime, 0.7f);
		TheArrow->AddLocalRotation(FRotator(0.0f, 0.0f, (WheelSpeed / 15)));
	}
}

void AArmadilloWheel::SpinWheel(float speed, UArrowComponent* RotationArrow, UArrowComponent* DotPArrow) 
{
	FVector WheelDirection = RotationArrow->GetForwardVector();
	FVector DotPArrowDirection = DotPArrow->GetForwardVector();
	ACharacter* Armadillo = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FVector ArmyDirection = Armadillo->GetActorForwardVector();

	float dotp = FVector::DotProduct(DotPArrowDirection, ArmyDirection);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("DotP: %f"), float(dotp)));

	if (FMath::Abs(dotp) > 0.7f) {
		if (dotp < 0.0f) {
			RotationArrow->AddLocalRotation(FRotator(0.0f, 0.0f, (speed / 15)));
			if (!WheelTurning) {
				WheelTurning = true;
			}
		}
		else if (dotp > 0.0f) {
			RotationArrow->AddLocalRotation(FRotator(0.0f, 0.0f, -(speed / 15)));
			if (!WheelTurning) {
				WheelTurning = true;
			}
		}
	}
}