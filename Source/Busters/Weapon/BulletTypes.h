#pragma once

UENUM(BlueprintType)
enum class EBulletType : uint8
{
	EBT_5mm UMETA(DisplayName = "5.56mm"),
	EBT_7mm UMETA(DisplayName = "7.62mm"),
	EBT_9mm UMETA(DisplayName = "9mm"),

	EBT_Max UMETA(DisplayName = "DefaultMax")
};