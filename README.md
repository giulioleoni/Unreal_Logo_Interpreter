# About

I did this project to better learn Unreal and C++. It's an Unreal Plugin that converts text written in [logo](https://www.transum.org/software/Logo/) language into 2D textures.

You just have to drag the .logo file into unreal to create the corresponding texture:

https://github.com/user-attachments/assets/fcc269e3-4a7e-4c30-9924-9e710c7c4cb1

For this project I used Unreal 5.3 and the plugin is written in C++.


## Import file

To import files, it is necessary to create a class that inherits from UFactory and then override the FactoryCreateFile method.
Additionally, there are some variables that need to be set correctly:

``` cpp   
ULogoFactory::ULogoFactory()
{
	bEditorImport = true;                            // I can drag to import the file
	SupportedClass = UTexture2D::StaticClass();      // the file I import will create a texture
	Formats.Add(FString("logo;Logo file format"));   // I expect a file with a .logo extension
}
```
Then I can define the FactoryCreateFile method that will be executed when I import a file:
``` cpp
UObject* ULogoFactory::FactoryCreateFile
(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	const FString& Filename,
	const TCHAR* Parms,
	FFeedbackContext* Warn,
	bool& bOutOperationCanceled)
{
	FIntVector2 TextureSize = FIntVector2(512 , 512);
	TArray<FColor> Result = ULogoVMBlueprintFunctionLibrary::LogoExecuteSimpleFromFile(Filename, TextureSize.X, TextureSize.Y);
	FCreateTexture2DParameters Params;
	return FImageUtils::CreateTexture2D(TextureSize.X, TextureSize.Y, Result, InParent, InName.ToString(), Flags, Params);
	
}
```

## Logo Interpreter

The file will be loaded and parsed to pick up only the commands, then the Logo Virtual Machine will be created to interpret them:

``` cpp   
TArray<FColor> ULogoVMBlueprintFunctionLibrary::LogoExecuteSimple(const TArray<FString>& Tokens, const int32 StageWidth, const int32 StageHeight)
{
	FLogoVM LogoVM;
	LogoVM.SetStageSize(FIntVector2(StageWidth, StageHeight));

	if (LogoVM.Execute(Tokens))
	{
		return LogoVM.GetStage();
	}

	return TArray<FColor>();
}
```

To interpret the text I thought FLogoVM should have a list of commands and an array of colors that will be the actual texture:

``` cpp
TArray<FColor> StageCells;

TMap<FString, TFunction<bool(FLogoVM&, TQueue<FString>&)>> Commands;
``` 

For the commands, I found it convenient to use a TMap so that each command (a text string) is associated with a function, which will return true if executed correctly.

Commands will be defined in the constructor of FLogoVM, such as below:

``` cpp
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
```

The virtual machine will simulate a pen writing on a paper (the color array) by following commands, which will either affect the color array or the pen, such as:

``` cpp
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
```

If all commands are executed successfully then I can return the array of colors.




