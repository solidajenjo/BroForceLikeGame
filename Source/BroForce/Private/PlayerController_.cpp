// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController_.h"
#include "..\Public\PlayerController_.h"
#include "../Public/CustomUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
APlayerController_::APlayerController_()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>("Root");
	
	rigidBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RigidBody"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	rigidBody->SetStaticMesh(CubeMeshAsset.Object);

	frontCollider = CreateDefaultSubobject<USphereComponent>(TEXT("FrontCollider"));
	frontCollider->SetSphereRadius(20.f);
	frontCollider->SetGenerateOverlapEvents(true);
	frontCollider->AttachToComponent(rigidBody, FAttachmentTransformRules::KeepRelativeTransform);

	gameCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));

}

// Called when the game starts or when spawned
void APlayerController_::BeginPlay()
{
	Super::BeginPlay();
	BindInput();

	targetRot = FQuat::MakeFromEuler(FVector(0.f, 0.f, 180.f)).Rotator();
}

// Called every frame
void APlayerController_::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	UpdateCamera(DeltaTime);
	InertiaControl();
	CheckIfLanded();	
	RotatePlayer();

	if (bFrontCollision) {
		LOG_SCREEN_DT("Front col", 0.05f);
	}
	else
		LOG_SCREEN_DT("NO Front col", 0.05f);
}

// Called to bind functionality to input
void APlayerController_::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerController_::InertiaControl()
{
	if (isLanded) 
	{
		FVector pLV = rigidBody->GetPhysicsLinearVelocity() * frictionFactor;

		rigidBody->SetPhysicsLinearVelocity(pLV);
	}
}

void APlayerController_::CheckIfLanded()
{
	FVector pLV = rigidBody->GetPhysicsLinearVelocity();

	//extra gravity to simulate a heavier player
	rigidBody->SetPhysicsLinearVelocity(rigidBody->GetPhysicsLinearVelocity() - gameCamera->GetUpVector() * gravityMultiplier);


	//Check if landed by raycast
	FHitResult testHitResult;
	UWorld* TheWorld = this->GetWorld();
	FVector testStartFVector = rigidBody->GetComponentLocation();
	FVector testEndFVector = testStartFVector - gameCamera->GetUpVector() * 100;

	FCollisionQueryParams TraceParams;

	if (TheWorld->LineTraceSingleByObjectType(testHitResult, testStartFVector, testEndFVector, ECC_WorldStatic, TraceParams))
	{		
		if (testHitResult.Distance < minZToBeLanded)
			isLanded = true;
		else
		{
			isLanded = false;
		}
	}
	else
	{
		isLanded = false;		
	}
		
	
}

void APlayerController_::BindInput()
{
	Super::InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerController_::Jump);
	Super::InputComponent->BindAxis("MoveForward", this, &APlayerController_::MoveHorizontal);
}


void APlayerController_::UpdateCamera(float dt)
{
	FVector camPos = gameCamera->GetComponentLocation();
	FVector playerPos = rigidBody->GetComponentLocation();
	if (playerPos.X < camPos.X) 
	{
		camPos.X -= camSpeed * dt;
	}
	if (playerPos.X > camPos.X)
	{
		camPos.X += camSpeed * dt;
	}
	if (playerPos.Z < camPos.Z + verticalOffset)
	{
		camPos.Z -= camSpeed * dt;
	}
	if (playerPos.Z > camPos.Z + verticalOffset)
	{
		camPos.Z += camSpeed * dt;
	}

	gameCamera->SetWorldLocation(camPos);
}

void APlayerController_::RotatePlayer()
{
	FRotator rBRot = rigidBody->GetComponentRotation();
	FRotator newRot = FMath::Lerp(rBRot, targetRot, 0.2f);
	rigidBody->SetWorldRotation(newRot);
}

void APlayerController_::Jump()
{
	if (isLanded) {
		rigidBody->SetPhysicsLinearVelocity(gameCamera->GetUpVector() * jumpForce);
	}	
}

void APlayerController_::MoveHorizontal(float value)
{
	if (abs(value) < 0.5f) { //Avoid thumbstick rebound
		return;
	}

	FVector pLV = rigidBody->GetPhysicsLinearVelocity();
	pLV.X = 0.f;

	if (isLanded)
		rigidBody->SetPhysicsLinearVelocity(pLV + gameCamera->GetRightVector() * value * moveSpeed);
	else
		rigidBody->SetPhysicsLinearVelocity(pLV + gameCamera->GetRightVector() * value * moveSpeed * airMovementFraction);

	FRotator rBRot = rigidBody->GetComponentRotation();
	if (value < 0.f) 
	{
		targetRot = FQuat::MakeFromEuler(FVector(0.f, 0.f, 0.f)).Rotator();
	}
	else
	{
		targetRot = FQuat::MakeFromEuler(FVector(0.f, 0.f, 180.f)).Rotator();
	}

}
