// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstsance = GetMesh()->GetAnimInstance();
	if (AnimInstsance && HitReactMontage) {
		AnimInstsance->Montage_Play(HitReactMontage);
		AnimInstsance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit(const FVector& ImpactPoint)
{
	//DrawDebugSphere(GetWorld(), ImpactPoint, 25.f, 12, FColor::Orange, false, 5.f);

	DirectionalReact(ImpactPoint);

	if (HitSound) {
		UGameplayStatics::PlaySoundAtLocation(
			this, HitSound, ImpactPoint);
	}

	if (HitParticles && GetWorld()) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}

}

void AEnemy::DirectionalReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	//Lover Impact Point to the Enemy's Acotr Loactioon z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	//벡터를 가져와 정규화 한다음 ToHit에 저장
	//수평 z값을 얻기 위해 ImpactPoint 대신에 ImpactLowered를 제작해서 사용
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	//중요
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	//Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);


	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0) {
		Theta *= -1.f;
	}

	FName Section = FName("FromBack");

	if (Theta >= -45.f && Theta < 45.f) {
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f) {
		Section = FName("FromLeft");
	}
	else  if (Theta >= 45.f && Theta < 135.f) {
		Section = FName("FromRight");
	}
	PlayHitReactMontage(Section);

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta));
	}
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.F);
}

