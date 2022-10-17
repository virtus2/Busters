// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class ABustersCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUSTERS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
		
public:
	friend class ABustersCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Weapon, Category = "Weapon")
	TObjectPtr<AWeapon> Weapon;

private:
	UPROPERTY()
	TObjectPtr<ABustersCharacter> Character;

public:	
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Weapon();

public:
	FTransform GetWeaponADSTransform();
	inline TObjectPtr<AWeapon> GetWeapon() { return Weapon; };
	inline void SetWeapon(TObjectPtr<AWeapon> WeaponToSet) { Weapon = WeaponToSet; };
};
