// Fill out your copyright notice in the Description page of Project Settings.


#include "M9Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AM9Character::AM9Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;      // camera follows mouse (default true)
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	
	GetCharacterMovement()->bOrientRotationToMovement = false;     // movement doesn't overrides rotation (default true)
	bUseControllerRotationYaw = true;       //camera turns with mouse (default false)

}

// Called when the game starts or when spawned
void AM9Character::BeginPlay()
{
	Super::BeginPlay();
	
}



// Called every frame
void AM9Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AM9Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AM9Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AM9Character::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &AM9Character::PrimaryAttack);
	// Used generic name 'SecondaryAttack' for binding
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AM9Character::Dash);
	PlayerInputComponent->BindAction("BlackHoleAttack", IE_Pressed, this, &AM9Character::BlackHoleAttack);


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

}

void AM9Character::MoveForward(float Value)
{
	
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;


	AddMovementInput(ControlRot.Vector(), Value);

}

void AM9Character::MoveRight(float Value)
{

	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, Value);

}


void AM9Character::PrimaryAttack()
{
	FVector HandLocation = GetMesh()->GetSocketLocation("Gun_R");

	FTransform SpawnTM = FTransform(GetControlRotation(), HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);

}

/*
void AM9Character::PrimaryAttack()
{

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &AM9Character::PrimaryAttack_TimeElapsed, 0.2f, false);

}

void AM9Character::PrimaryAttack_TimeElapsed()
{

	SpawnProjectile(ProjectileClass);

}
*/

void AM9Character::BlackHoleAttack()
{
	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &AM9Character::BlackholeAttack_TimeElapsed, 0.2f, false);
}


void AM9Character::BlackholeAttack_TimeElapsed()
{
	SpawnProjectile(BlackHoleProjectileClass);
}



void AM9Character::Dash()
{
	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &AM9Character::Dash_TimeElapsed, 0.2f, false);
}


void AM9Character::Dash_TimeElapsed()
{
	SpawnProjectile(DashProjectileClass);
}



void AM9Character::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	if (ensureAlways(ClassToSpawn))
	{
		FVector HandLocation = GetMesh()->GetSocketLocation("Gun_R");

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;

		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		// Ignore Player
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		FVector TraceStart = CameraComp->GetComponentLocation();

		// endpoint far into the look-at distance (not too far, still adjust somewhat towards crosshair on a miss)
		FVector TraceEnd = CameraComp->GetComponentLocation() + (GetControlRotation().Vector() * 5000);

		FHitResult Hit;
		// returns true if we got to a blocking hit 
		if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjParams, Shape, Params))
		{

			// Overwrite trace end with impact point in world
			TraceEnd = Hit.ImpactPoint;

		}

		// Find new direction/rotation from hand pointing to impact point in world.
		FRotator ProjRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();

		FTransform SpawnTM = FTransform(ProjRotation, HandLocation);
		GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTM, SpawnParams);

	}


}
