// Copyright Epic Games, Inc. All Rights Reserved.

#include "PingleStudioTestTaskCharacter.h"
#include "PingleStudioTestTaskProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/KismetSystemLibrary.h"
//#include "MathUtil.h"

DEFINE_LOG_CATEGORY_STATIC(LogSphere, All, All)
DEFINE_LOG_CATEGORY_STATIC(LogObject, All, All)
#define PI (3.1415926535897932f)

USphereComponent* CharacterSphere;
// APingleStudioTestTaskCharacter

APingleStudioTestTaskCharacter::APingleStudioTestTaskCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Sphere on character
	CharacterSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CharacterSphere"));
	CharacterSphere->SetupAttachment(GetRootComponent());
	CharacterSphere->InitSphereRadius(StartMaxRadius);
	CharacterSphere->SetCollisionResponseToAllChannels(ECR_Overlap);

}

void APingleStudioTestTaskCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	CharacterSphere->OnComponentBeginOverlap.AddDynamic(this,&APingleStudioTestTaskCharacter::OnCharacterSphereBeginOverlap);
}
	
void APingleStudioTestTaskCharacter::OnCharacterSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APingleStudioTestTaskCharacter>(OtherActor))
	{
		UE_LOG(LogSphere,Warning,TEXT("BEMPS"));
	}
}



void APingleStudioTestTaskCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	WaveNumber++;
	//SpawnSpheres(StartMaxRadius,StartSphereNum);
	NewWavePlayerLoc = GetActorLocation();
	PassingMin(PassingRadius,PassingNum,StartMaxRadius,StartSphereNum);
	

}
	

void APingleStudioTestTaskCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &APingleStudioTestTaskCharacter::OnPrimaryAction);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APingleStudioTestTaskCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APingleStudioTestTaskCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APingleStudioTestTaskCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &APingleStudioTestTaskCharacter::LookUpAtRate);
}

void APingleStudioTestTaskCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
	
}

void APingleStudioTestTaskCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void APingleStudioTestTaskCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void APingleStudioTestTaskCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APingleStudioTestTaskCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APingleStudioTestTaskCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void APingleStudioTestTaskCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool APingleStudioTestTaskCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APingleStudioTestTaskCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &APingleStudioTestTaskCharacter::EndTouch);

		return true;
	}
	
	return false;
}

void APingleStudioTestTaskCharacter::SpawnSpheres(float Radius, int NumberSpheres)
{
	
	for (int i = 0; i< NumberSpheres ; i++ )
	{

		float SpawnRadius = FMath::RandRange(5.0f,Radius);
		
		float angle = FMath::RandRange(0,180) * i;
		FVector PlayerLocaiton = GetActorLocation();
		FActorSpawnParameters SpawnInfo;
		float LocX = PlayerLocaiton.X + SpawnRadius * FMath::Cos(angle * (PI/180));
		float LocY = PlayerLocaiton.Y + SpawnRadius * FMath::Sin(angle * (PI/180));
		float LocZ = PlayerLocaiton.Z;
		FVector LocRadius = FVector(LocX,LocY,LocZ);
		if (CheckSpheres(SphereDistance,LocRadius) == 0)
		{
			GetWorld()->SpawnActor<AActor>(ActorToBeSpawned, LocRadius, GetActorRotation(),SpawnInfo);
		}
		else
		{
			i--;
		}
	}
}


void APingleStudioTestTaskCharacter::PassingMin(float MinRadius, int MinNumberSpheres,float MaxRadius, int MaxNumberSpheres)
{
	SpawnSpheres(MinRadius,MinNumberSpheres); //Спавнит минимальное необходимое количество
	SpawnSpheres(MaxRadius,MaxNumberSpheres-MinNumberSpheres); // добавляет спавн тех сфер что остались
	MinRadiusSphereNum = CheckSpheres(MinRadius,NewWavePlayerLoc);
	WaveStart = true;
}

void APingleStudioTestTaskCharacter::EndOfTheWave()
{
	
		EraseSpheres(StartMaxRadius);
		WaveStart = false;
		NewWaveMultiplier();
		NewWavePlayerLoc = GetActorLocation();
		PassingMin(PassingRadius,PassingNum,StartMaxRadius,StartSphereNum);
		WaveNumber++;
	
}

float APingleStudioTestTaskCharacter::CheckSpheres(float Radius,FVector StartLocation)
{
	// Set what actors to seek out from it's collision channel
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// Ignore any specific actors
	TArray<AActor*> IgnoreActors;

	IgnoreActors.Init(this, 1);
	

	TArray<AActor*> outActors;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), StartLocation, Radius, TraceObjectTypes, nullptr, IgnoreActors, outActors);
	
	
	return outActors.Num();
}


void APingleStudioTestTaskCharacter::Tick(float DeltaTime)
{
	Super::Tick (DeltaTime);

	if((MinRadiusSphereNum - CheckSpheres(PassingRadius,NewWavePlayerLoc)) >= PassingNum  && WaveStart == true)
	{
		EndOfTheWave();
	}
}

void APingleStudioTestTaskCharacter::EraseSpheres(float Radius)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Init(this, 1);
	
	TArray<AActor*> outActors;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), NewWavePlayerLoc, Radius, TraceObjectTypes, nullptr, IgnoreActors, outActors);
	
	for (AActor* erasedActor : outActors) {
		UE_LOG(LogObject,Warning, TEXT("ErasedSphere: %s"), *erasedActor->GetName());
		erasedActor->Destroy();
	}
}

void APingleStudioTestTaskCharacter::NewWaveMultiplier()
{
	StartSphereNum = StartSphereNum/100 * WaveSphereNumMultiplier + StartSphereNum;
	StartMaxRadius = StartMaxRadius/100 * WaveRadiusMultiplier + StartMaxRadius;
}