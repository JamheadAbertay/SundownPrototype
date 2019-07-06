// Fill out your copyright notice in the Description page of Project Settings.

#include "BirdPlayerController.h"

void ABirdPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCameraManager->ViewPitchMax = 15.0f;
	PlayerCameraManager->ViewPitchMin = -89.99f;
	PlayerCameraManager->DefaultFOV = 105.0f;
}