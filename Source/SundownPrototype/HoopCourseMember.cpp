// Fill out your copyright notice in the Description page of Project Settings.

#include "HoopCourseMember.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AHoopCourseMember::AHoopCourseMember()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &AHoopCourseMember::OnOverlapBegin);

	//Setup collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 360.0f));
	CollisionBox->SetRelativeRotation(FRotator(30.0f, 0.0f, 0.0f));
	CollisionBox->SetRelativeScale3D(FVector(10.0f, 1.0f, 11.0f));

	//Setup hoop object
	Hoop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hoop object"));
	Hoop->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HoopAsset(TEXT("StaticMesh'/Game/Assets/Landscape/SM_RockCircle.SM_RockCircle'"));
	if (HoopAsset.Succeeded())
	{
		Hoop->SetStaticMesh(HoopAsset.Object);
	}

	// Create a particle system
	//OurParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fire Particles"));
	//OurParticleSystem->SetupAttachment(RootComponent);
	//OurParticleSystem->bAutoActivate = false;
	//OurParticleSystem->SetVisibility(false);
	//OurParticleSystem->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	//static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"));
	//if (ParticleAsset.Succeeded())
	//{
	//	OurParticleSystem->SetTemplate(ParticleAsset.Object);
	//}

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

	printf("Hit the hoop");

}

//Called when character overlaps with collision box
void AHoopCourseMember::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor->IsA(ACharacter::StaticClass())))
	{
		printf("Hit the hoop");
	}
}

