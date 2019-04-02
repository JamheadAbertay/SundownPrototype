#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT(text), fstring))

#include "Collectible.h"

// Sets default values
ACollectible::ACollectible()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &ACollectible::OnOverlapBegin);

	//Setup hoop object
	Collectible = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collectible"));
	Collectible->SetupAttachment(RootComponent);
	Collectible->bUseDefaultCollision = false;
	Collectible->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collectible->SetMobility(EComponentMobility::Static);

	//Setup collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	CollisionBox->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	CollisionBox->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CollisionBox->SetMobility(EComponentMobility::Static);

	//Setup sound to play when bird flies through hoop
	CollectibleSound = CreateDefaultSubobject<USoundCue>(TEXT("Collectible sound"));

	static ConstructorHelpers::FObjectFinder<USoundCue> CollectibleSoundAsset(TEXT("SoundCue'/Game/Hoops/HoopSoundCue.HoopSoundCue'"));
	if (CollectibleSoundAsset.Succeeded())
	{
		CollectibleSound = CollectibleSoundAsset.Object;
	}

}

// Called when the game starts or when spawned
void ACollectible::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACollectible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Called when character overlaps with collision box
void ACollectible::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	//If overlapping actor is of type character
	if (OtherActor && (OtherActor->IsA(ACharacter::StaticClass())))
	{
		//Add 1 to collectible count
		collectCount += 1;

		//Update collectible count string
		collectCountString = FString::FromInt(collectCount);

		//Print collectible count
		UE_LOG(LogTemp, Warning, TEXT("collectible count: %s"), *collectCountString);

		//Play hoop sound
		UGameplayStatics::PlaySound2D(GetWorld(), CollectibleSound);

		//Destroy this actor
		this->Destroy();
	}
}

