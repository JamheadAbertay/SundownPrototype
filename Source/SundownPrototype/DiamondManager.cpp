// Developed by Meile Bauzyte
// Property of Jamhead Games

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

}

// Called every frame
void ADiamondManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("progress: %d"), progress));
}

bool ADiamondManager::DiamondCheck(int ID, bool bActivated)
{
	bool correct;

	//if the thing is not lit and it is indeed the right one
	if (ID == (progress + 1) && bActivated == false)
	{
		progress += 1;//adding  to the progress for cheching the following thing

		//going through all the diamonds
		for (int i = 0; i < NumDiamonds; i++)
		{
			//finds all diamonds in the level
			AChangeMaterial* mat = Cast<AChangeMaterial>(Diamonds[i]);

			//if the current diamond is the one to be activated later, turn off all the diamonds
			if (mat->sequenceID > progress)
				mat->Deactivate();

		}
		//set the thing in the sequence to be correct
		correct = true;
	}

	//if the diamond is incorrect one and havent been checked before
	else if (ID != (progress + 1) && bActivated == false)
	{
		progress = 0; //sets the queue back to the start

		//runs through all the diamonds in the level
		for (int i = 0; i < NumDiamonds; i++)
		{
			//finds all the diamonds in the level
			AChangeMaterial* mat = Cast<AChangeMaterial>(Diamonds[i]);

			//deactivates all diamonds
			mat->Deactivate();
		}

		//set the thing in the sequence to be incorrect
		correct = false;
	}

	//if all the diamonds have been ticked correctly display "woo" on a screen
	if (progress == NumDiamonds) 
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("Woo")));

	return correct;
}
