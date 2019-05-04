// BookSpline class for the narrative slides

#include "BookSpline.h"

// Sets default values
ABookSpline::ABookSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	// Initialize the spline for the book to follow
	StorySpline = CreateDefaultSubobject<USplineComponent>(TEXT("Story Spline"));
	StorySpline->SetupAttachment(RootComponent);

	// Initialize the book
	BookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Book Mesh"));
	BookMesh->SetupAttachment(RootComponent);

	// Initialize the particle effect
	BookParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Book Particles"));
	BookParticle->AttachTo(BookMesh);
	BookParticle->AttachToComponent(BookMesh, FAttachmentTransformRules::KeepRelativeTransform);

	// Initialize the trigger for the book
	BookTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Book Trigger"));
	BookTrigger->AttachTo(BookMesh);
	BookTrigger->AttachToComponent(BookMesh, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ABookSpline::BeginPlay()
{
	Super::BeginPlay();

	// Register Events
	OnActorBeginOverlap.AddDynamic(this, &ABookSpline::OnOverlapBegin);

	// Place book at the start
	BookMesh->SetWorldLocation(StorySpline->GetLocationAtSplinePoint(FirstStoryPoint, ESplineCoordinateSpace::World));
	BookMesh->SetWorldRotation(StorySpline->GetRotationAtSplinePoint(FirstStoryPoint, ESplineCoordinateSpace::World));

	// Set the first story point as current
	CurrentStoryPoint = FirstStoryPoint;
}

// Called every frame
void ABookSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bBookMoving) {
		// Increase distance along spline
		SplineDistance += SplineSpeed;

		// Check if ditance is past next point. If so, set it to the distance at that point
		if (SplineDistance > StorySpline->GetDistanceAlongSplineAtSplinePoint(CurrentStoryPoint)) {
			SplineDistance = StorySpline->GetDistanceAlongSplineAtSplinePoint(CurrentStoryPoint);
			bBookMoving = false; // And then set book moving to false
			CheckStoryPoint();
		}

		// Set the book mesh to be at the correct location and orientation
		BookMesh->SetWorldLocation(StorySpline->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World));
		BookMesh->SetWorldRotation(StorySpline->GetRotationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World));
	}
}

//Called when character overlaps with collision box
void ABookSpline::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor->IsA(ACharacter::StaticClass())))
	{
		bBookMoving = true;
	}
}

void ABookSpline::CheckStoryPoint() {
	if (CurrentStoryPoint == FirstStoryPoint) {
		CurrentStoryPoint = SecondStoryPoint; // Set current to the second story point
		bBookMoving = true; // The spline will need restarted upon triggering of the first story point
	}
	else if (CurrentStoryPoint == SecondStoryPoint) {
		CurrentStoryPoint = ThirdStoryPoint; // Then the third
	}
	else if (CurrentStoryPoint == ThirdStoryPoint) {
		CurrentStoryPoint = StorySpline->GetNumberOfSplinePoints() - 1; // Then the end of the spline, to hide it etc
	}
}