// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "BirdCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class SUNDOWNPROTOTYPE_API ABirdCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay();
	
	
};
