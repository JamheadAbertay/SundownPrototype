// Fill out your copyright notice in the Description page of Project Settings.

#include "CollisionTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"


// Sets default values
ACollisionTrigger::ACollisionTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &ACollisionTrigger::OnOverlapBegin);
	//Register Events
	OnActorEndOverlap.AddDynamic(this, &ACollisionTrigger::OnOverlapEnd);

	//Setup collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	CollisionBox->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ACollisionTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	CinderControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

// Called every frame
void ACollisionTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCloseToGround) {
		float CollisionForce = EnterZVal - Cinder->GetActorLocation().Z;
		Cinder->GetCharacterMovement()->AddForce(FVector(0.0f, 0.0f, UKismetMathLibrary::Abs(-CollisionForce)));
		
	}
}

//Called when character overlaps with collision box
void ACollisionTrigger::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor->IsA(ACharacter::StaticClass()))) {
		bCloseToGround = true;
		// Cast to Cinder
		Cinder = Cast<ACharacter>(OtherActor);
		EnterZVal = Cinder->GetActorLocation().Z;
	}

	// Cast to Cinder
	Cinder = Cast<ACharacter>(OtherActor);
}

void ACollisionTrigger::OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor) {
	bCloseToGround = false;
}