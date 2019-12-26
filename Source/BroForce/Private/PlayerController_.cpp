// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController_.h"
#include "CustomUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Shot.h"

// Sets default values
APlayerController_::APlayerController_()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>("Root");
	
	rigidBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RigidBody"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	rigidBody->SetStaticMesh(CubeMeshAsset.Object);

	aimRotator = CreateDefaultSubobject<USceneComponent>("AimRotator");
	aimRotator->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);

	aimPoint = CreateDefaultSubobject<USceneComponent>("AimPoint");
	aimPoint->AttachToComponent(aimRotator, FAttachmentTransformRules::KeepRelativeTransform);

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

	frontCollider->OnComponentBeginOverlap.AddDynamic(this, &APlayerController_::FrontOverlapBegin);
	frontCollider->OnComponentEndOverlap.AddDynamic(this, &APlayerController_::FrontOverlapEnd);

	for (size_t i = 0; i < SHOT_POOL_SIZE; ++i) 
	{
		AShot* newShot = GetWorld()->SpawnActor<AShot>(AShot::StaticClass(), NARNIA, FRotator::ZeroRotator);
		newShot->owner = this;
		newShot->shotSpeed = shotSpeed;
		newShot->rigidBody->SetEnableGravity(false);		
		newShot->rigidBody->BodyInstance.bLockYTranslation = true;
		newShot->rigidBody->BodyInstance.bLockXRotation = true;
		newShot->rigidBody->BodyInstance.bLockYRotation = true;
		newShot->rigidBody->BodyInstance.bLockZRotation = true;
		newShot->timeToLive = shotTimeToLive;
		shotsReady.push(newShot);
	}
}

// Called every frame
void APlayerController_::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	UpdateCamera(DeltaTime);
	UpdateTimers(DeltaTime);
	InertiaControl(DeltaTime);
	CheckIfLanded(DeltaTime);	
	RotatePlayer();
	ManageAimAndOrientation();

	//if (bFrontCollision) {
	//	LOG_SCREEN_DT("Front col", 0.05f);
	//}
	//else
	//	LOG_SCREEN_DT("NO Front col", 0.05f);
}

// Called to bind functionality to input
void APlayerController_::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerController_::InertiaControl(float dt)
{
	if (isLanded) 
	{
		FVector pLV = rigidBody->GetPhysicsLinearVelocity();
		pLV = pLV - pLV * (frictionFactor * dt);

		rigidBody->SetPhysicsLinearVelocity(pLV);
	}
}

void APlayerController_::CheckIfLanded(float dt)
{
	FVector pLV = rigidBody->GetPhysicsLinearVelocity();

	//extra gravity to simulate a heavier player
	rigidBody->SetPhysicsLinearVelocity(rigidBody->GetPhysicsLinearVelocity() - (gameCamera->GetUpVector() * dt) * gravityMultiplier);


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

void APlayerController_::UpdateTimers(float dt)
{
	if (shotCooldownTimer > 0.f)
		shotCooldownTimer -= dt;

}

void APlayerController_::BindInput()
{
	Super::InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerController_::Jump);
	Super::InputComponent->BindAction("Shoot", IE_Pressed, this, &APlayerController_::Shoot);
	Super::InputComponent->BindAxis("MoveForward", this, &APlayerController_::MoveHorizontal);
	Super::InputComponent->BindAxis("LookVertical", this, &APlayerController_::LookVertical);
	Super::InputComponent->BindAxis("LookHorizontal", this, &APlayerController_::LookHorizontal);
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

void APlayerController_::ReleaseShot(AShot* shot)
{
	shot->SetActorLocation(NARNIA);
	shotsReady.push(shot);
}

void APlayerController_::FrontOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bFrontCollision = true;
}

void APlayerController_::FrontOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bFrontCollision = false;
}

void APlayerController_::Jump()
{
	if (isLanded) {
		rigidBody->SetPhysicsLinearVelocity(gameCamera->GetUpVector() * jumpForce);
	}	
}

void APlayerController_::Shoot()
{
	if (shotCooldownTimer > 0.f || shotsReady.empty())
		return;

	shotCooldownTimer = shotCooldown;
	AShot* shot = shotsReady.front();
	shotsReady.pop();
	FVector shotDirection = aimPoint->GetComponentLocation() - rigidBody->GetComponentLocation();
	shotDirection.Normalize();
	shot->Go(shotDirection, aimPoint->GetComponentLocation());
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

}

void APlayerController_::LookVertical(float value)
{
	if (value == 0.f)
		return;
	yLookFactor = -value;
}

void APlayerController_::LookHorizontal(float value)
{
	if (value == 0.f)
		return;
	xLookFactor = -value;
}

void APlayerController_::ManageAimAndOrientation()
{	
	aimRotator->SetWorldLocation(rigidBody->GetComponentLocation());
	float newLookAngle = FMath::Atan2(yLookFactor, xLookFactor);
	if (newLookAngle > -.7f || newLookAngle < -2.4f)
		lookAngle = newLookAngle;

	aimRotator->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0.f, FMath::RadiansToDegrees(lookAngle), 0.f)));

	bool isLookingLeft = false;
	if (lookAngle > -.7f && lookAngle < PI * .5f)
		isLookingLeft = true;

	targetRot = isLookingLeft ? FQuat::MakeFromEuler(FVector(0.f, 0.f, 0.f)).Rotator() : FQuat::MakeFromEuler(FVector(0.f, 0.f, 180.f)).Rotator();

}
