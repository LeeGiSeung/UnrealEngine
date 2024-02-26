// Fill out your copyright notice in the Description page of Project Settings.
#include "Tank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


ATank::ATank(){
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    SpringArm->SetupAttachment(RootComponent);
    //RootComponent = BasePawn의 CapsuleComp

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
}

void ATank::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
    //상위 버전 호출
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATank::Move);
    PlayerInputComponent->BindAxis(TEXT("TURN"), this ,&ATank::Turn);

    PlayerInputComponent->BindAction(TEXT("Fire"),IE_Pressed,this, &ATank::Fire);

}

void ATank::Tick(float DeltaTime){
    Super::Tick(DeltaTime);

    if(PlayerController){
        FHitResult HitResult;
        PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false,HitResult);
        RotateTurret(HitResult.ImpactPoint);
    }
}

void ATank::HandleDestruction(){
    Super::HandleDestruction();
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
    bAlive = false;
}

void ATank::BeginPlay(){
    Super::BeginPlay();

    PlayerController = Cast<APlayerController>(GetController());

}

void ATank::Move(float Value){
    // UE_LOG(LogTemp,Warning,TEXT("Value : %f"),Value);

    FVector DeltaLocation= FVector::ZeroVector;
    //X = Value * DeltaTime * Speed     DeltaTime : Tick이 선언되는 간격            his : UWorld
    DeltaLocation.X = Value * Speed * UGameplayStatics::GetWorldDeltaSeconds(this);
    //true : 스윕기능 활성화
    AddActorLocalOffset(DeltaLocation, true);
}

void ATank::Turn(float Value)
{
    FRotator DeltaRotation = FRotator::ZeroRotator;
    DeltaRotation.Yaw = Value * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
    AddActorLocalRotation(DeltaRotation, true);
}