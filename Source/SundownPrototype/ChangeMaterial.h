// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ChangeMaterial.generated.h"

UCLASS()
class SUNDOWNPROTOTYPE_API AChangeMaterial : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChangeMaterial();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* myMesh;

	UPROPERTY(EditAnywhere)
		class UMaterial* offCollision;

	UPROPERTY(EditAnywhere)
		class UMaterial* onCollision;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* myBoxComponent;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void GetActivated(bool bActive);

private:
	bool bActivated;
	
};