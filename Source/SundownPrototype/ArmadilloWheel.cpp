// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmadilloWheel.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
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
	WheelMesh->SetupAttachment(RootComponent);

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

}

void AArmadilloWheel::SpinWheel(float speed) 
{
	FRotator MeshRotation = WheelMesh->GetComponentRotation();
	WheelMesh->SetWorldRotation(FRotator(MeshRotation.Pitch, MeshRotation.Yaw, MeshRotation.Roll + speed / 10));

}