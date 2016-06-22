// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "JapaneseMahjong.h"	// Required include
#include "UnrealNetwork.h"		// Replication

#include "MahjongTiles.h"
#include "MahjongTable.h"


#include "MahjongPlayerHand.h"
#include "JapaneseMahjongHandResolver.h"


AMahjongTable::AMahjongTable()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;

	// Initialize our arrays.
	Tiles.Reserve(136);
}

// Called when the game starts or when spawned
void AMahjongTable::BeginPlay()
{
	// Create our tiles.
	PopulateTiles();
	// Reset the tiles to their default values.
	ResetTiles();

	Super::BeginPlay();
}

// Called every frame
void AMahjongTable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMahjongTable::PopulateTiles() {
	// Each tile has 4 copies
	for (uint8 Copy = 0; Copy < 4; ++Copy) {
		// Suite has 9 values.
		for (uint8 SuiteTileIndex = 1; SuiteTileIndex <= 9; ++SuiteTileIndex) {
			AMahjongSouTile* SouTile = GetWorld()->SpawnActor<AMahjongSouTile>(AMahjongSouTile::StaticClass());
			AMahjongPinTile* PinTile = GetWorld()->SpawnActor<AMahjongPinTile>(AMahjongPinTile::StaticClass());
			AMahjongWanTile* WanTile = GetWorld()->SpawnActor<AMahjongWanTile>(AMahjongWanTile::StaticClass());

			// Init the tiles - there is 1 red tile (first copy), and the standard red tile is of value 5.
			SouTile->Init(SuiteTileIndex, (Copy == 0 && SuiteTileIndex == 5));
			PinTile->Init(SuiteTileIndex, (Copy == 0 && SuiteTileIndex == 5));
			WanTile->Init(SuiteTileIndex, (Copy == 0 && SuiteTileIndex == 5));

			// Add the tiles to the Tiles array.
			Tiles.Add(SouTile);
			Tiles.Add(PinTile);
			Tiles.Add(WanTile);
		}
		// Wind has 4 values.
		for (uint8 WindTileIndex = 1; WindTileIndex <= 4; ++WindTileIndex) {
			AMahjongWindTile* WindTile = GetWorld()->SpawnActor<AMahjongWindTile>(AMahjongWindTile::StaticClass());

			// Init the tiles - wind tiles do not have a red tile.
			WindTile->Init(WindTileIndex, false);

			// Add the tile to the Tiles array.
			Tiles.Add(WindTile);
		}
		// Dragon has 3 values.
		for (uint8 DragonTileIndex = 1; DragonTileIndex <= 3; ++DragonTileIndex) {
			AMahjongDragonTile* DragonTile = GetWorld()->SpawnActor<AMahjongDragonTile>(AMahjongDragonTile::StaticClass());

			// Init the tiles - dragon tiles do not have a red tile.
			DragonTile->Init(DragonTileIndex, false);

			// Add the tile to the Tiles array.
			Tiles.Add(DragonTile);
		}
	}
}

void AMahjongTable::ResetTiles() {
	for (AMahjongTile* Tile : Tiles) {
		Tile->ResetTile();
	}
	ShuffleTiles();
}

void AMahjongTable::ShuffleTiles() {
	int32 LastIndex = Tiles.Num() - 1;
	for (int32 i = 0; i < LastIndex; ++i) {
		int32 RandomIndex = FMath::RandRange(0, LastIndex);
		if (i != RandomIndex) {
			Tiles.Swap(i, RandomIndex);
		}
	}
}


void AMahjongTable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	DOREPLIFETIME(AMahjongTable, Tiles);
}






void AMahjongTable::DebugPrint() {

	UMahjongPlayerHand* PlayerHand = NewObject<UMahjongPlayerHand>();

	for (int32 i = 0; i < 13; ++i) {
		PlayerHand->AddTile(Tiles[i]);
	}
	//for (uint8 Copy = 0; Copy < 3; ++Copy) {
	//	// Suite has 9 values.
	//	for (uint8 SuiteTileIndex = 1; SuiteTileIndex <= 3; ++SuiteTileIndex) {
	//		AMahjongSouTile* SouTile = GetWorld()->SpawnActor<AMahjongSouTile>(AMahjongSouTile::StaticClass());

	//		// Init the tiles - there is 1 red tile (first copy), and the standard red tile is of value 5.
	//		SouTile->Init(SuiteTileIndex * 2, (Copy == 0 && SuiteTileIndex == 5));

	//		// Add the tiles to the Tiles array.
	//		PlayerHand->AddTile(SouTile);
	//	}
	//}
	//AMahjongSouTile* SouTile = GetWorld()->SpawnActor<AMahjongSouTile>(AMahjongSouTile::StaticClass());
	//SouTile->Init(3, false);
	//PlayerHand->AddTile(SouTile);
	PlayerHand->Sort();

	//FJapaneseMahjongHandResolver* HandResolver = Cast<FJapaneseMahjongHandResolver>(PlayerHand->GetHandResolver());
	FJapaneseMahjongHandResolver* HandResolver = static_cast<FJapaneseMahjongHandResolver*>(PlayerHand->GetHandResolver());

	if (GEngine && HandResolver) {
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20, FColor::Cyan, *PlayerHand->ToString(), false);
		UE_LOG(MahjongLog, Log, TEXT("%s"), *PlayerHand->ToString());

		FString SequenceOutput = TEXT("Number of 3-sequences: ");
		SequenceOutput += FString::FromInt(HandResolver->GetSequenceCount(3, PlayerHand));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20, FColor::Cyan, *SequenceOutput, false);

		SequenceOutput = TEXT("Number of 2-sequences: ");
		SequenceOutput += FString::FromInt(HandResolver->GetPartialSequenceCount(PlayerHand));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20, FColor::Cyan, *SequenceOutput, false);

		HandResolver->BeginResolve(PlayerHand);
		SequenceOutput = TEXT("Number of 3-sequences: ");
		SequenceOutput += FString::FromInt(HandResolver->GetSequenceCount(3, PlayerHand));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20, FColor::Cyan, *SequenceOutput, false);

		SequenceOutput = TEXT("Number of 2-sequences: ");
		SequenceOutput += FString::FromInt(HandResolver->GetPartialSequenceCount(PlayerHand));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20, FColor::Cyan, *SequenceOutput, false);
		HandResolver->EndResolve();

		FString PairOutput = TEXT("Number of 2-pairs: ");
		PairOutput += FString::FromInt(HandResolver->GetPairCount(2, PlayerHand));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20, FColor::Cyan, *PairOutput, false);

		PairOutput = TEXT("Number of 3-pairs: ");
		PairOutput += FString::FromInt(HandResolver->GetPairCount(3, PlayerHand));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20, FColor::Cyan, *PairOutput, false);

		FString ShantenOutput = TEXT("Shanten Value: ");
		ShantenOutput += FString::FromInt(HandResolver->GetShantenValue(PlayerHand));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20, FColor::Cyan, *ShantenOutput, false);
	}
}