// Copyright Epic Games, Inc. All Rights Reserved.

#include "Logo.h"
#include "LogoVMBlueprintFunctionLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/OutputDeviceNull.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ImageUtils.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

#define LOCTEXT_NAMESPACE "FLogoModule"

void FLogoModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FLogoModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

bool FLogoModule::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FParse::Command(&Cmd, TEXT("Logo")))
	{
		FString FileName = FParse::Token(Cmd, false);
		if (!FileName.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("FileName= %s"), *FileName);

			TArray<FColor> Result = ULogoVMBlueprintFunctionLibrary::LogoExecuteSimpleFromFile(FileName, 16, 16);
			
			UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
			//EditorWorld->SpawnActor<AActor>();

			UBlueprint* CubeObject = LoadObject<UBlueprint>(nullptr, TEXT("/Game/BP_CubeStage.BP_CubeStage"));
			//UE_LOG(LogTemp, Error, TEXT("CubeObject = %p"), CubeObject);

			UClass* CubeClass = CubeObject->GeneratedClass;
			if (CubeClass->IsChildOf<AActor>())
			{
				for (int32 Index = 0 ; Index < Result.Num(); Index++)
				{
					FTransform Transform;
					Transform.SetLocation(FVector((Index % 16) * 100, (Index / 16) * 100, 0));
				    AActor* Cube = EditorWorld->SpawnActor<AActor>(CubeClass, Transform);

					FOutputDeviceNull OutputDeviceNull;
					
					//FString::Printf(TEXT("SetColor (R=%u,G=%u,B=%u,A=%u)"), Result[Index].R, Result[Index].G, Result[Index].B, Result[Index].A);
					const FString Arg = FString::Printf(TEXT("SetColor %s"), *(Result[Index].ToString()));
					Cube->CallFunctionByNameWithArguments(*Arg, OutputDeviceNull, nullptr, true);

					//UE_LOG(LogTemp, Error, TEXT("Color = %s"), *Color.ToString());
				}
			}

			UPackage* NewPackage = CreatePackage(TEXT("/Game/Art/Texture/Logo"));
			FString FileName2 = FPackageName::LongPackageNameToFilename(NewPackage->GetPathName(), FPackageName::GetAssetPackageExtension());
			FCreateTexture2DParameters Params;

			UTexture2D* MyTexture = FImageUtils::CreateTexture2D(16, 16, Result, NewPackage, "Logo", RF_Public | RF_Standalone, Params);
			UPackage::SavePackage(NewPackage, MyTexture, RF_Public | RF_Standalone, *FileName2);
			FAssetRegistryModule::AssetCreated(MyTexture);
			
			
			
		}
		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLogoModule, Logo)


 // /Script/Engine.Blueprint'/Game/BP_CubeStage.BP_CubeStage'