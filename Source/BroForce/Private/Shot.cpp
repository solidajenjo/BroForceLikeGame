// Fill out your copyright notice in the Description page of Project Settings.


#include "Shot.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerController_.h"
#include "CustomUtils.h"

// Sets default values
AShot::AShot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rigidBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RigidBody"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	rigidBody->SetStaticMesh(SphereMeshAsset.Object);
	RootComponent = rigidBody;
}

// Called when the game starts or when spawned
void AShot::BeginPlay()
{
	Super::BeginPlay();	
	rigidBody->OnComponentHit.AddDynamic(this, &AShot::shotHit);
}

// Called every frame
void AShot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (lifeTimer > 0.f)
		lifeTimer -= DeltaTime;
	else if (!stopped)
		Stop();
}

void AShot::shotHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//FString s = GetName() +  FString(" S -> ") + OtherComp->GetName() + FString("|") + OtherActor->GetName() +
	//	FString(" ") + OtherActor->GetActorLocation().ToString();
	//LOG_SCREEN_DT(s, 5.0f);
	Stop();
}

void AShot::Go(const FVector& direction, const FVector& position)
{
	SetActorLocation(position, false, nullptr, ETeleportType::TeleportPhysics);

	FRotator newRot = (position + direction - position).Rotation();	
	SetActorRotation(newRot);

	rigidBody->SetSimulatePhysics(true);
	rigidBody->SetPhysicsLinearVelocity(GetActorForwardVector() * shotSpeed);
	rigidBody->SetNotifyRigidBodyCollision(true);

	lifeTimer = timeToLive;

	stopped = false;
}

void AShot::Stop()
{
	rigidBody->SetPhysicsLinearVelocity(FVector::ZeroVector);
	rigidBody->SetSimulatePhysics(false);
	rigidBody->SetNotifyRigidBodyCollision(false);

	owner->ReleaseShot(this);

	stopped = true;
}

