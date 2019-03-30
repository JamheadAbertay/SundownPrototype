#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Collectible.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API ACollectible : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectible();

	//Overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Hoop object
	UPROPERTY(EditAnywhere, Category = Collectible)
		UStaticMeshComponent* Collectible;

	//Collision box that will trigger pickup
	UPROPERTY(EditAnywhere, Category = Box)
		UBoxComponent* CollisionBox;

	//Sound to play when cinder collects pickup
	UPROPERTY()
		USoundCue* CollectibleSound;

	//Number of pickups cinder has collected
	int collectCount = 0;
	FString collectCountString;
};
