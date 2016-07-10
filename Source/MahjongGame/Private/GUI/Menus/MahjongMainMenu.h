// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

class FMahjongMainMenu : public TSharedFromThis<FMahjongMainMenu>, public FTickableGameObject
{
public:

    virtual ~FMahjongMainMenu();

    /** build menu */
    void Construct(TWeakObjectPtr<UMahjongGameInstance> _GameInstance, TWeakObjectPtr<ULocalPlayer> _PlayerOwner);

    /** Add the menu to the gameviewport so it becomes visible */
    void AddMenuToGameViewport();

    /** Remove from the gameviewport. */
    void RemoveMenuFromGameViewport();

    /** TickableObject Functions */
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override;
    virtual bool IsTickableWhenPaused() const override;

    /** Returns the player that owns the main menu. */
    ULocalPlayer* GetPlayerOwner() const;

    /** Returns the controller id of player that owns the main menu. */
    int32 GetPlayerOwnerControllerId() const;

    /** Returns the string name of the currently selected map */
    FString GetMapName() const;
};
