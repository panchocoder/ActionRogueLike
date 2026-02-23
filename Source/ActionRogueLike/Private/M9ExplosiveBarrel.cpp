// Fill out your copyright notice in the Description page of Project Settings.


#include "M9ExplosiveBarrel.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
AM9ExplosiveBarrel::AM9ExplosiveBarrel()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Comp");
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	ForceComp = CreateDefaultSubobject<URadialForceComponent>("ForceComp");
	ForceComp->SetupAttachment(MeshComp);


	//Leaving this on applies small constant force via component 'tick' (Optional)
	ForceComp->SetAutoActivate(false);

	ForceComp->Radius = 750.0f;
	ForceComp->ImpulseStrength = 2500.0f; //Alternative: 200000.0 if bImpulseVelChange = false
	// Optional, ignores 'Mass' of other objects (if false, the impulse strength will be much higher to push most objects depending on Mass)
	ForceComp->bImpulseVelChange = true;

	// Optional, default contructor of component already adds 4 object types to affect, excluding WorldDynamic
	ForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);

	//Binding either in constructor or in PostInitializeComponents() below
	//MeshComp->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnActorHit);


}


void AM9ExplosiveBarrel::PostInitializeComponents()
{

	//Don't forget to call parent function 
	Super::PostInitializeComponents();

	MeshComp->OnComponentHit.AddDynamic(this, &AM9ExplosiveBarrel::OnActorHit);

}

void AM9ExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	ForceComp->FireImpulse();

}