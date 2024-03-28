// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Attack()
{

}

void ABaseCharacter::Die()
{
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter) {
		DirectionalReact(Hitter->GetActorLocation());
	}

	else Die();

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstsance = GetMesh()->GetAnimInstance();
	if (AnimInstsance && HitReactMontage) {
		AnimInstsance->Montage_Play(HitReactMontage);
		AnimInstsance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::DirectionalReact(const FVector& ImpactPoint)
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

	//if (GEngine) {
		//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta));
	//}
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.F);

}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound) {
		UGameplayStatics::PlaySoundAtLocation(
			this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld()) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes) {
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstsance = GetMesh()->GetAnimInstance();
	if (AnimInstsance && Montage)
	{
		AnimInstsance->Montage_Play(Montage);
		AnimInstsance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

int32 ABaseCharacter::PlayAttackMontage() 
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		AnimInstance->Montage_Stop(0.25, AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	//GetSafeNormal 벡터의 정규화된 복사본을 가져오고, 길이를 기준으로 그렇게 하는 것이 안전한지 확인합니다.
	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();

	TargetToMe *= WarpTargetDistance;

	return CombatTargetLocation + TargetToMe;

}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget) {
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox()) {
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}


