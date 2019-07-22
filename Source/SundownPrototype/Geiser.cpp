// Fill out your copyright notice in the Description page of Project Settings.

#include "Geiser.h"
//#include "random"

// Sets default values
AGeiser::AGeiser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup MY mesh
	myGeiserMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MYmesh"));
	RootComponent = myGeiserMesh;

	// Set up MY box
	myBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("MYbox"));
	myBoxComponent->InitBoxExtent(FVector(100.0, 100.0, 100.0));
	myBoxComponent->SetCollisionProfileName("Trigger");
	myBoxComponent->SetupAttachment(RootComponent);

	//
	ImpulseDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("MYarrow"));

	// set up a notification for when this component overlaps something  
	myBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGeiser::OnOverlapBegin);
	// set up a notification for when this component stops overlapping  
	myBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AGeiser::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AGeiser::BeginPlay()
{
	Super::BeginPlay();
	
	

	GetWorldTimerManager().SetTimer(TimerHandleThing, this, &AGeiser::Spray, 1.0f, true, -1.0f);
}

// Called every frame
void AGeiser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGeiser::Spray()
{
	if (bArmadillo && MovementRef) 
	{
		MovementRef->AddImpulse(ImpulseDirection->GetForwardVector() * 5000.0f, true);
	}
}

void AGeiser::SetArmadilloRef(ACharacter * Army)
{
	ArmadilloRef = Army;
	MovementRef = ArmadilloRef->GetCharacterMovement();
}

void AGeiser::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && (OtherActor == ArmadilloRef))
	{
		// set the bool for activating bool spray
		bArmadillo = true;
	}
}

void AGeiser::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && (OtherActor == ArmadilloRef))
	{
		// set the bool for activating bool spray
		bArmadillo = false;
	}
}