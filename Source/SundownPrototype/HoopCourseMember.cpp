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

	// Create a particle system
	//HoopFire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fire Particles"));
	//HoopFire->SetupAttachment(RootComponent);
	//HoopFire->bAutoActivate = false;
	//HoopFire->SetVisibility(false);
	//HoopFire->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	//static ConstructorHelpers::FObjectFinder<UParticleSystem> HoopFireAsset(TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"));
	//if (HoopFireAsset.Succeeded())
	//{
	//	HoopFire->SetTemplate(HoopFireAsset.Object);
	//}

	HoopSound = CreateDefaultSubobject<USoundCue>(TEXT("Hoop sound"));
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundAsset(TEXT("SoundCue'/Game/Hoops/HoopSoundCue.HoopSoundCue'"));
	if (SoundAsset.Succeeded())
	{
		HoopSound = SoundAsset.Object;
	}
}

// Called when the game starts or when spawned
void AHoopCourseMember::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHoopCourseMember::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Called when character overlaps with collision box
void AHoopCourseMember::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (isHoopActive)
	{
		if (OtherActor && (OtherActor->IsA(ACharacter::StaticClass())))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit the hoop"));

			UGameplayStatics::PlaySound2D(GetWorld(), HoopSound);

			isHoopActive = false;
		}
	}
}

