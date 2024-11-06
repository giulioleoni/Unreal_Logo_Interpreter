// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "LogoFactory.generated.h"

/**
 * 
 */
UCLASS()
class LOGO_API ULogoFactory : public UFactory
{
	GENERATED_BODY()

public:
	ULogoFactory();

	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	
};
