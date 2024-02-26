// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "DrawDebugHelpers.h"
#include "BasePawn.h"
#include "Projectile.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABasePawn::ABasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//캡슐 컴포넌트 생성
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>((TEXT("Base Mesh")));
	BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>((TEXT("Turret Mesh")));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>((TEXT("Spawn Point")));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);
}

void ABasePawn::RotateTurret(FVector LookAtTarget){
	FVector ToTarget = LookAtTarget - TurretMesh->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw,0.f);
	//위랑 같음
	// FRotator LookAtRotation = ToTarget.Rotation();
	// LookAtRotation.Pitch = 0.f;
	// LookAtRotation.Roll = 0.f;

	TurretMesh->SetWorldRotation(FMath::RInterpTo(TurretMesh->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(this),5.f));
}

void ABasePawn::HandleDestruction(){
	// TODO: Visual/sound effects

	if(DeathParticles){
			UGameplayStatics::SpawnEmitterAtLocation(this, DeathParticles,GetActorLocation(), GetActorRotation());
	}
	if(DeathSound){
		UGameplayStatics::PlaySoundAtLocation(this,DeathSound,GetActorLocation());
	}
	if(DeathCameraShakeClass){
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DeathCameraShakeClass);
	}


}

void ABasePawn::Fire(){
	FVector ProjectileSpawnPointLocation = ProjectileSpawnPoint->GetComponentLocation();
	// DrawDebugSphere(GetWorld(),ProjectileSpawnPointLocation,25.f,12,FColor::Red,false,3.f);
	
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();

	//BasePawn의 하위계층에서 GetOwner를 하면 Projectile을 넘겨줌
	auto Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass,Location,Rotation);
	Projectile->SetOwner(this);

}