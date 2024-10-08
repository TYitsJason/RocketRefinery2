// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketCharacter.h"
#include "RocketGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
ARocketCharacter::ARocketCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UE_LOG(LogTemp, Warning, TEXT("RocketCharacter constructed"));

	// Create and set up components
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetEnableGravity(true);
	CollisionComponent->SetLinearDamping(0.1f);
	CollisionComponent->SetAngularDamping(0.1f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetMassOverrideInKg(NAME_None, 1.0f);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CollisionComponent);
	CameraComponent->bUsePawnControlRotation = false;

	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(CollisionComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GravityGun = CreateDefaultSubobject<UGravityGun>(TEXT("GravityGun"));

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	// Set default values
	MaxMovementPower = 1000.0f;
	MovementPowerIncrement = 10.0f;
	RotationSpeed = 45.0f;
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	// Set up collision notifications
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ARocketCharacter::OnHit);
}

void ARocketCharacter::UpdateDebugMessage()
{
	CurrentDebugMessage = FString::Printf(TEXT("Movement Power: X=%.2f, Y=%.2f, Z=%.2f"),
		MovementPower.X, MovementPower.Y, MovementPower.Z);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, CurrentDebugMessage, true);
	}
}

// Called when the game starts or when spawned
void ARocketCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("RocketCharacter BeginPlay called"));

	if (GetRootComponent() != CollisionComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("CollisionComponent is not the root component!"));
		SetRootComponent(CollisionComponent);
		CameraComponent->SetWorldRotation(GetActorRotation());
	}

	if (GravityGun && PhysicsHandle)
	{
		GravityGun->SetPhysicsHandle(PhysicsHandle);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GravityGun or PhysicsHandle is null in RocketCharacter"));
	}

	MovementPower = FVector(0, 0, DefaultUpForce);
	CurrentHealth = MaxHealth;
}

// Called every frame
void ARocketCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Movement =
		CameraComponent->GetForwardVector() * MovementPower.X +
		CameraComponent->GetRightVector() * MovementPower.Y +
		CollisionComponent->GetUpVector() * MovementPower.Z;

	FRotator CurrentRotation = GetActorRotation();
	FRotator CurrentCameraRotation = CameraComponent->GetComponentRotation();

	FVector NewCameraLocation = GetActorLocation();
	CameraComponent->SetWorldLocation(NewCameraLocation);

	// Move the rocket based on the current movement power
	CollisionComponent->AddForce(Movement);

	// Debug: Print out the calculated Movement vector
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue,
		FString::Printf(TEXT("Applied Force: X=%.2f, Y=%.2f, Z=%.2f"),
			Movement.X, Movement.Y, Movement.Z));

	// Debug: Print out the current velocity
	FVector Velocity = CollisionComponent->GetPhysicsLinearVelocity();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green,
		FString::Printf(TEXT("Current Velocity: X=%.2f, Y=%.2f, Z=%.2f"),
			Velocity.X, Velocity.Y, Velocity.Z));

	// Debug: Print out the current location
	FVector Location = GetActorLocation();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red,
		FString::Printf(TEXT("Current Location: X=%.2f, Y=%.2f, Z=%.2f"),
			Location.X, Location.Y, Location.Z));

	// Debug: Print out the camera's rotation
	FRotator CameraLocation = CameraComponent->GetRelativeRotation();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan,
		FString::Printf(TEXT("Camera Rotation: X=%.2f, Y=%.2f, Z=%.2f"),
			CameraLocation.Pitch, CameraLocation.Yaw, CameraLocation.Roll));

	// Debug: Print out the relative location of the camera to the character
	FVector RelativeLocation = CameraComponent->GetRelativeLocation();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Magenta,
		FString::Printf(TEXT("Camera Relative Location: X=%.2f, Y=%.2f, Z=%.2f"),
			RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z));

	UpdateDebugMessage();
}

// Called to bind functionality to input
void ARocketCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARocketCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARocketCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ARocketCharacter::MoveUp);
	PlayerInputComponent->BindAxis("Turn", this, &ARocketCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ARocketCharacter::LookUp);

	PlayerInputComponent->BindAction("ResetMovement", IE_Pressed, this, &ARocketCharacter::ResetMovement);

	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &ARocketCharacter::OnGravityGunGrab);
	PlayerInputComponent->BindAction("PickUp", IE_Released, this, &ARocketCharacter::OnGravityGunRelease);
	PlayerInputComponent->BindAction("Launch", IE_Pressed, this, &ARocketCharacter::OnGravityGunLaunch);
}

void ARocketCharacter::MoveForward(float value)
{
	if (value != 0.0f)
	{
		MovementPower.X += value * MovementPowerIncrement;
		MovementPower.X = FMath::Clamp(MovementPower.X, -MaxMovementPower, MaxMovementPower);
	}
}

void ARocketCharacter::MoveRight(float value)
{
	if (value != 0.0f)
	{
		MovementPower.Y += value * MovementPowerIncrement;
		MovementPower.Y = FMath::Clamp(MovementPower.Y, -MaxMovementPower, MaxMovementPower);
	}
}

void ARocketCharacter::MoveUp(float value)
{
	if (value != 0.0f)
	{
		MovementPower.Z += value * MovementPowerIncrement;
		MovementPower.Z = FMath::Clamp(MovementPower.Z, DefaultUpForce - MaxMovementPower, DefaultUpForce + MaxMovementPower);
	}
}

void ARocketCharacter::ResetMovement()
{
	MovementPower = FVector(0, 0, DefaultUpForce);
	UpdateDebugMessage();
}

void ARocketCharacter::Turn(float value) 
{
	if (value != 0.0f) 
	{
		const float YawChange = value * RotationSpeed * GetWorld()->GetDeltaSeconds();

		// Only update Yaw, keep Pitch and Roll at 0
		AddActorWorldRotation(FRotator(0.0f, YawChange, 0.0f));

		// Update camera rotation to match actor rotation
		CameraComponent->AddWorldRotation(FRotator(0.0f, YawChange, 0.0f));
	}
}

void ARocketCharacter::LookUp(float value)
{
	if (value != 0.0f)
	{
		const float PitchChange = value * RotationSpeed * GetWorld()->GetDeltaSeconds();

		CameraComponent->AddLocalRotation(FRotator(PitchChange, 0.0f, 0.0f));
	}
}

void ARocketCharacter::OnGravityGunGrab()
{
	if (GravityGun)
	{
		GravityGun->TryGrab();
	}
	else 
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red,
			FString::Printf(TEXT("Tried: Grab, Error: No Gravity Gun Found")));
	}
}

void ARocketCharacter::OnGravityGunRelease()
{
	if (GravityGun)
	{
		GravityGun->Release();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red,
			FString::Printf(TEXT("Tried: Release, Error: No Gravity Gun Found")));
	}
}

void ARocketCharacter::OnGravityGunLaunch()
{
	if (GravityGun)
	{
		GravityGun->Launch();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red,
			FString::Printf(TEXT("Tried: Launch, Error: No Gravity Gun Found")));
	}
}

void ARocketCharacter::TakeDamage(float DamageAmount)
{
	if (CurrentHealth <= 0.0f) 
	{
		return;
	}

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

	if (CurrentHealth <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character has died!"));
		ARocketGameMode* GameMode = Cast<ARocketGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->EndGame();
		}
	}

	// Log or display the current health
	UE_LOG(LogTemp, Warning, TEXT("Current Health: %f"), CurrentHealth);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health: %f"), CurrentHealth));
}

void ARocketCharacter::SetMaxHealth(float NewMaxHealth)
{
	MaxHealth = FMath::Max(0.0f, NewMaxHealth);
	CurrentHealth = FMath::Min(CurrentHealth, MaxHealth);
}

void ARocketCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		FVector Velocity = CollisionComponent->GetPhysicsLinearVelocity();
		float ImpactSpeed = Velocity.Size();

		UE_LOG(LogTemp, Warning, TEXT("Collision details - Speed: %f, Threshold: %f"), ImpactSpeed, DamageVelocityThreshold);

		if (ImpactSpeed > DamageVelocityThreshold)
		{
			float DamageAmount = (ImpactSpeed - DamageVelocityThreshold) * DamageScalingFactor;
			if (DamageAmount > MaxDamage) 
			{
				DamageAmount = MaxDamage;
			}
			TakeDamage(DamageAmount);

			UE_LOG(LogTemp, Warning, TEXT("Damage applied - Amount: %f, New Health: %f"), DamageAmount, CurrentHealth);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Impact speed below threshold, no damage applied"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit something, but it wasn't a valid actor or was self"));
	}
}
void ARocketCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin called: Overlapped Actor = %s"), OtherActor ? *OtherActor->GetName() : TEXT("None"));
}