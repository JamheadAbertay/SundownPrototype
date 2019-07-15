// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_CheckNextPosition.h"
#include "Engine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BirdPawn.h"
#include "SeagullController.h"
#include "Seagull.h"

UBTService_CheckNextPosition::UBTService_CheckNextPosition()
{
	bCreateNodeInstance = true;
}

void UBTService_CheckNextPosition::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	//get AI component class
	ASeagullController *SeagullAI = Cast<ASeagullController>(OwnerComp.GetAIOwner()); 

	
	if (SeagullAI) 
	{
		ABirdPawn *Cinder = Cast<ABirdPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

		if (Cinder)
		{
			//get target controller from the blackboard
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(SeagullAI->SeagullKeyID, Cinder);
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Eureka")));
		}
	}
}