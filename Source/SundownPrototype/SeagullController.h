// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SeagullController.generated.h"

/**
 * 
 */
UCLASS()
class SUNDOWNPROTOTYPE_API ASeagullController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(transient)
	class UBlackboardComponent *bbComp;

	UPROPERTY(transient)
	class UBehaviorTreeComponent *btComp;


public:
	ASeagullController();

	virtual void Possess(APawn * InPawn) override;

	uint8 SeagullKeyID;
};
