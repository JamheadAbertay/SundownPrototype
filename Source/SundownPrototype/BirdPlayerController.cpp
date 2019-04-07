// Fill out your copyright notice in the Description page of Project Settings.

#include "BirdPlayerController.h"

void ABirdPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCameraManager->ViewPitchMax = 35.0f;
	PlayerCameraManager->ViewPitchMin = -60.0f;
	PlayerCameraManager->DefaultFOV = 105.0f;
}