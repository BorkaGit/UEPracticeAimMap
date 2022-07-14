// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PingleStudioTestTaskCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;


// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

UCLASS(config=Game)
class APingleStudioTestTaskCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	APingleStudioTestTaskCharacter();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float TurnRateGamepad;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnUseItem;
protected:
	
	/** Fires a projectile. */
	void OnPrimaryAction();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);
	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY(EditDefaultsOnly, Category = "Actor To Be Spawned")
	TSubclassOf<AActor> ActorToBeSpawned;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Spawn" , meta = (ClampMin = "0",ClampMax = "10000.0"))
	float StartMaxRadius = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Spawn" , meta = (ClampMin = "0",ClampMax = "100"))
	int32 StartSphereNum = 15;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Spawn" , meta = (ClampMin = "0",ClampMax = "10000.0")) //Opportunity to ride the wave
	float PassingRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Spawn" , meta = (ClampMin = "0",ClampMax = "100")) //Opportunity to ride the wave
	int32 PassingNum = 10;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Spawn" , meta = (ClampMin = "0",ClampMax = "100"))
	float SphereDistance = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "WaveMultiplier" , meta = (ClampMin = "0",ClampMax = "100"))
	int32 WaveSphereNumMultiplier = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "WaveMultiplier" , meta = (ClampMin = "0",ClampMax = "100"))
	int32 WaveRadiusMultiplier = 5;
	
	UPROPERTY( BlueprintReadWrite, Category= "Waves" )
	int32 WaveNumber = 0;

private:
	void PostInitializeComponents();

	void OnCharacterSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SpawnSpheres(float Radius,int NumberSpheres);

	float CheckSpheres(float Radius,FVector StartLocation);
	
	void PassingMin(float MinRadius, int MinNumberSpheres,float MaxRadius, int MaxNumberSpheres); 

	void EraseSpheres(float Radius);

	void NewWaveMultiplier();
	
	bool WaveStart = false;

	float MinRadiusSphereNum;	
	
	void EndOfTheWave();

	FVector NewWavePlayerLoc;


public:
	UPROPERTY( BlueprintReadWrite, Category= "Wave" )
	int DestroyedSpheres = 0;
	
};

