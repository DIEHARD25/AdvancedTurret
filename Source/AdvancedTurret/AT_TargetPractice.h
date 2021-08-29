// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AT_TargetPractice.generated.h"

UCLASS()
class ADVANCEDTURRET_API AAT_TargetPractice : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAT_TargetPractice();

	FVector CurrentPosition;
	FRotator CurrentRotation;
	FVector CurrentVelocity;
	FVector CurrentVelocityTick;
	FVector CurrentDirection;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ProjectileMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateMovement(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};