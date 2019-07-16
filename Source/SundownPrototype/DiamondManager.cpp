// Fill out your copyright notice in the Description page of Project Settings.

#include "DiamondManager.h"

// Sets default values
ADiamondManager::ADiamondManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

// Called when the game starts or when spawned
void ADiamondManager::BeginPlay()
{
	Super::BeginPlay();

	//
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChangeMaterial::StaticClass(), Diamonds);
	NumDiamonds = Diamonds.Num();
	NumActivated = 0;

	//
	for (int i = 0; i < NumDiamonds; i++)
	{
		//AChangeMaterial* mat = Cast<AChangeMaterial>(Diamonds[i]);
		//mat->GetActivated(DiamondBools[i]);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Diamond %d = %s"), i, (DiamondBools[i] ? TEXT("True") : TEXT("False"))));
	}
}

// Called every frame
void ADiamondManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DiamondCheck();
}

void ADiamondManager::DiamondCheck() 
{

}
