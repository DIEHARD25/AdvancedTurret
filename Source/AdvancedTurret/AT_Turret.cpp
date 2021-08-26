// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_Turret.h"


// Sets default values
AAT_Turret::AAT_Turret()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	TurrenBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretBase"));
	TurretHorizontTower = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretHorizontTower"));
	
	RightBarrel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightBarrel"));
	LeftBarrel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftBarrel"));
	//TurretVerticalTower = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretVerticalTower"));

	DetectionSphere = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DetectionSphere"));
	// TODO: add sounds and FX

	// now attach them
	TurrenBase->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	DetectionSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	// tower attached to base
	TurretHorizontTower->AttachToComponent(TurrenBase, FAttachmentTransformRules::KeepRelativeTransform);
	// barrels attach to tower
	RightBarrel->AttachToComponent(TurretHorizontTower, FAttachmentTransformRules::KeepRelativeTransform);
	LeftBarrel->AttachToComponent(TurretHorizontTower, FAttachmentTransformRules::KeepRelativeTransform);

	CurrentTarget = nullptr;
}

// Called when the game starts or when spawned
void AAT_Turret::BeginPlay()
{
	Super::BeginPlay();
	// register overriden overlap call
	OnActorBeginOverlap.AddDynamic(this, &AAT_Turret::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AAT_Turret::OnEndOverlap);
}

// Called every frame
void AAT_Turret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentTarget)
		RotateToTarget(CurrentTarget->GetActorLocation());
	else
		ResetRotation();
}

// Called to bind functionality to input
void AAT_Turret::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAT_Turret::OnBeginOverlap(AActor* TurretActor, AActor* OtherActor)
{
	// first item - detection
	if (OtherActor->ActorHasTag("Target"))
	{
		// let's try to cast
		CurrentTarget = Cast<AAT_TargetPractice>(OtherActor);
		if (CurrentTarget)
		{
			RotateToTarget(CurrentTarget->GetActorLocation());
		}
	}
}

void AAT_Turret::OnEndOverlap(AActor* TurretActor, AActor* OtherActor)
{
	CurrentTarget = nullptr;
}

void AAT_Turret::ResetRotation()
{
	TurretHorizontTower->SetRelativeRotation(FRotator(0,0,0));
	RightBarrel->SetRelativeRotation(FRotator(0, 0, 0));
	LeftBarrel->SetRelativeRotation(FRotator(0, 0, 0));
}

void AAT_Turret::RotateToTarget(FVector TargetLocation)
{
	// pure magic...
	// calcualting the diff
	FVector TransformDiff = TargetLocation - TurretHorizontTower->GetComponentLocation();
	// now calculating the desiredLocation to point
	FVector DesiredLocation = UKismetMathLibrary::InverseTransformDirection(GetActorTransform(), TransformDiff);
	// now making rotation from location
	FRotator DesiredRotation = UKismetMathLibrary::MakeRotFromX(DesiredLocation);
	// and finally apply the rotator to tower
	TurretHorizontTower->SetRelativeRotation(FRotator( 0.0, DesiredRotation.Yaw, 0.0));

	RightBarrel->SetRelativeRotation(FRotator( DesiredRotation.Pitch, 0.0, 0.0));
	LeftBarrel->SetRelativeRotation(FRotator( DesiredRotation.Pitch, 0.0, 0.0));

}
