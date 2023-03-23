// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/AssetManager.h"
#include "ImageUtils.h"

#include "GameBoard.generated.h"

UCLASS()
class DICEDUNGEON_API AGameBoard : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGameBoard();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StartPosX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StartPosY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StartPosZ;

	FVector SpawnPosition;
	FRotator SpawnRotation;
	UObject* ObjGeneratedMap;

	///////// Wave function collapse

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = WaveFunctionCollapse)
		TArray<UTexture2D*> GridWTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = WaveFunctionCollapse)
		TArray<UTexture2D*> TilesImg;

	struct Cell {
		bool Collapsed = false;
		TArray<int32> Options;
	};

	int32 Obstacle, Up, Right, Down, Left, StrUp, StrRight;

	TArray<int32> Tiles;

	TArray<FColor> PixelMap;

	int32 DIM;

	int32 GridCellCompleted;
	int32 tries;

	// Create Grid
	TArray<Cell> Grid;
	TArray<Cell> NextGrid;
	TArray<Cell*> GridTemp;
	TArray<Cell> GridSortTemp;

	TArray<TArray<TArray<int32>>> Rules;

	void GenerateMap(UTexture2D* MapImg);

	void ValidOptions(TArray<int32>* ArrOptions, TArray<int32>* Valid);

	void ObjectLoad(FName AssetType, FName AssetName, FVector SpawnLoc, FRotator SpawnRot);

	void ObjectLoadRandom(FName AssetType, FVector SpawnLoc, FRotator SpawnRot);

	// Debug Fnc

	void ShowMatrixLog(TArray<Cell> GridLst, int32 DIM);

	void ShowPtrMatrixLog(TArray<Cell*> GridPtr, int32 DIM);

	void ShowListLog(TArray<Cell> GridLst);

	void ShowPtrListLog(TArray<Cell*> GridPtr);

	void ShowColl(TArray<Cell> GridLst, int32 Dimension);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};