// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include "AT_Projectile.generated.h"

UCLASS()
class ADVANCEDTURRET_API AAT_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAT_Projectile();

	UPROPERTY(EditAnywhere)	
		UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(EditAnywhere)	
		UCapsuleComponent * CollisionComp;
	UPROPERTY(EditAnywhere)	
		UProjectileMovementComponent * Movement;

	UPROPERTY(EditAnywhere)
		float ProjectileSpeed;
	UPROPERTY(EditAnywhere)
		float ProjectileRange;
	UPROPERTY(EditAnywhere)
		float Damage;

	FVector StartLocation;		
	bool bOnce;

	UFUNCTION()	
		void OnBeginOverlap(AActor* ProjectileActor, AActor* OtherActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
