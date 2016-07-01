// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

#include "MathBlueprintLibrary.generated.h"

UCLASS()
class UMathBlueprintLibrary : public UBlueprintFunctionLibrary {

	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "Math|Float", DisplayName = "Set Precision (float)")
	static float SetPrecision_Float(float Value, uint32 DecimalPlaces);

	UFUNCTION(BlueprintPure, Category = "Math|Float", DisplayName = "Set Precision (double)")
	static double SetPrecision_Double(double Value, uint32 DecimalPlaces);
};