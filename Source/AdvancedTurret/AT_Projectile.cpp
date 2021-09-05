// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_Projectile.h"
#include "AT_TargetPractice.h"

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
	
	ProjectileSpeed = 2500.0f;
	ProjectileRange = 3000.0f;
	Damage = 1.0f;

	Movement->Activate();
	Movement->InitialSpeed = ProjectileSpeed;
	Movement->MaxSpeed = ProjectileSpeed;

	Movement->bRotationFollowsVelocity = true;

	Movement->ProjectileGravityScale = 0.0f;	
	bOnce = true;
}

void AAT_Projectile::OnBeginOverlap(AActor * ProjectileActor, AActor * OtherActor)
{
	AAT_TargetPractice * Target = Cast<AAT_TargetPractice>(OtherActor);
	if (Target && bOnce)
	{
		UGameplayStatics::ApplyDamage(Target, Damage, NULL, this, NULL);
		bOnce = false;
		Destroy();
	}
}

// Called when the game starts or when spawned
void AAT_Projectile::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	OnActorBeginOverlap.AddDynamic(this, &AAT_Projectile::OnBeginOverlap);
}

// Called every frame
void AAT_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector CurrentLocation = GetActorLocation() - StartLocation;
	if (CurrentLocation.Size() >= ProjectileRange && bOnce)
	{
		bOnce = false;
		Destroy();
	}
}

