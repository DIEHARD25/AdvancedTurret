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
	PitchRestrict = 60.0f;
	YawRestrict = 360.0f;

	PitchSpeed = 150.0f;
	YawSpeed = 150.0f;
	PotentialTargets.Empty();
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
	// commented out since we have nice tickless tracking code with timers
	/*
	if(CurrentTarget)
		TrackTarget(CurrentTarget->GetActorLocation());
	else
		ResetRotation();
	*/
}

// Called to bind functionality to input
void AAT_Turret::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAT_Turret::OnBeginOverlap(AActor* TurretActor, AActor* OtherActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("OverlapBegin"));
	// first check if this is ok target and add it to array
	AAT_TargetPractice * Target = Cast<AAT_TargetPractice>(OtherActor);

	if (Target)
	{
		//UE_LOG(LogTemp, Warning, TEXT("OverlapBegin - adding"));
		PotentialTargets.Add(Target);
		UpdateCurrentTarget();
	}
	if (CurrentTarget)
		BeginTrack();
}

void AAT_Turret::OnEndOverlap(AActor* TurretActor, AActor* OtherActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("OverlapEnd"));
	AAT_TargetPractice * Target = Cast<AAT_TargetPractice>(OtherActor);

	if (Target)
	{
		//UE_LOG(LogTemp, Warning, TEXT("OverlapEnd - removing"));
		PotentialTargets.Remove(Target);
		UpdateCurrentTarget();
	}
	if (!CurrentTarget)
		BeginReset();
}

void AAT_Turret::ResetRotation()
{
	float DesiredYaw = UKismetMathLibrary::RInterpTo_Constant(
		TurretHorizontTower->GetRelativeRotation(),
		FRotator(0.0, 0.0, 0.0),
		GetWorld()->GetDeltaSeconds(),
		YawSpeed).Yaw;

	TurretHorizontTower->SetRelativeRotation(FRotator(0, DesiredYaw, 0));

	float DesiredPitch = UKismetMathLibrary::RInterpTo_Constant(
		RightBarrel->GetRelativeRotation(),
		FRotator(0.0, 0.0, 0.0),
		GetWorld()->GetDeltaSeconds(),
		PitchSpeed).Pitch;

	RightBarrel->SetRelativeRotation(FRotator(DesiredPitch, 0, 0));

	LeftBarrel->SetRelativeRotation(FRotator(DesiredPitch, 0, 0));
}

void AAT_Turret::BeginTrack()
{
	//UE_LOG(LogTemp, Warning, TEXT("BeginTrack()"));
	if (CurrentTarget)
		ContinueTrack();	
	else
		CancelTrack();
}

void AAT_Turret::ContinueTrack()
{
	//UE_LOG(LogTemp, Warning, TEXT("ContinueTrack()"));
	if (CurrentTarget)
	{
		UpdateCurrentTarget();
		TrackTarget(CurrentTarget->GetActorLocation());		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAT_Turret::ContinueTrack, GetWorld()->GetDeltaSeconds(), true, 0.005f);
	}
	else	
		CancelTrack();	
}

void AAT_Turret::CancelTrack()
{
	//UE_LOG(LogTemp, Warning, TEXT("CancelTrack()"));
	GetWorldTimerManager().ClearTimer(TimerHandle);
	BeginReset();
}

void AAT_Turret::BeginReset()
{
	//UE_LOG(LogTemp, Warning, TEXT("BeginReset()"));
	ContinueReset();
}

void AAT_Turret::ContinueReset()
{
	//UE_LOG(LogTemp, Warning, TEXT("ContinueReset()"));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAT_Turret::ContinueReset, GetWorld()->GetDeltaSeconds(), true, 0.005f);
	ResetRotation();

	if (FMath::IsNearlyEqual(TurretHorizontTower->GetRelativeRotation().Yaw, 0.0f, 0.0001f) 
		&& 
		FMath::IsNearlyEqual(RightBarrel->GetRelativeRotation().Pitch, 0.0f, 0.0001f))
		CancelReset();
}

void AAT_Turret::CancelReset()
{
	//UE_LOG(LogTemp, Warning, TEXT("CancelReset()"));	
	GetWorldTimerManager().ClearTimer(TimerHandle);
}


FRotator AAT_Turret::ApplyRestrict(FRotator DesiredRotation)
{	
	//UE_LOG(LogTemp, Warning, TEXT("ApplyRestrict"));
	if (DesiredRotation.Yaw >= YawRestrict)
		DesiredRotation.Yaw = YawRestrict;
	else if (DesiredRotation.Yaw <= -YawRestrict)
		DesiredRotation.Yaw = -YawRestrict;

	if (DesiredRotation.Pitch >= PitchRestrict)
		DesiredRotation.Pitch = PitchRestrict;
	else if (DesiredRotation.Pitch <= -PitchRestrict)
		DesiredRotation.Pitch = -PitchRestrict;

	return DesiredRotation;
}

void AAT_Turret::UpdateCurrentTarget()
{
	if ( PotentialTargets.Num() == 0 )
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateCurrentTarget - nullptr"));
		CurrentTarget = nullptr;
		return;
	}
	else
	{
		float MinLength = FVector::Distance(TurretHorizontTower->GetComponentLocation(), PotentialTargets[0]->GetActorLocation());
		for (auto it : PotentialTargets)
		{
			float NextLength = FVector::Distance(TurretHorizontTower->GetComponentLocation(), it->GetActorLocation());
			if (NextLength <= MinLength)
			{
				MinLength = NextLength;
				CurrentTarget = it;
			}
		}
	}	
}

void AAT_Turret::TrackTarget(FVector TargetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("TrackTarget"));
	// pure magic...
	// calcualting the diff
	FVector TransformDiff = TargetLocation - TurretHorizontTower->GetComponentLocation();
	// now calculating the desiredLocation to point
	FVector DesiredLocation = UKismetMathLibrary::InverseTransformDirection(GetActorTransform(), TransformDiff);
	// now making rotation from location
	FRotator DesiredRotation = UKismetMathLibrary::MakeRotFromX(DesiredLocation);
	// and finally apply the rotator to tower
	// apply restrict
	DesiredRotation = ApplyRestrict(DesiredRotation);
	// calculate the RInterp values for Yaw 
	float DesiredYaw = UKismetMathLibrary::RInterpTo_Constant(
		TurretHorizontTower->GetRelativeRotation(), 
		FRotator(0.0, DesiredRotation.Yaw, 0.0), 
		GetWorld()->GetDeltaSeconds(),
		YawSpeed).Yaw;
	// apply Yaw
	TurretHorizontTower->SetRelativeRotation(FRotator( 0.0, DesiredYaw, 0.0));
	// calculate the RInterp values for Pitch 
	float DesiredPitch = UKismetMathLibrary::RInterpTo_Constant(
		RightBarrel->GetRelativeRotation(), 
		FRotator(DesiredRotation.Pitch, 0.0, 0.0),
		GetWorld()->GetDeltaSeconds(),
		PitchSpeed).Pitch;
	// apply Pitch
	RightBarrel->SetRelativeRotation(FRotator( DesiredPitch, 0.0, 0.0));
	LeftBarrel->SetRelativeRotation(FRotator( DesiredPitch, 0.0, 0.0));
}
