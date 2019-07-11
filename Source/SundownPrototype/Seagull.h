// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/RandomStream.h"
#include "Seagull.generated.h"



UCLASS()
class SUNDOWNPROTOTYPE_API ASeagull : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASeagull();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Frame(float);

	

	UPROPERTY(EditAnywhere)
		class UBehaviorTree *SeagullBehavior;
	

};

/*
private:

	FVector vC, vMove, vTurn;
	FRotator fRotatorSeagull;

	UPROPERTY(EditAnywhere)
		float Hreach = 5000.0f;

	UPROPERTY(EditAnywhere)
		float Vreach = 1500.0f;

	float XDistance, YDistance, ZDistance;
	float accelerationAmount = 1.0f;

	bool bTurn = false; //turning mechanism while active

	FVector ReturnDirection(); v  

*/