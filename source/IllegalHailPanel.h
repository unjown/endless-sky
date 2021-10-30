/* IllegalHailPanel.h
Copyright (c) 2021 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef ILLEGAL_HAIL_PANEL_H_
#define ILLEGAL_HAIL_PANEL_H_

#include "Panel.h"

#include "Angle.h"
#include "Point.h"
#include "Politics.h"

#include <cstdint>
#include <memory>
#include <string>

class Government;
class Planet;
class PlayerInfo;
class Ship;
class Sprite;
class StellarObject;



// This panel is shown when a ship scans you and finds illegal outfits.
class IllegalHailPanel : public Panel {
public:
	IllegalHailPanel(PlayerInfo &player, const Ship &hailingShip, const Ship &scannedShip, const Politics::Punishment &fine);

	virtual void Draw() override;


protected:
	// Only override the ones you need; the default action is to return false.
	virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;


private:
	//void SetBribe(double scale);


private:
	PlayerInfo &player;
	const Ship &hailingShip;
	const Ship &scannedShip;
	Angle facing;

	std::string header;
	std::string message;

	Politics::Punishment fine;
	int64_t bribe;
};



#endif
