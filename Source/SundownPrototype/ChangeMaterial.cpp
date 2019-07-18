// Developed by Meile Bauzyte
// Property of Jamhead Games

#include "ChangeMaterial.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"

// Sets default values
AChangeMaterial::AChangeMaterial()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	myMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = myMesh;

	myBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("box"));
	myBoxComponent->InitBoxExtent(FVector(100.0, 100.0, 100.0));
	myBoxComponent->SetCollisionProfileName("Trigger");
	myBoxComponent->SetupAttachment(RootComponent);

	onCollision = CreateAbstractDefaultSubobject<UMaterial>(TEXT("ONCollision"));
	offCollision = CreateAbstractDefaultSubobject<UMaterial>(TEXT("OffCollision"));

	// set up a notification for when this component overlaps something  
	myBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AChangeMaterial::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AChangeMaterial::BeginPlay()
{
	Super::BeginPlay();
	
	//DrawDebugBox(GetWorld(), GetActorLocation(), FVector(100, 100, 100), FColor::White, true, -1, 0, 10);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADiamondManager::StaticClass(), DiamondManagers);
	DiamondManager = Cast<ADiamondManager>(DiamondManagers[0]);
	if (DiamondManager) {
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Manager found")));
	}

	myMesh->SetMaterial(0, offCollision);
	bActivated = false;
}

// Called every frame
void AChangeMaterial::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AChangeMaterial::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if collision happens
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		bool correct = DiamondManager->DiamondCheck(sequenceID, bActivated);
		//activate the boolean to say that it is indeed shiny
		bActivated = correct;
		
		//if incorrect order has been presented, deactivate everything that has been done/ change all material to be unlit
		if (!correct)
			Deactivate();
		//if the sequence is correct, set material to shiny
		else if (correct) 
			myMesh->SetMaterial(0, onCollision);
		
	}
}

void AChangeMaterial::Deactivate() 
{
	//sets the material to be unlit
	myMesh->SetMaterial(0, offCollision);
	bActivated = false;
}