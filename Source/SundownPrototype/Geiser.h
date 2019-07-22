//scrambled by Meile Bauzyte and Gary Whittle

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Geiser.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API AGeiser : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	AGeiser();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Geiser spray function
	void Spray();

	//
	UFUNCTION(BlueprintCallable)
		void SetArmadilloRef(ACharacter* Army);

	// 
	UPROPERTY(EditAnywhere)
		class UBoxComponent* myBoxComponent;

	//
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* myGeiserMesh;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UArrowComponent* ImpulseDirection;

private:
	FTimerHandle TimerHandleThing;
	// For if armadillo is in geiser area
	bool bArmadillo;
	// 
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = Timing)
		float GeiserTime = 5.0f;

	//
	ACharacter* ArmadilloRef;
	UCharacterMovementComponent* MovementRef;
};
