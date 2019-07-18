// Developed by Meile Bauzyte 
// Property of Jamhead Games

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
	
	//
	bool DiamondCheck(int ID, bool bActivated);

private:
	TArray<AActor*> Diamonds;
	int progress = 0;
	int NumDiamonds;
};
