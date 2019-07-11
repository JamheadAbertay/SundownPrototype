// Developed by Meile Bauzyte and Gary Whittle
// answers.unrealengine.com/questions/543696/how-to-initialize-a-c-blackboard.html 

#include "SeagullController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Seagull.h"

ASeagullController::ASeagullController() 
{
	bbComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	btComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComponent"));
}

void ASeagullController::Possess(APawn * InPawn) 
{
	Super::Possess(InPawn);
	ASeagull *Adam = Cast<ASeagull>(InPawn);
	
	if (Adam && Adam->SeagullBehavior)
		{
			bbComp->InitializeBlackboard(*Adam->SeagullBehavior->BlackboardAsset);
			//bbComp->SetValueAsVector("Location", FVector(0, 0, 0));

			SeagullKeyID = bbComp->GetKeyID("Target");

			btComp->StartTree(*Adam->SeagullBehavior);
		}
}