// Fill out your copyright notice in the Description page of Project Settings.


#include "RefineryBouncer.h"
#include "Components/StaticMeshComponent.h"
#include "RocketOre.h"

// Sets default values
ARefineryBouncer::ARefineryBouncer()
{
    PrimaryActorTick.bCanEverTick = false;

    BouncerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BouncerMesh"));
    SetRootComponent(BouncerMesh);
    BouncerMesh->SetCollisionProfileName(TEXT("BlockAll"));
    BouncerMesh->SetNotifyRigidBodyCollision(true);
}

void ARefineryBouncer::BeginPlay()
{
    Super::BeginPlay();
    BouncerMesh->OnComponentHit.AddDynamic(this, &ARefineryBouncer::OnHit);
}

void ARefineryBouncer::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    ARocketOre* Ore = Cast<ARocketOre>(OtherActor);
    UE_LOG(LogTemp, Warning, TEXT("Ore Bounce Detected"));
    if (Ore)
    {
        // Increase the ore's multiplier
        Ore->IncreaseMultiplier(ScoreMultiplierIncrease);

        // Apply bounce force
        FVector BounceDirection = Hit.ImpactNormal;
        OtherComp->AddImpulse(BounceDirection * BounceForceMagnitude);
    }
}