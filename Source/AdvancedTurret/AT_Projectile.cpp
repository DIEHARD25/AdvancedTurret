// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_Projectile.h"

// Sets default values
AAT_Projectile::AAT_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));

	CollisionComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ProjectileMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ProjectileMesh->SetGenerateOverlapEvents(true);
	
	Movement->Activate();
	Movement->InitialSpeed = 2000.0f;
	Movement->MaxSpeed = 2500.0f;

	Movement->bRotationFollowsVelocity = true;

	Movement->ProjectileGravityScale = 0.0f;

}

// Called when the game starts or when spawned
void AAT_Projectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAT_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

