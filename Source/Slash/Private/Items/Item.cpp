// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshCompoennt"));
	RootComponent = ItemMesh;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	//상속 받은 기능을 덮어 사용할때는 Super 사용 SUPER 아님
	Super::BeginPlay();

	int32 AVGInt = Avg<int32>(1, 3);
	UE_LOG(LogTemp, Warning, TEXT("Avg of 1 and 3: %d"), AVGInt);

	float AVGfloat = Avg<float>(1.3f, 3.4f);
	UE_LOG(LogTemp, Warning, TEXT("Avg of 1 and 3: %d"), AVGfloat);
}

//float AItem::TransformedSin(float Value)
//{
	//return Amplitude * FMath::Sin(Value * TimeConstant);
//}


// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	RunningTime += DeltaTime;

}

