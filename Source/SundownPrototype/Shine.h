// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Shine.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API AShine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShine();

	// THe book mesh 
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ShinyThing;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* Material;

	UMaterialInstanceDynamic* DynMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
