// Fill out your copyright notice in the Description page of Project Settings.

#include "BirdCameraManager.h"


void ABirdCameraManager::BeginPlay()
{
	Super::BeginPlay();
	ViewPitchMax = 70.0f;
	ViewPitchMin = -70.0f;
}


