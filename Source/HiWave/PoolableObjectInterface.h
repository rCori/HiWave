// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPoolableObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HIWAVE_API IPoolableObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PoolableObjectInterface")
	void SetObjectLifeSpan(float InLifespan);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PoolableObjectInterface")
	void SetActive(bool IsActive);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PoolableObjectInterface")
	bool IsActive();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PoolableObjectInterface")
	void Deactivate();

};
