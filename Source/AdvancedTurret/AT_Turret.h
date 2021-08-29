// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "AT_TargetPractice.h"

#include "AT_Turret.generated.h"

UCLASS()
class ADVANCEDTURRET_API AAT_Turret : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAT_Turret();

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* TurrenBase; // non-moving

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* TurretHorizontTower; // moving in horizont

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent * LeftBarrel;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent * RightBarrel;

	UPROPERTY(EditAnywhere)
		UCapsuleComponent* DetectionSphere;

	AAT_TargetPractice* CurrentTarget;

	//UPROPERTY(EditAnywhere)
	//	TSubsclassOf<AT_Projectile>* AT_Projectile;

	UFUNCTION()
		void OnBeginOverlap(AActor* TurretActor, AActor* OtherActor);
	UFUNCTION()
		void OnEndOverlap(AActor* TurretActor, AActor* OtherActor);

	void RotateToTarget(FVector TargetLocation);
	void ResetRotation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};