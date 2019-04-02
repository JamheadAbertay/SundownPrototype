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
#include "GameFramework/Controller.h"

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

	CinderControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	BrazierTransition.BlendExp = 20.0f;
	BrazierTransition.BlendFunction = EViewTargetBlendFunction::VTBlend_EaseInOut;
	BrazierTransition.BlendTime = 0.1f;
	BrazierTransition.bLockOutgoing = false;
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
		AccelerationStored = Cinder->GetCharacterMovement()->MaxAcceleration;
		Cinder->GetCharacterMovement()->MaxAcceleration = 0.0f;

		// Set the location of where to put Cinder during brazier interaction (we are hiding him underground)
		NewLocation = FVector(Brazier->GetActorLocation().X, Brazier->GetActorLocation().Y, Brazier->GetActorLocation().Z - 10000);

		// Begin fade
		SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), FadeOut, FMovieSceneSequencePlaybackSettings(), SequenceActor);
		if (SequencePlayer)
		{
			SequencePlayer->Play();
		}

		// Prepare to move Cinder
		GetWorld()->GetTimerManager().SetTimer(BeginBrazierTimer, this, &ATrigger::SitOnBrazier, 4.0f, false);

		// Prepare to move Cinder
		GetWorld()->GetTimerManager().SetTimer(LeaveBrazierTimer, this, &ATrigger::LeaveBrazier, 20.0f, false);
	}
}

void ATrigger::SitOnBrazier()
{
	//printFString("CinderVelocity: %s", *Cinder->GetVelocity().ToString());
	Cinder->SetActorLocation(NewLocation);
	//
	CinderControllerRef->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
	CinderControllerRef->SetViewTarget(BrazierCamera, BrazierTransition);
	//
	GetWorldTimerManager().ClearTimer(BeginBrazierTimer);
}

void ATrigger::LeaveBrazier()
{
	// Set the location of where to put Cinder after brazier interaction (we are hiding him underground)
	NewLocation = FVector(Brazier->GetActorLocation().X, Brazier->GetActorLocation().Y, Brazier->GetActorLocation().Z + 300);
	//
	Cinder->SetActorLocation(NewLocation);
	Cinder->SetActorRotation(NewRotation);
	Cinder->GetCharacterMovement()->MaxAcceleration = AccelerationStored;
	//
	CinderControllerRef->SetViewTarget(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), BrazierTransition);

	GetWorldTimerManager().ClearTimer(LeaveBrazierTimer);
}