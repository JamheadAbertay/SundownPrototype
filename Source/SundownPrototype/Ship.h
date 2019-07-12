// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Ship.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API AShip : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShip();

	UPROPERTY(EditAnywhere, Category = Material)
		UMaterialInterface* SailMaterial;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ShipMesh;

	UPROPERTY(EditAnywhere)
		USplineComponent* ShipSpline;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	float fFireAmount = 0.0f;
	
	float fSplineProgress = 0.0f;
};
