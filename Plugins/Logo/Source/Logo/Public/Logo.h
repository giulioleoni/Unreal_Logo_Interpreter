// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FLogoVM
{
public:
	FLogoVM() :
		Position(0, 0), Direction(0, 1), bIsPenUp(false), StageSize(20, 20), Rotation(0)
	{
		CurrentColor = FColor::Black;
		SetStageSize(StageSize);

		Colors.Add(FColor::Black);
		//Colors.Add(FColor::White);
		Colors.Add(FColor::Red);
		Colors.Add(FColor::Blue);
		Colors.Add(FColor::Green);
		Colors.Add(FColor::Yellow);
		Colors.Add(FColor::Cyan);
		Colors.Add(FColor::Magenta);
		Colors.Add(FColor::Purple);
		Colors.Add(FColor::Orange);

		Commands.Add("fd", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {
			if (TokensQueue.IsEmpty())
			{
				return false;
			}

			FString MyArg;
			TokensQueue.Dequeue(MyArg);

			if (!MyArg.IsNumeric())
			{
				return false;
			}

			int32 MoveDistance = FCString::Atoi(*MyArg);

			LogoVM.Move(MoveDistance);

			return true;
			});

		Commands.Add("bk", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {
			if (TokensQueue.IsEmpty())
			{
				return false;
			}

			FString MyArg;
			TokensQueue.Dequeue(MyArg);

			if (!MyArg.IsNumeric())
			{
				return false;
			}

			int32 MoveDistance = FCString::Atoi(*MyArg);

			LogoVM.Move(-MoveDistance);

			return true;
			});

		Commands.Add("rt", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {
			if (TokensQueue.IsEmpty())
			{
				return false;
			}

			FString MyArg;
			TokensQueue.Dequeue(MyArg);

			if (!MyArg.IsNumeric())
			{
				return false;
			}

			float RotationAmount = FCString::Atof(*MyArg);
			int RotationAmountInt = FCString::Atoi(*MyArg);

			LogoVM.RotateInt(RotationAmount);

			return true;
			});

		Commands.Add("pd", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {
			LogoVM.SetPenUp(false);
			return true;
			});

		Commands.Add("pu", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {
			LogoVM.SetPenUp(true);
			return true;
			});

		Commands.Add("cs", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {
			LogoVM.SetStageSize(LogoVM.GetStageSize());
			return true;
			});

		Commands.Add("ct", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {
			LogoVM.SetPosition(FVector2D::Zero());
			return true;
			});

		Commands.Add("pc", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {

			if (TokensQueue.IsEmpty())
			{
				return false;
			}

			FString MyArg;
			TokensQueue.Dequeue(MyArg);

			if (!MyArg.IsNumeric())
			{
				return false;
			}

			int32 Color = FCString::Atoi(*MyArg);
			LogoVM.SetCurrentColor(Color);

			return true;
			});
	};

	~FLogoVM()
	{

	};

	FLogoVM(FLogoVM&& Other) = delete;
	FLogoVM(const FLogoVM& Other) = delete;
	FLogoVM& operator=(FLogoVM& Other) = delete;

	void SetStageSize(const FIntVector2 InStageSize)
	{
		StageCells.Empty();
		StageSize = InStageSize;
		Rotation = 0;
		RotateInt(0);
		CenterOffset = FIntVector2(InStageSize.X / 2, InStageSize.Y / 2);
		StageCells.AddUninitialized(InStageSize.X * InStageSize.Y);

		for (int32 Index = 0; Index < StageCells.Num(); Index++)
		{
			StageCells[Index] = FColor::White;
		}
	}

	bool Execute(const TArray<FString>& Tokens)
	{
		TQueue<FString> TokensQueue;

		for (const FString& Token : Tokens)
		{
			TokensQueue.Enqueue(Token);
		}

		while (!TokensQueue.IsEmpty())
		{
			FString PoppedToken;
			TokensQueue.Dequeue(PoppedToken);

			if (Commands.Contains(PoppedToken))
			{
				if (!Commands[PoppedToken](*this, TokensQueue))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	void Move(int32 MoveVector)
	{
		UE_LOG(LogTemp, Warning, TEXT("Move: %d"), MoveVector);
		if (!bIsPenUp)
		{
			for (int32 Step = 0; Step < FMath::Abs(MoveVector); Step++)
			{
				Position += Direction * FMath::Sign(MoveVector);
				FIntVector2 RealPosition = FIntVector2((CenterOffset.X + FMath::RoundToInt32(Position.X)), (CenterOffset.Y + FMath::RoundToInt32(Position.Y)));
				const int32 Index = RealPosition.X + RealPosition.Y * StageSize.X;
				if (StageCells.IsValidIndex(Index))
				{
					StageCells[Index] = CurrentColor;
				}
			}
		}
		else
		{
			Position += Direction * MoveVector;
		}
	}

	void RotateInt(const int Angle)
	{
		Rotation += Angle % 360;
		float RadiantRotation = FMath::DegreesToRadians(Rotation);

		Direction.X = FMath::Cos(RadiantRotation);
		Direction.Y = FMath::Sin(RadiantRotation);

	}

	void SetPenUp(bool bInIsPenUp)
	{
		bIsPenUp = bInIsPenUp;
	}

	void SetCurrentColor(const int32 ColorIndex)
	{
		if (Colors.IsValidIndex(ColorIndex))
		{
			CurrentColor = Colors[ColorIndex];
		}
		else
		{
			CurrentColor = Colors[0];
		}

	}

	void SetPosition(const FVector2D InPosition)
	{
		Position = InPosition;
	}

	const TArray<FColor> GetStage()
	{
		return StageCells;
	}

	const FIntVector2 GetStageSize()
	{
		return StageSize;
	}


protected:
	FVector2D Position;
	FVector2D Direction;
	bool bIsPenUp;
	FIntVector2 StageSize;
	int32 Rotation;
	FColor CurrentColor;
	TArray<FColor> StageCells;
	FIntVector2 CenterOffset;

	TMap<FString, TFunction<bool(FLogoVM&, TQueue<FString>&)>> Commands;

	TArray<FColor> Colors;
};

class FLogoModule : public IModuleInterface, public FSelfRegisteringExec
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
};
