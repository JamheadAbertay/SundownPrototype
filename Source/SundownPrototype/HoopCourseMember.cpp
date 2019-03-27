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
	Hoop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
	Hoop->SetupAttachment(RootComponent);
	Hoop->bUseDefaultCollision = false;
	Hoop->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Hoop->SetMobility(EComponentMobility::Static);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> HoopAsset(TEXT("StaticMesh'/Game/Assets/Props/Ring_Whole01_polySurface23.Ring_Whole01_polySurface23'"));
	if (HoopAsset.Succeeded())
	{
		Hoop->SetStaticMesh(HoopAsset.Object);
	}

	RootComponent = Hoop;

	//Setup glowing hoop object
	GlowingHoop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlowingHoop"));
	GlowingHoop->SetupAttachment(RootComponent);
	GlowingHoop->SetRelativeLocation(FVector(-38.0f, -15.0f, 129.0f));
	GlowingHoop->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	GlowingHoop->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	GlowingHoop->bUseDefaultCollision = false;
	GlowingHoop->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GlowingHoop->SetMobility(EComponentMobility::Static);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> GlowingHoopAsset(TEXT("StaticMesh'/Game/Assets/Props/Ring_Whole01_polySurface31.Ring_Whole01_polySurface31'"));
	if (GlowingHoopAsset.Succeeded())
	{
		GlowingHoop->SetStaticMesh(GlowingHoopAsset.Object);
	}

	//Setup collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetRelativeLocation(FVector(-38.0f, -15.0f, 129.0f));
	CollisionBox->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	CollisionBox->SetRelativeScale3D(FVector(0.75f, 0.1f, 0.75f));
	CollisionBox->SetMobility(EComponentMobility::Static);

	//Setup hoops light
	HoopLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("HoopLight"));
	HoopLight->SetupAttachment(RootComponent);
	HoopLight->Activate();
	HoopLight->bVisible = false;
	HoopLight->IntensityUnits = ELightUnits::Candelas;
	HoopLight->SetIntensity(1000.0f);
	HoopLight->SetLightColor(FLinearColor(0.0f, 1.0f, 1.0f, 1.0f));
	HoopLight->SetAttenuationRadius(500.0f);
	HoopLight->SetRelativeLocation(FVector(-38.0f, -15.0f, 129.0f));
	HoopLight->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	HoopLight->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	HoopLight->SetMobility(EComponentMobility::Stationary);

	//Setup sound to play when bird flies through hoop
	HoopSound = CreateDefaultSubobject<USoundCue>(TEXT("Hoop sound"));

	static ConstructorHelpers::FObjectFinder<USoundCue> HoopSoundAsset(TEXT("SoundCue'/Game/Hoops/HoopSoundCue.HoopSoundCue'"));
	if (HoopSoundAsset.Succeeded())
	{
		HoopSound = HoopSoundAsset.Object;
	}
}

// Called when the game starts or when spawned
void AHoopCourseMember::BeginPlay()
{
	Super::BeginPlay();

	if (isHoopActive)
	{
		HoopLight->ToggleVisibility();
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
			if (HoopToLight == nullptr)
			{
				//Print course is finished to log
				UE_LOG(LogTemp, Warning, TEXT("Hoop course finished"));
			}

			//If there is another hoop after this one
			else
			{
				//Set the next hoop to be active and not hidden
				HoopToLight->isHoopActive = true;
				//Turn hoop to lights light on
				HoopToLight->HoopLight->ToggleVisibility();
			}
			//Play hoop sound
			UGameplayStatics::PlaySound2D(GetWorld(), HoopSound);

			//Set this hoop to be inactive
			isHoopActive = false;
		}
	}
}

