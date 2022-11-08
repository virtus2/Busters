// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UCapsuleComponent;
class UWidgetComponent;
class UAnimationAsset;
class USoundCue;
enum class EWeaponType : uint8;

UCLASS()
class BUSTERS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TObjectPtr<UAnimationAsset> FireAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TObjectPtr<USoundCue> FireSound;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UWidgetComponent> PickUpWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FTransform ADSTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;

	//
	// Sway
 	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float MaxSwayDegree = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float SwaySpeed = 4.f;

	//
	// Ammo and Magazines
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int MaxMagazine = 30;
	int CurrentMagazine = 0;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void Fire(const FVector& HitTarget, const bool bADS);

public:
	inline TObjectPtr<USkeletalMeshComponent> GetSkeletalMesh() { return SkeletalMesh; };
	inline FTransform GetADSTransform() { return ADSTransform; };
	inline EWeaponType GetWeaponType() { return WeaponType; };
	inline float GetMaxSwayDegree() { return MaxSwayDegree; };
	inline float GetSwaySpeed() { return SwaySpeed; };

};
