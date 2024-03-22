// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

// Sets default values
ASlashCharacter::ASlashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("SlashCharacter"));
	
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
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocekt"), this, this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
	}
	else 
	//가지고 있는 장비 착용 / 해제
	{
		if (CanDisarm()) {
			PlayEquipMontage(FName("UnEquip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if (CanArm()) {
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void ASlashCharacter::Attack()
{	
	Super::Attack();
	if(CanAttack()){
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
	
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}



bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}

void ASlashCharacter::Disarm()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::Arm()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocekt"));
	}
}

void ASlashCharacter::FinishEquippping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::PlayAttackMontage()
{
	Super::PlayAttackMontage();
	UAnimInstance* AnimInstsance = GetMesh()->GetAnimInstance();
	if (AnimInstsance && AttackMontage) {
		AnimInstsance->Montage_Play(AttackMontage);
		const int32 Selection = FMath::RandRange(0, 1);
		FName SelectionName = FName();
		switch (Selection)
		{
		case 0:
			SelectionName = FName("Attack1");
			break;
		case 1:
			SelectionName = FName("Attack2");
			break;
		default:
			break;
		}
		AnimInstsance->Montage_JumpToSection(SelectionName, AttackMontage);
	}
}

void ASlashCharacter::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstsance = GetMesh()->GetAnimInstance();
	if (AnimInstsance && EquipMontage) {
		AnimInstsance->Montage_Play(EquipMontage);
		AnimInstsance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}



