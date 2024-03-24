#pragma once
UENUM(BluePrintType)
enum class ECharacterState : uint8 {
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "EquippedOneHandedWeapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "EquippedTwoHandedWeapon")
};

UENUM(BluePrintType)
enum class EActionState : uint8 {
	EAS_Unoccupied UMETA(DisplayName ="Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon")
};

UENUM(BluePrintType)
enum EDeathPose {
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_Death5 UMETA(DisplayName = "Death5"),
	EDP_Death6 UMETA(DisplayName = "Death6"),

	EDP_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BluePrintType)
enum class EEnemyState : uint8 {
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),

	EES_NoState UMETA(DisplayName = "NoState")
};