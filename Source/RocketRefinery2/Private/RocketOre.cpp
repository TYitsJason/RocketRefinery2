// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketOre.h"
#include "Components/StaticMeshComponent.h"
#include "RocketGameMode.h"
#include "Kismet/GameplayStatics.h"

ARocketOre::ARocketOre()
{
    PrimaryActorTick.bCanEverTick = true;

    OreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OreMesh"));
    SetRootComponent(OreMesh);
    OreMesh->SetSimulatePhysics(true);
    OreMesh->SetCollisionProfileName(TEXT("PhysicsActor"));

    BaseScoreValue = 100;
    CurrentMultiplier = 1;
}

void ARocketOre::BeginPlay()
{
    Super::BeginPlay();
}

void ARocketOre::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ARocketOre::ResetMultiplier()
{
    CurrentMultiplier = 1;
}

void ARocketOre::IncreaseMultiplier(int32 Increase)
{
    CurrentMultiplier += Increase;
}

int32 ARocketOre::GetCurrentScore() const
{
    return BaseScoreValue * CurrentMultiplier;
}

void ARocketOre::CollectOre()
{
    UE_LOG(LogTemp, Warning, TEXT("Ore Collected"));
    ARocketGameMode* GameMode = Cast<ARocketGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GameMode)
    {
        GameMode->AddScore(GetCurrentScore());
    }
    Destroy();
}