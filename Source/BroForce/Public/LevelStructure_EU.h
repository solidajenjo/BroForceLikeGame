// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityActor.h"
#include "LevelStructure_EU.generated.h"

/**
 * 
 */
UCLASS()
class BROFORCE_API ALevelStructure_EU : public AEditorUtilityActor
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
		FVector GetCamPosition();

	UFUNCTION(BlueprintCallable)
		FVector GetCamForwardPoint(float length);
};
