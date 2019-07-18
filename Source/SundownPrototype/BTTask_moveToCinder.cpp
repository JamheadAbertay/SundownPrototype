// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_moveToCinder.h"
#include "Engine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BirdPawn.h"
#include "SeagullController.h"
#include "Seagull.h"

EBTNodeResult::Type UBTTask_moveToCinder::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//get an enemy component
	ASeagullController *SeagullAI = Cast<ASeagullController>(OwnerComp.GetAIOwner());

	//cast it from the blackboard
	ABirdPawn *Cinder = Cast<ABirdPawn>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(SeagullAI->SeagullKeyID));

	if (Cinder)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("cinder")));

		SeagullAI->MoveToActor(Cinder, 5.0f, true, true, true, 0, true);
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	EBTNodeResult::Failed;
}


