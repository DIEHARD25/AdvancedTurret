// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_Projectile.h"
#include "AT_TargetPractice.h"

// Sets default values
AAT_TargetPractice::AAT_TargetPractice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));

	ProjectileMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CollisionComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Health = 5.0f;
	bOnce = true;
}

void AAT_TargetPractice::OnBeginOverlap(AActor * TargetActor, AActor * OtherActor)
{
	AAT_Projectile * Projectile = Cast<AAT_Projectile>(OtherActor);
	if (Projectile)
	{
		ReceiveAnyDamage(Projectile->Damage, NULL, NULL, Projectile->GetOwner());
	}
}

// Called when the game starts or when spawned
void AAT_TargetPractice::BeginPlay()
{
	Super::BeginPlay();

	CurrentPosition = GetActorLocation();
	CurrentRotation = GetActorRotation();
	CurrentVelocity = FVector(FMath::RandRange(-700.0f, -900.0f), 0.0f, 0.0f);
	// pff.. simple
	CurrentDirection = CurrentPosition + CurrentVelocity;
	OnActorBeginOverlap.AddDynamic(this, &AAT_TargetPractice::OnBeginOverlap);		
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
	CurrentVelocityTick.Z = CurrentVelocity.Z * DeltaTime;

	CurrentDirection += CurrentVelocityTick;
	SetActorLocation(CurrentDirection);
}

void AAT_TargetPractice::ReceiveAnyDamage(float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	// calling parent for any case
	Super::ReceiveAnyDamage(Damage, DamageType, InstigatedBy, DamageCauser);
	// our own implementation before BP
	Health -= Damage;
	if (Health <= 0.0f && bOnce)
	{
		bOnce = false;
		Destroy();
	}
}
