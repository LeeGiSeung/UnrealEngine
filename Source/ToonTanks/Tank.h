// Fill out your copyright notice in the Description page of Project Settings.

//중복 방지
#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "Tank.generated.h"

/**
 * 
 */
UCLASS()					//BasePawn클래스 상속함
class TOONTANKS_API ATank : public ABasePawn
{
	GENERATED_BODY()

public:
	ATank();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HandleDestruction();

	class APlayerController* GetTankPlayerController() const{return PlayerController; }

	bool bAlive = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category="Movement")
	float Speed = 200.f;

	UPROPERTY(EditAnywhere, Category="Movement")
	float TurnRate = 45.f;

	void Move(float Value);

	void Turn(float Value);
	//TankPlayerController
	APlayerController* PlayerController;
};
