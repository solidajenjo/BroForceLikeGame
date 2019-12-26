// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shot.generated.h"

class UStaticMeshComponent;
class APlayerController_;

UCLASS()
class BROFORCE_API AShot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void shotHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void Go(const FVector& direction, const FVector& position);
	void Stop();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* rigidBody = nullptr;

	float shotSpeed = 0.f;
	float timeToLive = 0.f;
	float lifeTimer = 0.f;

	APlayerController_* owner = nullptr;

	bool stopped = true;

};
