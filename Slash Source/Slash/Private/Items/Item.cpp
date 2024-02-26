// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	//상속 받은 기능을 덮어 사용할때는 Super 사용 SUPER 아님
	Super::BeginPlay();

	//Unreal Engine C++에서 변수명 앞에 'F'를 붙이는 것은 주로 Unreal Engine의 특정 데이터 타입을 나타내는 접두사
	//FString: Unreal Engine의 문자열 클래스
	//FVector : 3D 공간에서의 벡터 클래스
	//FRotator : 3D 회전을 나타내는 클래스
	//FTransform : 3D 변환 정보를 나타내는 클래스
	/*
	UE_LOG(LogTemp, Warning ,TEXT("HELLO"));
	FString Name = GetName();
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Red, FString("Item OnScreen Message!"));
	}
	*/
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 25.f, 10, FColor::Red, false, 30.f);
	
	//SetActorLocation(FVector(0.f, 0.f, 50.f));
	//SetActorRotation(FRotator(0.f, 45.f, 0.f));

	//DrawDebugPoint(GetWorld(), GetActorLocation(), 10.f, FColor::Red, false, 30.f);
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f, FColor::Red, true , -1.f, 0 ,1.f);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// 초당 이동속도
	float MovementRate = 50.f;
	float RotationRate = 100.f;

	AddActorWorldOffset(FVector(MovementRate * DeltaTime, 0.f, 0.f));
	AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f, FColor::Red, false, -1.f, 0, 1.f);
	DrawDebugSphere(GetWorld(), GetActorLocation(), 25.f, 10, FColor::Red, false, -1.f);
	//DRAW_SPHERE_SingleFrame(GetActorLocation());

}

