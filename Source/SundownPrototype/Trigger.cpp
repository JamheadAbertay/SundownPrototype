#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT(text), fstring))

#include "Trigger.h"
// include draw debug helpers header file
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"

ATrigger::ATrigger()
{
	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &ATrigger::OnOverlapBegin);

	//Setup collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	CollisionBox->SetupAttachment(RootComponent);

	//Setup sequence
	static ConstructorHelpers::FObjectFinder<ULevelSequence> SequenceAsset(TEXT("LevelSequence'/Game/Sequences/FadeInFadeOut.FadeInFadeOut'"));
	if (SequenceAsset.Succeeded())
	{
		FadeOut = SequenceAsset.Object;
	}
}

// Called when the game starts or when spawned
void ATrigger::BeginPlay()
{
	Super::BeginPlay();
}

//Called when character overlaps with collision box
void ATrigger::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor->IsA(ACharacter::StaticClass())))
	{
		inBrazierZone = true;

		// Cast to Cinder
		Cinder = Cast<ACharacter>(OtherActor);

		// Deal with Cinder's acceleration
		float AccelerationStored = Cinder->GetCharacterMovement()->MaxAcceleration;
		Cinder->GetCharacterMovement()->MaxAcceleration = 0.0f;

		// Set the location of where to put Cinder (we are hiding him underground)
		NewLocation = FVector(Brazier->GetActorLocation().X, Brazier->GetActorLocation().Y, Brazier->GetActorLocation().Z - 10000);

		// Begin fade
		SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), FadeOut, FMovieSceneSequencePlaybackSettings(), SequenceActor);
		if (SequencePlayer)
		{
			SequencePlayer->Play();
		}

		// Prepare to move Cinder
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATrigger::SitOnBrazier, 4.0f, false);
	}
}

void ATrigger::SitOnBrazier()
{
	printFString("CinderVelocity: %s", *Cinder->GetVelocity().ToString());
	Cinder->SetActorLocation(NewLocation);
	GetWorldTimerManager().ClearTimer(TimerHandle);
}