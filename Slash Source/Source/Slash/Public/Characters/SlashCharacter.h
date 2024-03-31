// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

class AItem;
class USlashOverlay;
class ASoul;
class ATreasure;
//enum 타입인걸 알리기 위해 E를 붙임

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Jump() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(class AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(class ATreasure* Treasure) override;

protected:
	virtual void BeginPlay() override;

	// Callback Input
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	virtual void Attack() override;

	/*Combat*/
	void EquipWeapon(AWeapon* Weapon);
	virtual void Die() override;

	/*
	Play Montage Functions
	*/
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;
	void PlayEquipMontage(FName SectionName);
	bool CanDisarm();
	bool CanArm();
	void DisArm();
	void Arm();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttackWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquippping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

private:
	//HUD 초기화
	void InitializeSlashOverlay();
	//HUD HealthBar Update
	void SetHUDHealth();

	bool IsUnoccupied();


	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* ViewCamera;
	UPROPERTY(VisibleAnywhere)
	AItem* OverlappingItem;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	class UAnimMontage* EquipMontage;

	UPROPERTY()
	USlashOverlay* SlashOverlay;



public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
