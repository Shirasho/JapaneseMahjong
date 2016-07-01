// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#include "MahjongGame.h"
#include "MathBlueprintLibrary.h"


float UMathBlueprintLibrary::SetPrecision_Float(float Value, uint32 DecimalPlaces)
{
	float PowerValue = FMath::Pow(10, DecimalPlaces);
	return (float)(FMath::Floor(Value * PowerValue)) / PowerValue;
}

double UMathBlueprintLibrary::SetPrecision_Double(double Value, uint32 DecimalPlaces)
{
	double PowerValue = FMath::Pow(10, DecimalPlaces);
	return (double)(FMath::Floor(Value * PowerValue)) / PowerValue;
}