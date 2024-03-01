// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/SlashCharacter.h"

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
}

void ASlashCharacter::MoveForward(float Value)
{
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

