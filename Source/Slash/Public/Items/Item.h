// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//���� �� �� ������ �������Ʈ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sin")
	float Amplitude = 0.25f;

	//�� ������Ƽ�� ������Ƽ â���� ������ �� ������, ��ŰŸ���� �ƴ� �ν��Ͻ� �󿡼��� �����մϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sin")
	float TimeConstant = 5.f;

	//UPROPERTY(BlueprintCallable)
	//float TransformedSin(float Value);

	template<typename T>
	T Avg(T First, T Second);

private:
	UPROPERTY(EditAnywhere)
	float RunningTime;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;

};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}
