// Developed by Meile Bauzyte and Gary Whittle
// answers.unrealengine.com/questions/543696/how-to-initialize-a-c-blackboard.html 

#include "SeagullController.h"

ASeagullController::ASeagullController() {
	bbComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void ASeagullController::Possess(APawn * InPawn) {
	Super::Possess(InPawn);
	ASeagull* Adam = Cast<ASeagull>(InPawn);
	
	if (Adam) {
		if (Adam->BehaviourTree->BlackboardAsset) {
			bbComp->InitializeBlackboard(*Adam->BehaviourTree->BlackboardAsset);
			bbComp->SetValueAsVector("Location", FVector(0, 0, 0));
		}

		btComp->StartTree(*Adam->BehaviourTree);
	}
}

void ASeagullController::SetNextPosition(FVector NextPosition) {
	if (bbComp)
	{
		bbComp->SetValueAsVector("Location", NextPosition);
	}
}