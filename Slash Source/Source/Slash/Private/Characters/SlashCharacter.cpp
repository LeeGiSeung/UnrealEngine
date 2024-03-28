// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

// Sets default values
ASlashCharacter::ASlashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));
	
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRight);

	PlayerInputComponent->BindAction(FName("Jump"),IE_Pressed,this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);

}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	ActionState = EActionState::EAS_HitReaction;
}

void ASlashCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if ((Controller != nullptr) && Value != 0.f) {
		const FRotator ControlRotation =  GetControlRotation();
		const FRotator YaWRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Directrion = FRotationMatrix(YaWRotation).GetUnitAxis(EAxis::X);

		//FVector Forward = GetActorForwardVector();
		AddMovementInput(Directrion, Value);
	}
}

void ASlashCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if ((Controller != nullptr) && Value != 0.f) {
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YaWRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Directrion = FRotationMatrix(YaWRotation).GetUnitAxis(EAxis::Y);

		//FVector Forward = GetActorForwardVector();
		AddMovementInput(Directrion, Value);
		
		//FVector Right = GetActorRightVector();
		//AddMovementInput(Right, Value);
	}
}

void ASlashCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ASlashCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	//땅에 떨어진 장비 줍기
	if (OverlappingItem) 
	{
		EquipWeapon(OverlappingWeapon);
	}
	else 
	//가지고 있는 장비 착용 / 해제
	{
		if (CanDisarm()) {
			DisArm();
		}
		else if (CanArm()) {
			Arm();
		}
	}
}

void ASlashCharacter::Attack()
{	
	Super::Attack();

	if(CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
	
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocekt"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}

void ASlashCharacter::DisArm()
{
	PlayEquipMontage(FName("UnEquip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::AttackWeaponToHand()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocekt"));
	}
}

void ASlashCharacter::FinishEquippping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstsance = GetMesh()->GetAnimInstance();
	if (AnimInstsance && EquipMontage) {
		AnimInstsance->Montage_Play(EquipMontage);
		AnimInstsance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}





