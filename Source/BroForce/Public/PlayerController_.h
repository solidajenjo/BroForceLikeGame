// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define SHOT_POOL_SIZE 20
#define NARNIA FVector(100000.f, 100000.f, 100000.f)

#include <queue>

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerController_.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class USphereComponent;
class AShot;

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

	void BindInput();
	void BindColliders();

	void InertiaControl(float dt);
	void CheckIfLanded(float dt);
	void CheckIfUsingStair();
	void UpdateTimers(float dt);
	
	void UpdateCamera(float dt);
	void RotatePlayer();

	void ReleaseShot(AShot* shot);

	//Events 
	UFUNCTION()
	void FrontOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void FrontOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void RBOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void RBOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Control actions

	void Jump();
	void Shoot();
	void MoveHorizontal(float value);
	void MoveVertical(float value);
	void LookVertical(float value);
	void LookHorizontal(float value);
	void ManageAimAndOrientation();
	void ManageMovement(float dt);
	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* rigidBody = nullptr;

	UPROPERTY(VisibleDefaultsOnly)
	USphereComponent* frontCollider = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCameraComponent* gameCamera = nullptr;

	USceneComponent* root = nullptr;
	USceneComponent* aimRotator = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* aimPoint = nullptr;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float jumpForce = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float moveSpeed = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float stairSpeed = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float airMovementFraction = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float frictionFactor = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float gravityMultiplier = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player movement")
	float minZToBeLanded = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Game Camera")
	float camSpeed = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Game Camera")
	float camDistanceFactor = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Game Camera")
	float verticalOffset = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Shot paramters")
	float shotSpeed = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Shot paramters")
	float shotCooldown = 0.f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Shot paramters")
	float shotTimeToLive = 0.f;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
	bool bFrontCollision = false;


	bool isLanded = false;
	bool isInStair = false;
	bool isUsingStair = false;
	
	FRotator targetRot;

	float yLookFactor = 0.f;
	float xLookFactor = 0.f;
	float lookAngle = 0.f;
	float shotCooldownTimer = 0.f;

	float horizontalMovementAmount = 0.f;

	std::queue<AShot*> shotsReady;
};
