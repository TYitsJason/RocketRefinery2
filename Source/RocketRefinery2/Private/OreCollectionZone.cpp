// Fill out your copyright notice in the Description page of Project Settings.


#include "OreCollectionZone.h"
#include "Components/BoxComponent.h"
#include "RocketOre.h"

AOreCollectionZone::AOreCollectionZone()
{
    PrimaryActorTick.bCanEverTick = false;

    CollectionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("CollectionZone"));
    SetRootComponent(CollectionZone);
    CollectionZone->SetCollisionProfileName(TEXT("OverlapAll"));
}

void AOreCollectionZone::BeginPlay()
{
    Super::BeginPlay();
    CollectionZone->OnComponentBeginOverlap.AddDynamic(this, &AOreCollectionZone::OnOverlapBegin);
}

void AOreCollectionZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ARocketOre* Ore = Cast<ARocketOre>(OtherActor);
    if (Ore)
    {
        Ore->CollectOre();
    }
}