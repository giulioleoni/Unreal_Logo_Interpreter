// Fill out your copyright notice in the Description page of Project Settings.


#include "LogoFactory.h"
#include "LogoVMBlueprintFunctionLibrary.h"
#include "ImageUtils.h"





ULogoFactory::ULogoFactory()
{
	bEditorImport = true;                            // vuol dire che posso trascinare il file che sto importando
	SupportedClass = UTexture2D::StaticClass();      // vuol dire che quello che trascino creerà una texture2d
	Formats.Add(FString("logo;Logo file format"));   // vuol dire che mi aspetto un file con estensione .logo
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
	TArray<FColor> Result = ULogoVMBlueprintFunctionLibrary::LogoExecuteSimpleFromFile(Filename, 16, 16);
	FCreateTexture2DParameters Params;
	return FImageUtils::CreateTexture2D(16, 16, Result, InParent, InName.ToString(), Flags, Params);
	
}



























