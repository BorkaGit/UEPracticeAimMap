// Copyright Epic Games, Inc. All Rights Reserved.

#include "PingleStudioTestTaskProjectile.h"
#include "PingleStudioTestTaskCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"


APingleStudioTestTaskProjectile::APingleStudioTestTaskProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &APingleStudioTestTaskProjectile::OnHit);		// set up a notification for when this component hits something blocking

	PSTCharacter = CreateDefaultSubobject<APingleStudioTestTaskCharacter>("ProjectileClass");
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void APingleStudioTestTaskProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if(Hit.GetActor()->ActorHasTag(TEXT("SpawnSphere")))
	{
		Hit.GetActor()->Destroy();
	}
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		PSTCharacter->DestroyedSpheres++;
		Destroy();
	}
}