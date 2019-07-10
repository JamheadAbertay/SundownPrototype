// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardData.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTreeComponent.h"
#include "Runtime/AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Seagull.h"
#include "SeagullController.generated.h"

/**
 * 
 */
UCLASS()
class SUNDOWNPROTOTYPE_API ASeagullController : public AAIController
{
	GENERATED_BODY()

public:
	ASeagullController();


protected:
	virtual void Possess(APawn * InPawn);

private:
	UBlackboardComponent* bbComp;
	UBehaviorTreeComponent* btComp;
	

	void SetNextPosition(FVector NextPosition);

};
