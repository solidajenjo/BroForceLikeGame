// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController_.h"
#include "..\Public\PlayerController_.h"
#include "../Public/CustomUtils.h"

// Sets default values
APlayerController_::APlayerController_()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
	rigidBody->GetPhysicsLinearVelocity();
	auto t = rigidBody->GetPhysicsLinearVelocity();
	if (abs(t.Z) < 2.5f)
		isLanded = true;
	else
		isLanded = false;
}

void APlayerController_::BindInput()
{
	input->BindAction("Jump", IE_Pressed, this, &APlayerController_::Jump);
	input->BindAxis("MoveForward", this, &APlayerController_::MoveHorizontal);
}

void APlayerController_::Jump()
{
	if (isLanded) {
		LOG_SCREEN_DT("jump", 0.5f);
		rigidBody->SetPhysicsLinearVelocity(rigidBody->GetUpVector() * jumpForce);
	}	
}

void APlayerController_::MoveHorizontal(float value)
{
	if (value == 0.f)
		return;

	FVector pLV = rigidBody->GetPhysicsLinearVelocity();
	pLV.X = 0.f;
	if (isLanded)
		rigidBody->SetPhysicsLinearVelocity(pLV + rigidBody->GetForwardVector() * value * moveSpeed + rigidBody->GetUpVector() * 2.f);
	else
		rigidBody->SetPhysicsLinearVelocity(pLV + rigidBody->GetForwardVector() * value * moveSpeed);
	FString m = FString("Move: ") + rigidBody->GetPhysicsLinearVelocity().ToString();
	LOG_SCREEN_DT(m, 0.05f);
}
