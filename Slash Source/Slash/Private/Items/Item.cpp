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
	//��� ���� ����� ���� ����Ҷ��� Super ��� SUPER �ƴ�
	Super::BeginPlay();

	//Unreal Engine C++���� ������ �տ� 'F'�� ���̴� ���� �ַ� Unreal Engine�� Ư�� ������ Ÿ���� ��Ÿ���� ���λ�
	//FString: Unreal Engine�� ���ڿ� Ŭ����
	//FVector : 3D ���������� ���� Ŭ����
	//FRotator : 3D ȸ���� ��Ÿ���� Ŭ����
	//FTransform : 3D ��ȯ ������ ��Ÿ���� Ŭ����
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


	// �ʴ� �̵��ӵ�
	float MovementRate = 50.f;
	float RotationRate = 100.f;

	AddActorWorldOffset(FVector(MovementRate * DeltaTime, 0.f, 0.f));
	AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f, FColor::Red, false, -1.f, 0, 1.f);
	DrawDebugSphere(GetWorld(), GetActorLocation(), 25.f, 10, FColor::Red, false, -1.f);
	//DRAW_SPHERE_SingleFrame(GetActorLocation());

}

