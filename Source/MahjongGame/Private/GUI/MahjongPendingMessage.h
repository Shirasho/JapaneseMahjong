// Distributed by Shirasho Media 2016. All rights reserved. Available for distribution under the GNU GENERAL PUBLIC LICENSE v3.

#pragma once

class FMahjongPendingMessage
{
public:
    FText	DisplayString;				// This is the display message in the main message body
    FText	OKButtonString;				// This is the ok button text
    FText	CancelButtonString;			// If this is not empty, it will be the cancel button text
    FName	NextState;					// Final destination state once message is discarded

    TWeakObjectPtr< ULocalPlayer > PlayerOwner;		// Owner of dialog who will have focus (can be NULL)
};