// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_TargetPractice.h"

// Sets default values
AAT_TargetPractice::AAT_TargetPractice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AAT_TargetPractice::BeginPlay()
{
	Super::BeginPlay();

	CurrentPosition = GetActorLocation();
	CurrentRotation = GetActorRotation();
	CurrentVelocity = FVector(FMath::RandRange(-150.0f, 150.0f), FMath::RandRange(-150.0f, 150.0f), 0.0f);
	// pff.. simple
	CurrentDirection = CurrentPosition + CurrentVelocity;
}

// Called every frame
void AAT_TargetPractice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMovement(DeltaTime);	
}

void AAT_TargetPractice::UpdateMovement(float DeltaTime)
{
	CurrentPosition = GetActorLocation();

	CurrentVelocityTick.X = CurrentVelocity.X * DeltaTime;
	CurrentVelocityTick.Y = CurrentVelocity.Y * DeltaTime;
	CurrentVelocityTick.Z = 0.0f;

	CurrentDirection += CurrentVelocityTick;
	SetActorLocation(CurrentDirection);
}