// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

struct FMahjongGameLoadingScreenBrush : public FSlateDynamicImageBrush, public FGCObject
{
    FMahjongGameLoadingScreenBrush(const FName InTextureName, const FVector2D& InImageSize)
        : FSlateDynamicImageBrush(InTextureName, InImageSize)
    {
        ResourceObject = LoadObject<UObject>(NULL, *InTextureName.ToString());
    }

    virtual void AddReferencedObjects(FReferenceCollector& Collector)
    {
        if (ResourceObject)
        {
            Collector.AddReferencedObject(ResourceObject);
        }
    }
};