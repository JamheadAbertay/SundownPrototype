// Fill out your copyright notice in the Description page of Project Settings.

#include "HoopCourseMember.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHoopCourseMember::AHoopCourseMember()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &AHoopCourseMember::OnOverlapBegin);

	//Setup hoop object
	Hoop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hoop object"));
	Hoop->SetupAttachment(RootComponent);
	Hoop->SetMobility(EComponentMobility::Static);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> HoopAsset(TEXT("StaticMesh'/Game/Assets/Landscape/SM_RockCircle.SM_RockCircle'"));
	if (HoopAsset.Succeeded())
	{
		Hoop->SetStaticMesh(HoopAsset.Object);
	}

	//Setup collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	CollisionBox->SetupAttachment(Hoop);
	CollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 360.0f));
	CollisionBox->SetRelativeRotation(FRotator(30.0f, 0.0f, 0.0f));
	CollisionBox->SetRelativeScale3D(FVector(10.0f, 1.0f, 11.0f));
	CollisionBox->SetMobility(EComponentMobility::Static);

	//Initially set next hoop to hidden
	//HoopToSpawn->SetActorHiddenInGame(true);

	//Setup a particle system
	HoopFire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fire Particles"));
	HoopFire->SetupAttachment(Hoop);
	HoopFire->bAutoActivate = true;
	HoopFire->SetVisibility(false);
	HoopFire->SetRelativeLocation(FVector(0.0f, 0.0f, 360.0f));
	HoopFire->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	HoopFire->SetRelativeScale3D(FVector(15.0f, 15.0f, 20.0f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> HoopFireAsset(TEXT("ParticleSystem'/Game/Characters/Creatures/Bavi/FairyParticle.FairyParticle'"));
	if (HoopFireAsset.Succeeded())
	{
		HoopFire->SetTemplate(HoopFireAsset.Object);
	}

	//Setup sound to play when bird flies through hoop
	HoopSound = CreateDefaultSubobject<USoundCue>(TEXT("Hoop sound"));
	static ConstructorHelpers::FObjectFinder<USoundCue> HoopSoundAsset(TEXT("SoundCue'/Game/Hoops/HoopSoundCue.HoopSoundCue'"));
	if (HoopSoundAsset.Succeeded())
	{
		HoopSound = HoopSoundAsset.Object;
	}

	//Setup sound to play when fire starts
	FireSound = CreateDefaultSubobject<USoundCue>(TEXT("Fire sound"));
	static ConstructorHelpers::FObjectFinder<USoundCue> FireSoundAsset(TEXT("SoundCue'/Game/Hoops/HoopSoundCue.HoopSoundCue'"));
	if (FireSoundAsset.Succeeded())
	{
		FireSound = FireSoundAsset.Object;
	}
}

// Called when the game starts or when spawned
void AHoopCourseMember::BeginPlay()
{
	Super::BeginPlay();

	//If this hoop is not active, hide it in game
    //Only start hoop will be active on begin play
	if (!isHoopActive)
	{
		this->SetActorHiddenInGame(true);
	}
	else
	{
		this->SetActorHiddenInGame(false);
	}
}

// Called every frame
void AHoopCourseMember::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Called when character overlaps with collision box
void AHoopCourseMember::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	//If this hoop is active
	if (isHoopActive)
	{
		//If overlapping actor is of type character
		if (OtherActor && (OtherActor->IsA(ACharacter::StaticClass())))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit the hoop"));

			//If this hoop is the last in the course
			if (HoopToSpawn == nullptr)
			{
				//Print course is finished to log
				UE_LOG(LogTemp, Warning, TEXT("Hoop course finished"));
			}
			//If there is another hoop after this one
			else
			{
				//Set the next hoop to be active and not hidden
				HoopToSpawn->isHoopActive = true;
				HoopToSpawn->SetActorHiddenInGame(false);
			}
			//Play hoop sound
			UGameplayStatics::PlaySound2D(GetWorld(), HoopSound);
			//Play fire sound
			UGameplayStatics::PlaySound2D(GetWorld(), FireSound);
			//Spawn particle system
			HoopFire->SetVisibility(true);
			//Set this hoop to be inactive
			isHoopActive = false;
		}
	}
}

