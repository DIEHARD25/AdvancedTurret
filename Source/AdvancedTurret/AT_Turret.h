// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "AT_TargetPractice.h"
#include "AT_Projectile.h"

#include "AT_Turret.generated.h"

UCLASS()
class ADVANCEDTURRET_API AAT_Turret : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAT_Turret();

	UPROPERTY(EditAnywhere)	
		UStaticMeshComponent * TurrenBase; // non-moving
	UPROPERTY(EditAnywhere)	
		UStaticMeshComponent * TurretHorizontTower; // moving in horizont
	UPROPERTY(EditAnywhere)	
		UStaticMeshComponent * LeftBarrel; // moving in vertical
	UPROPERTY(EditAnywhere)	
		UStaticMeshComponent * RightBarrel; // moving in vertical
	UPROPERTY(EditAnywhere)	
		UCapsuleComponent * DetectionSphere; // temporary detection - will move this logic to dedicated class
	UPROPERTY(EditAnywhere)	
		TSubclassOf<AAT_Projectile> Projectile_BP;

	UFUNCTION() 
		void OnBeginOverlap(AActor* TurretActor, AActor* OtherActor);
	UFUNCTION()	
		void OnEndOverlap(AActor* TurretActor, AActor* OtherActor);

	TArray<AAT_TargetPractice *> PotentialTargets;
	
	// rotation restrict values
	UPROPERTY(EditAnywhere)	
		float PitchRestrictUp;
	UPROPERTY(EditAnywhere)	
		float PitchRestrictDown;
	UPROPERTY(EditAnywhere)	
		float YawRestrictLeft;
	UPROPERTY(EditAnywhere)	
		float YawRestrictRight;
	// rotation speed values
	UPROPERTY(EditAnywhere)	
		float PitchSpeed;
	UPROPERTY(EditAnywhere)	
		float YawSpeed;
	UPROPERTY(EditAnywhere)	
		float FireRate;
	UPROPERTY(EditAnywhere)
		float TrackTolerance;
	UPROPERTY(EditAnywhere)
		float ProjectileSpread;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateCurrentTarget();
	FRotator TrackTarget(FVector TargetLocation);
	FVector PredictTargetPosition(FVector TargetLocation);
	void ResetRotation();

	void BeginTrack();
	void ContinueTrack();
	void CancelTrack();

	void BeginReset();
	void ContinueReset();
	void CancelReset();

	void BeginFiring();
	void ContinueFiring();
	void CancelFiring();

	void FireProjectile();

	FRotator ApplyRestrict(FRotator DesiredRotation);
	
	bool bOnce;

	FTimerHandle RotationTimerHandle;
	FTimerHandle FireTimerHandle;

	FActorSpawnParameters Params;

	AAT_TargetPractice * CurrentTarget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
