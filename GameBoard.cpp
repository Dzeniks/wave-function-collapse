// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBoard.h"

// Sets default values
AGameBoard::AGameBoard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StartPosX = 1900.f;
	StartPosY = 1900.f;
	StartPosZ = 0.f;

	SpawnPosition = FVector(StartPosX, StartPosY, StartPosZ);
	SpawnRotation = FRotator(0, 0, 0);

	// Wave Function Collapse 



	Obstacle = 0, Up = 1, Right = 2, Down = 3, Left = 4;// StrUp = 5, StrRight = 6;

	Tiles = { Obstacle, Up, Right, Down, Left }; //, StrUp, StrRight};

	DIM = 5;

	Rules = {
		//Blank rules array 0
		{ {Obstacle, Up, /*StrRight*/},	// Up Rule 
		{Obstacle, Right, /*StrUp*/}, // Right Rule
		{Obstacle, Down, /*StrRight*/}, // Down Rule
		{Obstacle, Left, /*StrUp*/} },// Left Rule
		//Up rules array 1
		{ {Right, Down, Left, /*StrUp*/},
		{Up, Down, Left, /*StrRight*/},
		{Obstacle, Down, /*StrRight*/},
		{Right, Down, Up, /*StrRight*/} },
		/*Right rules array 2*/
		{ {Right, Down, Left, /*StrUp*/},
		{Up, Down, Left, /*StrRight*/},
		{Up, Right, Left, /*StrUp*/},
		{Obstacle, Left, /*StrUp*/} },
		/*Down rules array 3*/
		{ {Obstacle, Up, /*StrRight*/},
		{Up, Down, Left, /*StrRight*/},
		{Up, Right, Left, /*StrUp*/},
		{Up, Right, Down, /*StrRight*/} },
		/*Left rules array 4*/
		{ {Right, Down, Left, /*StrUp*/},
		{Obstacle, Right, /*StrUp*/},
		{Up, Right, Left, /*StrUp*/},
		{Up, Right, Down, /*StrRight*/} },
		///*StrUp rules array 5*/
		//{ {Right, Down, Left, StrUp},
		//{Obstacle, Right, StrUp},
		//{ Left, Right, Up, StrUp},
		//{Obstacle, Left, StrUp} },
		///*StrRight rules array 6*/
		//{ {Obstacle ,Up, StrRight},
		//{Up, Down, Left, StrRight},
		//{Obstacle, Down, StrRight},
		//{Up, Right, Down, StrRight} }

	};

	GridCellCompleted = 0;
	tries = 0;

	// Create Grid
	Grid.SetNum(DIM * DIM);
	UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!! %i"), Grid.Num());
	for (int32 i = 0; i < (DIM * DIM); i++) {
		Grid[i] = { false , Tiles };
	}
	ShowMatrixLog(Grid, DIM);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *this->GetLevelTransform().ToString());

}

// Called when the game starts or when spawned
void AGameBoard::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, "Start");

	FString GridString;

	/////////////// Wave function colapsed

	while (true) {

		tries++;
		UE_LOG(LogTemp, Warning, TEXT("While Start %i"), tries);
		UE_LOG(LogTemp, Warning, TEXT("While Start %i"), GridCellCompleted);
		/////////////// Array Create for generation
		// Create Array copy
		GridTemp.SetNum(DIM * DIM);
		for (int32 i = 0; i < Grid.Num(); i++) {
			GridTemp[i] = &Grid[i];
		}


		for (int32 i = 0; i < GridTemp.Num(); i++) {
			if (GridTemp[i]->Collapsed == true) {
				GridTemp.RemoveAt(i);
				i = -1;
			}
		}
		// Sort array by count of options 
		//GridTemp.Sort([](const Cell& A, const Cell& B) { return A.Options.Num() < B.Options.Num();});
		GridTemp.HeapSort([](const Cell& A, const Cell& B) { return A.Options.Num() < B.Options.Num(); });


		ShowPtrMatrixLog(GridTemp, DIM);

		// Cut list on elements with least options
		int32 lengthOpt = GridTemp[0]->Options.Num();
		int32 StopIndex = 0;
		for (int32 i = 1; i < GridTemp.Num(); i++) {
			if (GridTemp[i]->Options.Num() > lengthOpt) {
				StopIndex = i;
				UE_LOG(LogTemp, Warning, TEXT("StopIndex %i"), StopIndex);
				break;
			}
		}
		if (StopIndex > 0) { GridTemp.SetNum(StopIndex); }

		ShowPtrMatrixLog(GridTemp, DIM);

		// Set GridCell on 1 of GridCellOptions from GridTemp

		int32 RandomIndex = FMath::RandRange(0, GridTemp.Num() - 1);
		if (!GridTemp[RandomIndex]->Collapsed) {
			GridTemp[RandomIndex]->Collapsed = GridTemp[RandomIndex]->Collapsed = true;
			GridTemp[RandomIndex]->Options = { GridTemp[RandomIndex]->Options[FMath::RandRange(0,  GridTemp[RandomIndex]->Options.Num() - 1)] };
			GridCellCompleted++;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Error collapsed"), );
		}

		ShowPtrMatrixLog(GridTemp, DIM);

		ShowMatrixLog(Grid, DIM);
		UE_LOG(LogTemp, Warning, TEXT("GridCell Collapsed %i"), tries);

		// Generate new grid with new entorpy on cells
		NextGrid.SetNum(DIM * DIM);
		for (int32 x = 0; x < DIM; x++) {
			for (int32 y = 0; y < DIM; y++) {
				int32 index = y + x * DIM;
				if (Grid[index].Collapsed) {
					NextGrid[index] = Grid[index];
				}
				else {
					TArray<int32> options = Tiles;
					// Look Above
					// Thats mean.I look on grid above, take rules for Grid. Exatly rules for grid down and check validation.
					if (x > 0) {
						Cell UpGrid = Grid[y + (x - 1) * DIM];
						TArray<int32> ValidOpt = {};
						for (int32 GridOption : UpGrid.Options) {
							TArray<int32> Valid = Rules[GridOption][2];
							for (int i = 0; i < Valid.Num(); i++) {
								ValidOpt.Add(Valid[i]);
							}
						}
						ValidOptions(&options, &ValidOpt);
					}
					// Look Right
					if (y < DIM - 1) {
						Cell RightGrid = Grid[y + 1 + x * DIM];
						TArray<int32> ValidOpt = {};
						for (int32 GridOption : RightGrid.Options) {
							TArray<int32> Valid = Rules[GridOption][3];
							for (int i = 0; i < Valid.Num(); i++) {
								ValidOpt.Add(Valid[i]);
							}
						}
						ValidOptions(&options, &ValidOpt);
					}
					// Look Down
					if (x < DIM - 1) {
						Cell DownGrid = Grid[y + (x + 1) * DIM];
						TArray<int32> ValidOpt = {};
						for (int32 GridOption : DownGrid.Options) {
							TArray<int32> Valid = Rules[GridOption][0];
							for (int i = 0; i < Valid.Num(); i++) {
								ValidOpt.Add(Valid[i]);
							}
						}
						ValidOptions(&options, &ValidOpt);
					}
					// Look Left
					if (y > 0) {
						Cell LeftGrid = Grid[y - 1 + x * DIM];
						TArray<int32> ValidOpt = {};
						for (int32 GridOption : LeftGrid.Options) {
							TArray<int32> Valid = Rules[GridOption][1];
							for (int i = 0; i < Valid.Num(); i++) {
								ValidOpt.Add(Valid[i]);
							}
						}
						ValidOptions(&options, &ValidOpt);
					}
					NextGrid[index] = { false, options };
				}
			}
		}

		Grid = NextGrid;

		GridString = "";
		for (int32 i = 0; i < Grid.Num(); i++) {
			if (i % DIM == 0) {
				GridString.Append("\n");
			}
			GridString.Append(*FString::Printf(TEXT("%i, "), Grid[i].Collapsed));
		}
		UE_LOG(LogTemp, Warning, TEXT("Grid Collapsed: \n%s"), *GridString);

		ShowMatrixLog(Grid, DIM);


		if (GridCellCompleted >= Grid.Num()) {
			UE_LOG(LogTemp, Warning, TEXT("END %i \n\n\n\n\n"), GridCellCompleted);
			//UE_LOG(LogTemp, Warning, TEXT("Grid 8 is collapsed %s \n\n\n\n\n"), Grid[8].Collapsed);
			ShowMatrixLog(NextGrid, DIM);
			ShowMatrixLog(Grid, DIM);
			break;
		}
		if (tries >= (DIM * DIM * 10)) {
			UE_LOG(LogTemp, Warning, TEXT("ERROR \n\n\n\n\n"));
			ShowMatrixLog(NextGrid, DIM);
			ShowMatrixLog(Grid, DIM);
			break;
		}
	}

	// Show End Grid

	GridString = "";
	for (int32 i = 0; i < Grid.Num(); i++) {
		if (i % DIM == 0) {
			GridString.Append("\n");
		}
		GridString.Append(*FString::Printf(TEXT("%i, "), Grid[i].Options[0]));
	}

	UE_LOG(LogTemp, Warning, TEXT("Finally Grid: \n%s"), *GridString);

	/////////////// Create map from grid array 

	// Create array with tiles
	GridWTiles.SetNum(Grid.Num());

	for (int32 x = 0; x < DIM; x++) {
		for (int32 y = 0; y < DIM; y++) {
			int32 index = y + x * DIM;
			for (int32 i = 0; i < Tiles.Num(); i++) {
				if (Grid[index].Options[0] == Tiles[i]) {
					GridWTiles[index] = TilesImg[i];
				}
			}
		}
	}

	///// Create pixel array from GridWTiles (TArray pixelMap) 
	// Setup

	// Full array Extract
	for (int32 j = 0; j < DIM; j++) {
		// Full tile pixel extract only for row of tiles
		for (int32 Y = 0; Y < 4; Y++) {
			// Row pixel extract only for row of tiles
			for (int32 i = 0; i < DIM; i++) {
				Image = GridWTiles[i + j * DIM];
				FTexture2DMipMap* MipMap = &Image->GetPlatformData()->Mips[0];
				const FColor* FormatedImageData = static_cast<const FColor*>(Image->GetPlatformData()->Mips[0].BulkData.LockReadOnly());
				//UE_LOG(LogTemp, Warning, TEXT("%i %i %s"), i, j, *Image->GetFName().ToString());

				for (int32 X = 0; X < MipMap->SizeX; X++)
				{
					FColor PixelColor = FormatedImageData[X + Y * MipMap->SizeX];
					PixelMap.Add(PixelColor);
					//UE_LOG(LogTemp, Warning, TEXT("%s"), *PixelColor.ToString());
				}
				Image->GetPlatformData()->Mips[0].BulkData.Unlock();
			}
		}
	}


	// Path tile = #6E3C0D = Brown
	// Obstacles tile = #00ff00 = Green
	// StartTile =  #ff0000 =
	// EndTile = #0000ff =  
	FColor PathTile = FColor(110, 60, 13, 255);
	FColor ObstacleTile = FColor(0, 255, 0, 255);

	FColor StartTile = FColor(0, 0, 255, 255);
	FColor EndTile = FColor(255, 0, 0, 255);

	// Set Start and End tile

	for (int32 i = 0; i < PixelMap.Num(); i++) {
		if (PixelMap[i] == PathTile) {
			PixelMap[i] = StartTile;
			break;
		}
	}
	for (int32 i = PixelMap.Num() - 1; i > 0; i--) {
		UE_LOG(LogTemp, Warning, TEXT("%i"), i);
		if (PixelMap[i] == PathTile) {
			PixelMap[i] = EndTile;
			break;
		}
	}

	// Create map from pixel (color) array

	UTexture2D* GeneratedMap = UTexture2D::CreateTransient(20, 20, PF_B8G8R8A8, FName("GeneratedMap"));
	GeneratedMap->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	GeneratedMap->SRGB = 1;
	GeneratedMap->LODBias = 0;
	GeneratedMap->LODGroup = TextureGroup::TEXTUREGROUP_16BitData;
	GeneratedMap->MipGenSettings = TMGS_NoMipmaps;
	GeneratedMap->AddToRoot();
	GeneratedMap->UpdateResource();
	auto stride = (int32)(sizeof(uint8) * 4); // for r, g, b, a
	UE_LOG(LogTemp, Warning, TEXT("%i."), stride);
	FMemory::Memcpy(GeneratedMap->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE), PixelMap.GetData(), 20 * 20 * stride);
	GeneratedMap->GetPlatformData()->Mips[0].BulkData.Unlock();
	GeneratedMap->UpdateResource();

	Image = GeneratedMap;

	UE_LOG(LogTemp, Warning, TEXT("Wave Function End"));

	// Generate Map
	GenerateMap(GeneratedMap);

}

// Called every frame
void AGameBoard::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}


void AGameBoard::GenerateMap(UTexture2D* MapImg) {
	// Spawn object on map // Generate Map

	FTexture2DMipMap* MipMap = &MapImg->GetPlatformData()->Mips[0];
	const FColor* FormatedImageData = static_cast<const FColor*>(MapImg->GetPlatformData()->Mips[0].BulkData.LockReadOnly());

	// Path tile = #6E3C0D = Brown
	// Obstacles tile = #00ff00 = Green
	// StartTile =  #ff0000 =
	// EndTile = #0000ff =  
	FColor PathTile = FColor(110, 60, 13, 255);
	FColor ObstacleTile = FColor(0, 255, 0, 255);

	FColor StartTile = FColor(0, 0, 255, 255);
	FColor EndTile = FColor(255, 0, 0, 255);


	for (int32 X = 0; X < MipMap->SizeX; X++)
	{
		for (int32 Y = 0; Y < MipMap->SizeY; Y++)
		{
			FColor PixelColor = FormatedImageData[X + Y * MipMap->SizeX];

			if (PixelColor == PathTile) {
				ObjectLoad(FName("PathsType"), FName("Path"), SpawnPosition, SpawnRotation);
				//ObjectLoadRandom(FName("PathsType"), SpawnPosition, SpawnRotation);
			}
			if (PixelColor == ObstacleTile) {
				ObjectLoadRandom(FName("ObstaclesType"), SpawnPosition, SpawnRotation);
			}
			if (PixelColor == EndTile) {
				ObjectLoad(FName("PathsType"), FName("EndTile"), SpawnPosition, SpawnRotation);
				UE_LOG(LogTemp, Warning, TEXT("EndTile"));
			}
			if (PixelColor == StartTile) {
				ObjectLoad(FName("PathsType"), FName("StartTile"), SpawnPosition, SpawnRotation);
				UE_LOG(LogTemp, Warning, TEXT("StartTile"));
			}

			SpawnRotation = FRotator(0, 90 * FMath::RandRange(0, 3), 0);
			SpawnPosition.Y -= 200.f;
		}
		SpawnPosition.Y = StartPosY;
		SpawnPosition.X -= 200.f;
	}
	MapImg->GetPlatformData()->Mips[0].BulkData.Unlock();
}

void AGameBoard::ValidOptions(TArray<int32>* ArrOptions, TArray<int32>* Valid) {
	TArray<int32> DefArrOptions = *ArrOptions;
	TArray<int32> DefValid = *Valid;

	for (int i = DefArrOptions.Num() - 1; i >= 0; i--) {
		if (!(DefValid.Contains(DefArrOptions[i]))) {
			DefArrOptions.RemoveAt(i);
		}
	}

	*ArrOptions = DefArrOptions;
	*Valid = DefValid;
}

void AGameBoard::ObjectLoad(FName AssetType, FName AssetName, FVector SpawnLoc, FRotator SpawnRot) {
	if (UAssetManager* Manager = UAssetManager::GetIfValid()) {

		FPrimaryAssetId AssetID(AssetType, AssetName);
		FSoftObjectPath Path = Manager->GetPrimaryAssetPath(AssetID);

		const FSoftClassPath ObjectPath(*Path.ToString());
		UClass* ObjectClass = ObjectPath.TryLoadClass<UObject>();
		if (ObjectClass != NULL) {
			UObject* Object = GetWorld()->SpawnActor<UObject>(ObjectClass, SpawnLoc, SpawnRot);
			AActor* ObjectActor = Cast<AActor>(Object);
			ObjectActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

void AGameBoard::ObjectLoadRandom(FName AssetType, FVector SpawnLoc, FRotator SpawnRot) {
	if (UAssetManager* Manager = UAssetManager::GetIfValid()) {
		TArray<FSoftObjectPath> ObjectPaths;

		Manager->GetPrimaryAssetPathList(FPrimaryAssetType(AssetType), ObjectPaths);

		const FSoftClassPath ObjectPath(*ObjectPaths[FMath::RandRange(0, ObjectPaths.Num() - 1)].ToString());
		UClass* ObjectClass = ObjectPath.TryLoadClass<UObject>();
		if (ObjectClass != NULL) {
			UObject* Object = GetWorld()->SpawnActor<UObject>(ObjectClass, SpawnLoc, SpawnRot);
			AActor* ObjectActor = Cast<AActor>(Object);
			ObjectActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

// SHOW me your mother FNC // Debug Fnc

void AGameBoard::ShowMatrixLog(TArray<Cell> GridList, int32 Dimension) {
	FString GridString;
	for (int32 i = 0; i < GridList.Num(); i++) {
		if (i % Dimension == 0) {
			GridString.Append("\n");
		}
		GridString.Append(*FString::Printf(TEXT("%i, "), GridList[i].Options.Num()));
	}
	UE_LOG(LogTemp, Warning, TEXT("Grid: \n%s"), *GridString);
}

void AGameBoard::ShowPtrMatrixLog(TArray<Cell*> GridListPtr, int32 Dimension) {
	TArray<Cell> GridList;
	GridList.SetNum(DIM * DIM);
	for (int32 u = 0; u < GridListPtr.Num(); u++) {
		GridList[u] = *GridListPtr[u];
	}
	FString GridString;
	for (int32 i = 0; i < GridListPtr.Num(); i++) {
		if (i % Dimension == 0) {
			GridString.Append("\n");
		}
		GridString.Append(*FString::Printf(TEXT("%i, "), GridList[i].Options.Num()));
	}
	UE_LOG(LogTemp, Warning, TEXT("Grid_PTR: \n%s"), *GridString);
}

void AGameBoard::ShowListLog(TArray<Cell> GridLst) {
	FString GridString;
	for (int32 i = 0; i < GridLst.Num(); i++) {
		GridString.Append(*FString::Printf(TEXT("%i, "), GridLst[i].Options.Num()));
	}
	UE_LOG(LogTemp, Warning, TEXT("Grid_list: \n%s"), *GridString);
}

void AGameBoard::ShowColl(TArray<Cell> GridLst, int32 Dimension) {
	FString GridString;
	for (int32 i = 0; i < GridLst.Num(); i++) {
		if (i % Dimension == 0) {
			GridString.Append("\n");
		}
		GridString.Append(*FString::Printf(TEXT("%i, "), GridLst[i].Options.Num()));
	}
	UE_LOG(LogTemp, Warning, TEXT("COLLAPSED: \n%s"), *GridString);
}