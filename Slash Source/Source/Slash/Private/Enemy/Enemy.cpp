// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "Perception/PawnSensingComponent.h"

#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false);
	}
	
	EnemyController = Cast<AAIController>(GetController());

	MoveToTarget(PatrolTarget);

	if (PawnSensing) {
		//UE_LOG(LogTemp, Warning, TEXT("PawnSensing Component!"));
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
	
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstsance = GetMesh()->GetAnimInstance();
	if (AnimInstsance && HitReactMontage) {
		AnimInstsance->Montage_Play(HitReactMontage);
		AnimInstsance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::Die()
{
	//TODO Play Death Montage
	UAnimInstance* AnimInstsance = GetMesh()->GetAnimInstance();
	if (AnimInstsance && DeathMontage) {
		AnimInstsance->Montage_Play(DeathMontage);
		const int32 Selection = FMath::RandRange(0, 5);
		FName SelectionName = FName();
		switch (Selection)
		{
		case 0:
			SelectionName = FName("Death1");
			DeathPose = EDeathPose::EDP_Death1;
			break;
		case 1:
			SelectionName = FName("Death2");
			DeathPose = EDeathPose::EDP_Death2;
			break;
		case 2:
			SelectionName = FName("Death3");
			DeathPose = EDeathPose::EDP_Death3;
			break;
		case 3:
			SelectionName = FName("Death4");
			DeathPose = EDeathPose::EDP_Death4;
			break;
		case 4:
			SelectionName = FName("Death5");
			DeathPose = EDeathPose::EDP_Death5;
			break;
		case 5:
			SelectionName = FName("Death6");
			DeathPose = EDeathPose::EDP_Death6;
			break;
		default:
			break;
		}
		AnimInstsance->Montage_JumpToSection(SelectionName, DeathMontage);
	}
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(5.f);

}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	return DistanceTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(15.f);
		EnemyController->MoveTo(MoveRequest);
	
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (auto Target : PatrolTargets) {
		if (Target != PatrolTarget) {
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0) {
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];

	}
	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	
	if (EnemyState == EEnemyState::EES_Chasing) return;
	if (SeenPawn->ActorHasTag(FName("SlashCharacter")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		CombatTarget = SeenPawn;
			if (EnemyState != EEnemyState::EES_Attacking) {
			EnemyState = EEnemyState::EES_Chasing;
			MoveToTarget(CombatTarget);
			UE_LOG(LogTemp, Warning, TEXT("Seen Pawn, Now Chasing"));
		}
	}
	
}

	


void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState > EEnemyState::EES_Patrolling) {
		CheckCombatTarget();
	}
	else {
		CheckPatrolTarget();
	}	
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius)) {
		PatrolTarget = ChoosePatrolTarget();
		//MoveToTarget(PatrolTarget);
		const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget, CombatRadius)) {

		CombatTarget = nullptr;
		if (HealthBarWidget) {
			HealthBarWidget->SetVisibility(false);
		}
		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 125.f;
		MoveToTarget(PatrolTarget);
		UE_LOG(LogTemp, Warning, TEXT("Lose Interset"));
	}
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		//공격범위 밖에서 플레이어 쫒기
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget);
		UE_LOG(LogTemp, Warning, TEXT("Chase Target"));
	}
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking) 
	{
		//공격 범위 안에서 플레이어 쫒기
		EnemyState = EEnemyState::EES_Attacking;
		//TODO: Attack montage
		UE_LOG(LogTemp, Warning, TEXT("Attack"));
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	//DrawDebugSphere(GetWorld(), ImpactPoint, 25.f, 12, FColor::Orange, false, 5.f);

	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(true);
	}
	
	
	if (Attributes && Attributes->IsAlive()) {
		DirectionalReact(ImpactPoint);
	}
	else 
	{
		Die();
	}

	

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

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes && HealthBarWidget) {
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
	CombatTarget = EventInstigator->GetPawn();
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	MoveToTarget(CombatTarget);
	return DamageAmount;
}

