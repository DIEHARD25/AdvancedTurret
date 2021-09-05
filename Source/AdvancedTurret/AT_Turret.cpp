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

	LeftBarrel->AttachToComponent(TurretHorizontTower, FAttachmentTransformRules::KeepRelativeTransform);
	RightBarrel->AttachToComponent(TurretHorizontTower, FAttachmentTransformRules::KeepRelativeTransform);

	CurrentTarget = nullptr;
	PitchRestrictUp = 60.0f;
	PitchRestrictDown = 10.0f;
	YawRestrictRight = 120.0f;
	YawRestrictLeft = 120.0f;

	PitchSpeed = 150.0f;
	YawSpeed = 150.0f;

	FireRate = 60.0f; // shots per minute
	TrackTolerance = 0.01f;
	ProjectileSpread = 0.5f;
	ProjectileSpeed = 3000.0f;

	Params = {};
	Params.Owner = this;

	bOnce = true;

	PotentialTargets.Empty();
}

// Called when the game starts or when spawned
void AAT_Turret::BeginPlay()
{
	Super::BeginPlay();
	// register overriden overlap call
	OnActorBeginOverlap.AddDynamic(this, &AAT_Turret::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AAT_Turret::OnEndOverlap);	
	// extra check for zro FireRate
	if (FireRate == 0.0f)
		FireRate = 0.0001f;
	if(ProjectileSpeed == 0.0f)
		ProjectileSpeed = 0.0001f;
}

// Called every frame
void AAT_Turret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAT_Turret::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAT_Turret::OnBeginOverlap(AActor* TurretActor, AActor* OtherActor)
{
	// first check if this is ok target and add it to array
	AAT_TargetPractice * Target = Cast<AAT_TargetPractice>(OtherActor);

	if (Target)
	{
		PotentialTargets.Add(Target);
		// update the currentTarget - we want to track closest to turret
		UpdateCurrentTarget();
	}
	if (CurrentTarget)
		BeginTrack();
}

void AAT_Turret::OnEndOverlap(AActor* TurretActor, AActor* OtherActor)
{
	AAT_TargetPractice * Target = Cast<AAT_TargetPractice>(OtherActor);

	if (Target)
	{
		PotentialTargets.Remove(Target);
		// update the currentTarget - we want to track closest to turret
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
		LeftBarrel->GetRelativeRotation(),
		FRotator(0.0, 0.0, 0.0),
		GetWorld()->GetDeltaSeconds(),
		PitchSpeed).Pitch;

	LeftBarrel->SetRelativeRotation(FRotator(DesiredPitch, 0, 0));
	RightBarrel->SetRelativeRotation(FRotator(DesiredPitch, 0, 0));
}

void AAT_Turret::BeginTrack()
{
	GetWorld()->GetTimerManager().SetTimer(RotationTimerHandle, this, &AAT_Turret::ContinueTrack, GetWorld()->GetDeltaSeconds(), true, 0.0f);
	// double-check we have the target
	if (CurrentTarget)
		ContinueTrack();	
	else
		CancelTrack();
}

void AAT_Turret::ContinueTrack()
{
	if (CurrentTarget)
	{
		UpdateCurrentTarget();
		FVector PredictedOffset = PredictTargetPosition(CurrentTarget->GetActorLocation());
		// pure magic...
		FRotator CurrentRotation = TrackTarget(CurrentTarget->GetActorLocation() + PredictedOffset);

		if (FMath::IsNearlyEqual(TurretHorizontTower->GetRelativeRotation().Yaw, CurrentRotation.Yaw, TrackTolerance)
			&&
			FMath::IsNearlyEqual(LeftBarrel->GetRelativeRotation().Pitch, CurrentRotation.Pitch, TrackTolerance)
			&&
			bOnce
			&&
			( // track restrict check
			!FMath::IsNearlyEqual(TurretHorizontTower->GetRelativeRotation().Yaw, YawRestrictRight, TrackTolerance)
			||
			!FMath::IsNearlyEqual(TurretHorizontTower->GetRelativeRotation().Yaw, -YawRestrictLeft, TrackTolerance)
			||
			!FMath::IsNearlyEqual(LeftBarrel->GetRelativeRotation().Pitch, PitchRestrictUp, TrackTolerance)
			||
			!FMath::IsNearlyEqual(LeftBarrel->GetRelativeRotation().Pitch, -PitchRestrictDown, TrackTolerance))
			)
			BeginFiring();
			
	}
	else	
		CancelTrack();	
}

void AAT_Turret::CancelTrack()
{
	GetWorldTimerManager().ClearTimer(RotationTimerHandle);
	BeginReset();
}

void AAT_Turret::BeginReset()
{
	GetWorld()->GetTimerManager().SetTimer(RotationTimerHandle, this, &AAT_Turret::ContinueReset, GetWorld()->GetDeltaSeconds(), true, 0.0f);
	ContinueReset();
}

void AAT_Turret::ContinueReset()
{	
	ResetRotation();
	if (FMath::IsNearlyEqual(TurretHorizontTower->GetRelativeRotation().Yaw, 0.0f, TrackTolerance)
		&& 
		FMath::IsNearlyEqual(LeftBarrel->GetRelativeRotation().Pitch, 0.0f, TrackTolerance))
		CancelReset();
}

void AAT_Turret::CancelReset()
{
	GetWorldTimerManager().ClearTimer(RotationTimerHandle);
}

void AAT_Turret::BeginFiring()
{	
	bOnce = false;
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AAT_Turret::ContinueFiring, 60.0 / FireRate, true, 60.0 / FireRate);
	ContinueFiring();
}

void AAT_Turret::ContinueFiring()
{
	if (CurrentTarget)
		FireProjectile();
	else
		CancelFiring();
}

void AAT_Turret::CancelFiring()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	bOnce = true;
	// target destriyed or left the range - let's check is there any other targets
	BeginTrack();
}

void AAT_Turret::FireProjectile()
{
	// spawn the projectile
	if (Projectile_BP)
	{
		FVector SpawnLocation = LeftBarrel->GetComponentLocation();
		if (LeftBarrel->DoesSocketExist(TEXT("ProjectileSpawn"))) SpawnLocation = LeftBarrel->GetSocketLocation(TEXT("ProjectileSpawn"));
		FRotator SpawnRotation = LeftBarrel->GetComponentRotation();
		AAT_Projectile * NewProjectile = Cast<AAT_Projectile>(GetWorld()->SpawnActor(Projectile_BP, &SpawnLocation, &SpawnRotation, Params));
		if (NewProjectile)
		{
			// don;t touching X since it's the initial look of the projectile
			NewProjectile->Movement->Velocity.Y += FMath::RandRange(-ProjectileSpread * 100, ProjectileSpread * 100);
			NewProjectile->Movement->Velocity.Z += FMath::RandRange(-ProjectileSpread * 100, ProjectileSpread * 100);
		}
					
		SpawnLocation = RightBarrel->GetComponentLocation();
		if (RightBarrel->DoesSocketExist(TEXT("ProjectileSpawn"))) SpawnLocation = RightBarrel->GetSocketLocation(TEXT("ProjectileSpawn"));
		SpawnRotation = RightBarrel->GetComponentRotation();
		AAT_Projectile * RightProjectile = Cast<AAT_Projectile>(GetWorld()->SpawnActor(Projectile_BP, &SpawnLocation, &SpawnRotation, Params));
		if (RightProjectile)
		{
			// don;t touching X since it's the initial look of the projectile
			RightProjectile->Movement->Velocity.Y += FMath::RandRange(-ProjectileSpread * 100, ProjectileSpread * 100);
			RightProjectile->Movement->Velocity.Z += FMath::RandRange(-ProjectileSpread * 100, ProjectileSpread * 100);
		}		
	}
}


FRotator AAT_Turret::ApplyRestrict(FRotator DesiredRotation)
{	
	if (DesiredRotation.Yaw >= YawRestrictRight)
		DesiredRotation.Yaw = YawRestrictRight;
	else if (DesiredRotation.Yaw <= -YawRestrictLeft)
		DesiredRotation.Yaw = -YawRestrictLeft;

	if (DesiredRotation.Pitch >= PitchRestrictUp)
		DesiredRotation.Pitch = PitchRestrictUp;
	else if (DesiredRotation.Pitch <= -PitchRestrictDown)
		DesiredRotation.Pitch = -PitchRestrictDown;

	return DesiredRotation;
}

void AAT_Turret::UpdateCurrentTarget()
{
	if ( PotentialTargets.Num() == 0 )
	{
		CurrentTarget = nullptr;
		return;
	}
	else
	{
		AAT_TargetPractice * OldCurrentTarget = CurrentTarget;
		float MinLength = (TurretHorizontTower->GetComponentLocation() - PotentialTargets[0]->GetActorLocation()).Size();
		for (auto it : PotentialTargets)
		{
			float NextLength = (TurretHorizontTower->GetComponentLocation() - it->GetActorLocation()).Size();
			if (NextLength <= MinLength)
			{
				MinLength = NextLength;
				CurrentTarget = it;
			}
		}
		if (OldCurrentTarget != CurrentTarget && !bOnce) // we have new one, so stop fire during possible track
			CancelFiring();
	}	
}

FRotator AAT_Turret::TrackTarget(FVector TargetLocation)
{
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
		LeftBarrel->GetRelativeRotation(),
		FRotator(DesiredRotation.Pitch, 0.0, 0.0),
		GetWorld()->GetDeltaSeconds(),
		PitchSpeed).Pitch;
	// apply Pitch
	LeftBarrel->SetRelativeRotation(FRotator( DesiredPitch, 0.0, 0.0));
	RightBarrel->SetRelativeRotation(FRotator(DesiredPitch, 0.0, 0.0));
	return DesiredRotation;
}

FVector AAT_Turret::PredictTargetPosition(FVector TargetLocation)
{
	if (!Projectile_BP)
		return FVector();

	if (!CurrentTarget)
		return FVector();

	//UE_LOG(LogTemp, Warning, TEXT("Predicting start"));
	// get actual distance to target - using turretTower
	float Distance = (TargetLocation - TurretHorizontTower->GetComponentLocation()).Size();
	// now let's calc the time projectile travel those dist
	float TravelTime = Distance / ProjectileSpeed;
	// we have prediction time, now let's calc the speed vector during those time
	//UE_LOG(LogTemp, Warning, TEXT("Predicting result: %f | %f | %f"), CurrentTarget->CurrentVelocityTick.X, CurrentTarget->CurrentVelocityTick.Y, CurrentTarget->CurrentVelocityTick.Z);
	return CurrentTarget->CurrentVelocity * TravelTime;
}
