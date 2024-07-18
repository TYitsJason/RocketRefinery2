// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketCharacter.h"
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
ARocketCharacter::ARocketCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and set up components
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetEnableGravity(true);
	CollisionComponent->SetLinearDamping(0.1f);
	CollisionComponent->SetAngularDamping(0.1f);

	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(CollisionComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	if (!CameraComponent) {
		CameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
		CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight)); 
	}

	// Create and set up movement component
	UFloatingPawnMovement* MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

	// Set default values
	MaxMovementPower = 1000.0f;
	MovementPowerIncrement = 10.0f;
	MovementPower = FVector::ZeroVector;
	RotationSpeed = 45.0f;
}

// Called when the game starts or when spawned
void ARocketCharacter::BeginPlay()
{
	Super::BeginPlay();
	LockRotation();
}

// Called every frame
void ARocketCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Movement = 
		CameraComponent->GetForwardVector() * MovementPower.X +
		CameraComponent->GetRightVector() * MovementPower.Y +
		CameraComponent->GetUpVector() * MovementPower.Z;

	// Move the rocket based on the current movement power
	CollisionComponent->AddForce(Movement * DeltaTime);
	LockRotation();
}

// Called to bind functionality to input
void ARocketCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARocketCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARocketCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ARocketCharacter::MoveUp);
	PlayerInputComponent->BindAxis("Turn", this, &ARocketCharacter::Turn);

	PlayerInputComponent->BindAction("ResetMovement", IE_Pressed, this, &ARocketCharacter::ResetMovement);
}

void ARocketCharacter::MoveForward(float value)
{
	if (value != 0.0f)
	{
		MovementPower.X += value * MovementPowerIncrement;
		MovementPower.X = FMath::Clamp(MovementPower.X, -MaxMovementPower, MaxMovementPower);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Moving X"));
	}
}

void ARocketCharacter::MoveRight(float value)
{
	if (value != 0.0f)
	{
		MovementPower.Y += value * MovementPowerIncrement;
		MovementPower.Y = FMath::Clamp(MovementPower.Y, -MaxMovementPower, MaxMovementPower);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Moving Y"));
	}
}

void ARocketCharacter::MoveUp(float value)
{
	if (value != 0.0f)
	{
		MovementPower.Z += value * MovementPowerIncrement;
		MovementPower.Z = FMath::Clamp(MovementPower.Z, -MaxMovementPower, MaxMovementPower);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Moving Z"));
	}
}

void ARocketCharacter::ResetMovement()
{
	MovementPower = FVector::ZeroVector;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Resetting Movement"));
}

void ARocketCharacter::Turn(float value)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += value * RotationSpeed * GetWorld()->GetDeltaSeconds();
	SetActorRotation(NewRotation);
}

void ARocketCharacter::LockRotation()
{
	FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FRotator(0.0f, CurrentRotation.Yaw, 0.0f));

	// Lock physics rotation as well
	FVector AngularVelocity = CollisionComponent->GetPhysicsAngularVelocityInDegrees();
	CollisionComponent->SetPhysicsAngularVelocityInDegrees(FVector(0.0f, 0.0f, AngularVelocity.Z));
}

