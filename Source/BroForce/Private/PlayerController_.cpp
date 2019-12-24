// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController_.h"
#include "..\Public\PlayerController_.h"
#include "../Public/CustomUtils.h"
#include "Camera/CameraComponent.h"


// Sets default values
APlayerController_::APlayerController_()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>("Root");
	
	rigidBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RigidBody"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	rigidBody->SetStaticMesh(CubeMeshAsset.Object);

	gameCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));

}

// Called when the game starts or when spawned
void APlayerController_::BeginPlay()
{
	Super::BeginPlay();
	BindInput();

}

// Called every frame
void APlayerController_::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	UpdateCamera(DeltaTime);
	CheckIfLanded();	
	
}

// Called to bind functionality to input
void APlayerController_::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	input = PlayerInputComponent;

}

void APlayerController_::CheckIfLanded()
{
	//extra gravity to simulate a heavier player
	rigidBody->SetPhysicsLinearVelocity(rigidBody->GetPhysicsLinearVelocity() - gameCamera->GetUpVector() * gravityMultiplier);


	//Check if landed by raycast
	FHitResult testHitResult;
	UWorld* TheWorld = this->GetWorld();
	FVector testStartFVector = rigidBody->GetComponentLocation();
	FVector testEndFVector = testStartFVector - gameCamera->GetUpVector() * 100;

	FCollisionQueryParams TraceParams;// (TEXT("TraceOfAwesome"));

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
	input->BindAction("Jump", IE_Pressed, this, &APlayerController_::Jump);
	input->BindAxis("MoveForward", this, &APlayerController_::MoveHorizontal);
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

void APlayerController_::Jump()
{
	if (isLanded) {
		rigidBody->SetPhysicsLinearVelocity(gameCamera->GetUpVector() * jumpForce);
	}	
}

void APlayerController_::MoveHorizontal(float value)
{
	if (value == 0.f)
		return;

	FVector pLV = rigidBody->GetPhysicsLinearVelocity();
	pLV.X = 0.f;

	if (isLanded)
		rigidBody->SetPhysicsLinearVelocity(pLV + gameCamera->GetRightVector() * value * moveSpeed);
	else
		rigidBody->SetPhysicsLinearVelocity(pLV + gameCamera->GetRightVector() * value * moveSpeed * airMovementFraction);

}
