// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"
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
	

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass) {
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::Die()
{

	EnemyState = EEnemyState::EES_Dead;

	PlayDeathMontage();
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan = 8.f);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

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
		MoveRequest.SetAcceptanceRadius(50.f);
		EnemyController->MoveTo(MoveRequest);
	
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
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

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("Select"));
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		//EnemyState == EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Chasing &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldChaseTarget) 
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
	
}

void AEnemy::Attack()
{
	EnemyState = EEnemyState::EES_Engaged;
	Super::Attack();
	PlayAttackMontage();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if (Attributes && HealthBarWidget) {
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

}

int32 AEnemy::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);

	if (Pose < EDeathPose::EDP_MAX) 
	{
		DeathPose = Pose;
	}

	return Selection;
	
}

bool AEnemy::CanAttack()
{
	bool bCanAttack = IsInSideAttackRadius() && !IsAttacking() && !IsDead() && !IsEngaged();
	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);

}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	if (EnemyState > EEnemyState::EES_Patrolling) {
		CheckCombatTarget();
	}
	else {
		CheckPatrolTarget();
	}	
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon();
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius)) {
		PatrolTarget = ChoosePatrolTarget();
		//MoveToTarget(PatrolTarget);
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutSideCombatRadius()) {
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged()) StartPatroling();
		
	}
	else if (IsOutSideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		//공격범위 밖에서 플레이어 쫒기
		if (!IsEngaged()) ChaseTarget();;
	}
	else if (CanAttack())
	{
		//공격 범위 안에서 플레이어 쫒기
		StartAttackTimer();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if(!IsDead()) ShowHealthBar();
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	StopAttackMontage();
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	if (IsInSideAttackRadius()) {
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutSideAttackRadius()) {
		ChaseTarget();
	}
	return DamageAmount;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon) {
		EquippedWeapon->Destroy();
	}
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

void AEnemy::StartPatroling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

bool AEnemy::IsOutSideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutSideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInSideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

