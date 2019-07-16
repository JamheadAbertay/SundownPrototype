// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChangeMaterial.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "DiamondManager.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API ADiamondManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADiamondManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DiamondCheck();

private:
	TArray<AActor*> Diamonds;
	//TArray<bool*> DiamondBools;
	int NumDiamonds; 
	int NumActivated;

};
