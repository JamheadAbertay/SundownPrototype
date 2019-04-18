#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT(text), fstring))

#include "MatineeTrigger.h"

// Sets default values
AMatineeTrigger::AMatineeTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &AMatineeTrigger::OnOverlapBegin);

	//Setup collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	CollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	CollisionBox->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMatineeTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMatineeTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Called when character overlaps with collision box
void AMatineeTrigger::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor->IsA(ACharacter::StaticClass())))
	{
		// Begin fade
		SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), SequenceToPlay, FMovieSceneSequencePlaybackSettings(), SequenceActor);
		if (SequencePlayer)
		{
			SequencePlayer->Play();
		}
	}
}

