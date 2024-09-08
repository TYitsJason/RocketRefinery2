// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityGun.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UGravityGun::UGravityGun()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UGravityGun::SetPhysicsHandle(UPhysicsHandleComponent* NewPhysicsHandle)
{
	PhysicsHandle = NewPhysicsHandle;
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicsHandle is null in GravityGun::SetPhysicsHandle"));
	}
}

// Called when the game starts
void UGravityGun::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UGravityGun::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateGunTransform();

	if (IsHolding())
	{
		UpdateHeldObjectPosition();
	}

}

void UGravityGun::UpdateGunTransform()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		UCameraComponent* CameraComp = OwnerCharacter->FindComponentByClass<UCameraComponent>();
		if (CameraComp)
		{
			FVector NewLocation = CameraComp->GetComponentLocation() +
				CameraComp->GetRightVector() * GunOffset.Y +
				CameraComp->GetUpVector() * GunOffset.Z +
				CameraComp->GetForwardVector() * GunOffset.X;

			FRotator NewRotation = CameraComp->GetComponentRotation();

			// For debug visualization
			DrawDebugBox(GetWorld(), NewLocation, FVector(5.0f), NewRotation.Quaternion(), FColor::Blue, false, -1, 0, 2);
		}
	}
}

void UGravityGun::TryGrab()
{
	if (!PhysicsHandle) return;

	FHitResult HitResult = TraceForObjects();
	if (HitResult.GetActor())
	{
		UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
		if (ComponentToGrab && ComponentToGrab->IsSimulatingPhysics())
		{
			// Store the original physics settings
			OriginalLinearDamping = ComponentToGrab->GetLinearDamping();
			OriginalAngularDamping = ComponentToGrab->GetAngularDamping();
			OriginalGravityEnabled = ComponentToGrab->IsGravityEnabled();

			// Apply new physics settings for tighter control
			ComponentToGrab->SetLinearDamping(10.0f);
			ComponentToGrab->SetAngularDamping(10.0f);
			ComponentToGrab->SetEnableGravity(false);

			// Grab the component
			PhysicsHandle->GrabComponentAtLocationWithRotation(
				ComponentToGrab,
				NAME_None,
				ComponentToGrab->GetOwner()->GetActorLocation(),
				ComponentToGrab->GetOwner()->GetActorRotation()
			);

			// Snap the object to the desired position immediately
			UpdateHeldObjectPosition();
		}
	}
}

void UGravityGun::Release()
{
	if (!PhysicsHandle) return;

	if (IsHolding())
	{
		UPrimitiveComponent* ReleasedComponent = PhysicsHandle->GetGrabbedComponent();
		PhysicsHandle->ReleaseComponent();

		// Restore original physics settings
		if (ReleasedComponent)
		{
			ReleasedComponent->SetLinearDamping(OriginalLinearDamping);
			ReleasedComponent->SetAngularDamping(OriginalAngularDamping);
			ReleasedComponent->SetEnableGravity(OriginalGravityEnabled);
		}
	}
}

void UGravityGun::Launch()
{
	if (!PhysicsHandle)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("Error: No PhysicsHandle")));
		return;
	}

	if (IsHolding())
	{
		UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent();
		PhysicsHandle->ReleaseComponent();

		if (GrabbedComponent)
		{
			GrabbedComponent->SetLinearDamping(OriginalLinearDamping);
			GrabbedComponent->SetAngularDamping(OriginalAngularDamping);
			GrabbedComponent->SetEnableGravity(OriginalGravityEnabled);

			FVector LaunchDirection = GetOwner()->GetActorForwardVector();
			GrabbedComponent->AddImpulse(LaunchDirection * LaunchForce);

			// Debug: Print info about launched object
			UE_LOG(LogTemp, Warning, TEXT("Launched object with force: %f"), LaunchForce);
		}
	}
}

bool UGravityGun::IsHolding() const
{
	return PhysicsHandle && PhysicsHandle->GetGrabbedComponent();
}

FHitResult UGravityGun::TraceForObjects() const
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return FHitResult();

	UCameraComponent* CameraComp = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	if (!CameraComp) return FHitResult();

	FVector Start = CameraComp->GetComponentLocation();
	FVector ForwardVector = CameraComp->GetForwardVector();
	FVector End = Start + ForwardVector * GrabDistance;

	// Define the cone's half-angle in radians (e.g., 15 degrees)
	float ConeHalfAngle = FMath::DegreesToRadians(15.0f);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	TArray<FHitResult> HitResults;

	// Perform the sphere trace
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		Start,
		End,
		GrabDistance * FMath::Tan(ConeHalfAngle), // Radius at the end of the cone
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f // Debug draw duration
	);

	// Filter hits based on the "GravityGunInteractable" tag and being within the cone
	FHitResult ClosestValidHit;
	float ClosestDistance = MAX_FLT;
	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(FName("GravityGunInteractable")))
		{
			FVector DirectionToHit = (Hit.ImpactPoint - Start).GetSafeNormal();
			float AngleToHit = FMath::Acos(FVector::DotProduct(ForwardVector, DirectionToHit));

			// Check if the hit is within the cone
			if (AngleToHit <= ConeHalfAngle)
			{
				float Distance = (Hit.ImpactPoint - Start).Size();
				if (Distance < ClosestDistance)
				{
					ClosestValidHit = Hit;
					ClosestDistance = Distance;
				}
			}
		}
	}

	// Debug visualization for the cone
	if (GetWorld() && GetWorld()->IsPlayInEditor())
	{
		DrawDebugCone(
			GetWorld(),
			Start,
			ForwardVector,
			GrabDistance,
			ConeHalfAngle,
			ConeHalfAngle,
			32,
			FColor::Yellow,
			false,
			5.0f,
			0,
			2.0f
		);
	}

	// Debug logging
	if (ClosestValidHit.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace hit interactable object: %s at location %s"),
			*ClosestValidHit.GetActor()->GetName(),
			*ClosestValidHit.ImpactPoint.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace did not hit any interactable objects"));
	}

	return ClosestValidHit;
}

void UGravityGun::UpdateHeldObjectPosition()
{
	if (!IsHolding()) return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	UCameraComponent* CameraComp = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	if (!CameraComp) return;

	FVector TargetLocation = CameraComp->GetComponentLocation() +
		CameraComp->GetForwardVector() * HeldObjectDistance +
		HeldObjectOffset;
	FRotator TargetRotation = CameraComp->GetComponentRotation();

	PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, TargetRotation);

	// Force the grabbed component to the target position and rotation
	UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent();
	if (GrabbedComponent)
	{
		GrabbedComponent->SetWorldLocationAndRotation(TargetLocation, TargetRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}
