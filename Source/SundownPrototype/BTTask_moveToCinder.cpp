// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_moveToCinder.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BirdPawn.h"
#include "SeagullController.h"
#include "Seagull.h"

EBTNodeResult::Type UBTTask_moveToCinder::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASeagullController *SeagullAI = Cast<ASeagullController>(OwnerComp.GetAIOwner());

	ABirdPawn *Cinder = Cast<ABirdPawn>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(SeagullAI->SeagullKeyID));

	if (Cinder)
	{
		SeagullAI->MoveToActor(Cinder, 5.0f, true, true, true, 0, true);
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	EBTNodeResult::Failed;
}


