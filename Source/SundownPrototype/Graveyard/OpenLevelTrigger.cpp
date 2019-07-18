#include "OpenLevelTrigger.h"
// include draw debug helpers header file
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

AOpenLevelTrigger::AOpenLevelTrigger()
{
	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &AOpenLevelTrigger::OnOverlapBegin);

	//Setup collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	CollisionBox->SetupAttachment(RootComponent);

	//Setup sequence
	static ConstructorHelpers::FObjectFinder<ULevelSequence> SequenceAsset(TEXT("LevelSequence'/Game/Sequences/FadeOut.FadeOut'"));
	if (SequenceAsset.Succeeded())
	{
		FadeOut = SequenceAsset.Object;
	}
}

// Called when the game starts or when spawned
void AOpenLevelTrigger::BeginPlay()
{
	Super::BeginPlay();
}

//Called when character overlaps with collision box
void AOpenLevelTrigger::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor->IsA(ACharacter::StaticClass())))
	{
		SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), FadeOut, FMovieSceneSequencePlaybackSettings(), SequenceActor);

		if (SequencePlayer)
		{
			SequencePlayer->Play();
		}

		Cinder = Cast<ACharacter>(OtherActor);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AOpenLevelTrigger::OpenNewLevel, 3.0f, false);
	}
}

void AOpenLevelTrigger::OpenNewLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), levelToOpen);
	//GetWorldTimerManager().ClearTimer(TimerHandle);
}