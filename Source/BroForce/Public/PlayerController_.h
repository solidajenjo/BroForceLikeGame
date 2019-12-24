// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerController_.generated.h"

class UCameraComponent;
class UStaticMeshComponent;

UCLASS()
class BROFORCE_API APlayerController_ : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerController_();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void CheckIfLanded();
	void BindInput();
	void UpdateCamera(float dt);

	//Actions

	void Jump();	
	void MoveHorizontal(float value);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* rigidBody = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCameraComponent* gameCamera = nullptr;

	USceneComponent* root = nullptr;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float jumpForce = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float moveSpeed = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float airMovementFraction = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float gravityMultiplier = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float minZToBeLanded = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Game Camera")
	float camSpeed = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Game Camera")
	float verticalOffset = 0.f;

	UInputComponent* input;

	bool isLanded = false;
};
