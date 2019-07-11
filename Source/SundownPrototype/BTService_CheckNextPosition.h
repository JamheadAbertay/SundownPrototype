// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckNextPosition.generated.h"

/**
 * 
 */
UCLASS()
class SUNDOWNPROTOTYPE_API UBTService_CheckNextPosition : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_CheckNextPosition();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
