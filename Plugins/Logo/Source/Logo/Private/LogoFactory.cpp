// Fill out your copyright notice in the Description page of Project Settings.


#include "LogoFactory.h"
#include "LogoVMBlueprintFunctionLibrary.h"
#include "ImageUtils.h"





ULogoFactory::ULogoFactory()
{
	bEditorImport = true;                            // I can drag to import the file
	SupportedClass = UTexture2D::StaticClass();      // the file I import will create a texture
	Formats.Add(FString("logo;Logo file format"));   // I expect a file with a .logo extension
}


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



























