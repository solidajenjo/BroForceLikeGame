// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelStructure_EU.h"
#include <Editor\UnrealEd\Public\EditorViewportClient.h>

FVector ALevelStructure_EU::GetCamPosition()
{
	FEditorViewportClient* client = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	return client->GetViewLocation();	
}

FVector ALevelStructure_EU::GetCamForwardPoint(float length)
{
	FEditorViewportClient* client = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	FVector editorCameraPositon = client->GetViewLocation();
	return editorCameraPositon + client->GetViewRotation().Vector() * length;
}
