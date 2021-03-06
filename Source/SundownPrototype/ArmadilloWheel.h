// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "ArmadilloWheel.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API AArmadilloWheel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArmadilloWheel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void SpinWheel(float speed, UArrowComponent* RotationArrow, UArrowComponent* DotPArrow);

	UPROPERTY(BlueprintReadWrite)
		bool WheelTurning = false;
	float WheelSpeed = 0.0f;

	UArrowComponent* TheArrow;
};
