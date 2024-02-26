// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BasePawn.generated.h"

UCLASS()
class TOONTANKS_API ABasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasePawn();

	void HandleDestruction();

protected:
	void RotateTurret(FVector LookAtTarget);
	void Fire();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComp;

	//UStaticMeshComponent는 기본적으로 모든 액터에 포함되어 있으며 당연히 폰은 액터이기에 UStaticMeshComponent가 존재한거다. 모르겠으면 그냥 다 전방 선언class 해줘도됨
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseMesh;

	//UPROPERTY
	//언리얼 엔진이 이 변수를 언리얼 엔진 리플렉션 시스템에 참여할 수 있게 인식하는 방법
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;
														//이 변수가 private 상태에서도 이벤트 그래프에서 볼 수 있음
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Super Duper Variables")
	int32 VisibleAnywhereInt = 12;

	//어디서나 편집하게 할 수 있다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Super Duper Variables")
	int32 EditAnywhere = 12;

	UPROPERTY(EditAnywhere, Category="Combat")
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Combat")
	class UParticleSystem* DeathParticles;

	UPROPERTY(EditAnywhere, Category="Combat")
	class USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class UCameraShakeBase> DeathCameraShakeClass;


};
