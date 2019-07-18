// Fill out your copyright notice in the Description page of Project Settings.

#include "Shine.h"



// Sets default values
AShine::AShine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	// Initialize the book
	ShinyThing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Diamond Mesh"));
	ShinyThing->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AShine::BeginPlay()
{
	Super::BeginPlay();

	//create dynamic material
	DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
	//set paramater with Set***ParamaterValue
	DynMaterial->SetScalarParameterValue("MyParameter", 0.0f);
	ShinyThing->SetMaterial(0, DynMaterial);
}

// Called every frame
void AShine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

