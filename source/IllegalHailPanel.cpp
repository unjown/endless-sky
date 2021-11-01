/* IllegalHailPanel.cpp
Copyright (c) 2021 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "IllegalHailPanel.h"

#include "text/alignment.hpp"
#include "DrawList.h"
#include "text/Font.h"
#include "text/FontSet.h"
#include "text/Format.h"
#include "GameData.h"
#include "Government.h"
#include "Information.h"
#include "Interface.h"
#include "Messages.h"
#include "Planet.h"
#include "PlayerInfo.h"
#include "Politics.h"
#include "Ship.h"
#include "ShipEvent.h"
#include "Sprite.h"
#include "System.h"
#include "UI.h"
#include "text/WrappedText.h"

#include <algorithm>
#include <cmath>

using namespace std;



IllegalHailPanel::IllegalHailPanel(PlayerInfo &player, const Ship &hailingShip, const Ship &scannedShip, const Politics::Punishment &fine)
	: player(player), hailingShip(hailingShip), scannedShip(scannedShip), facing(hailingShip.Facing()), fine(fine)
{
	const Government *gov = hailingShip.GetGovernment();
	if(!hailingShip.Name().empty())
		header = gov->GetName() + " " + hailingShip.Noun() + " \"" + hailingShip.Name() + "\"";
	else
		header = hailingShip.ModelName() + " (" + gov->GetName() + ")";
	header += " is hailing you:";

	static const std::string defaultMessage
		= "You've been detected carrying illegal <type> and have been issued a fine of <fine> credits. \n\tDump your cargo immediately or we'll be forced to disable and board your ship.";

	map<string, string> subs = {
		{"<type>", fine.reason & Politics::Punishment::Outfit ? "outfits" : "cargo"},
		{"<fine>", Format::Number(fine.cost)},
	};
	message =
		Format::Replace(gov->GetInterdiction().empty() ? defaultMessage : gov->GetInterdiction(), subs);

	if(gov->GetBribeFraction())
	{
		static const std::string defaultBribe
			= "If you want us to leave you alone, it'll cost you <bribe> credits.";

		bribe = 1000 * static_cast<int64_t>(sqrt(fine.cost) * gov->GetBribeFraction());
		subs["<bribe>"] = Format::Credits(bribe);

		auto bribeMessage =
			Format::Replace(gov->GetInterdictionBribe().empty() ? defaultBribe : gov->GetInterdictionBribe(),
					subs);
		message += "\n\t";
		message += bribeMessage;
	}
}



void IllegalHailPanel::Draw()
{
	DrawBackdrop();

	Information info;
	info.SetString("header", header);
	if(bribe)
		info.SetCondition("can bribe");

	const Interface *hailUi = GameData::Interfaces().Get("illegal hail panel");
	hailUi->Draw(info, this);

	// Draw the sprite, rotated, scaled, and swizzled as necessary.
	float zoom = min(2., 400. / max(hailingShip.Width(), hailingShip.Height()));
	Point center(-170., -10.);

	DrawList draw;
	// Copy its swizzle, animation settings, etc.
	draw.Add(Body(hailingShip, center, Point(), facing, zoom));

	// Draw its turret sprites.
	for(const Hardpoint &hardpoint : hailingShip.Weapons())
		if(hardpoint.GetOutfit() && hardpoint.GetOutfit()->HardpointSprite().HasSprite())
		{
			Body body(
				hardpoint.GetOutfit()->HardpointSprite(),
				center + zoom * facing.Rotate(hardpoint.GetPoint()),
				Point(),
				facing + hardpoint.GetAngle(),
				zoom);
			draw.Add(body);
		}
	draw.Draw();

	// Draw the current message.
	WrappedText wrap;
	wrap.SetAlignment(Alignment::JUSTIFIED);
	wrap.SetWrapWidth(330);
	wrap.SetFont(FontSet::Get(14));
	wrap.Wrap(message);
	wrap.Draw(Point(-50., -50.), *GameData::Colors().Get("medium"));
}



bool IllegalHailPanel::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress)
{
	if(key == 's' || key == 'c')
	{
		// TODO: Dump illegal cago.
		(void)scannedShip;

		// Pay the required fine.
		player.Accounts().AddFine(fine.cost);
		GetUI()->Pop(this);
	}
	else if(key == 'f')
	{
		hailingShip.GetGovernment()->Offend(ShipEvent::PROVOKE);
		GetUI()->Pop(this);
	}
	else if((key == 'o' || key == 'b') && bribe)
	{
		if(bribe > player.Accounts().Credits())
		{
			message = "Sorry, but you don't have enough money to be worth my while.";
			return true;
		}

		player.Accounts().AddCredits(-bribe);
		Messages::Add("You bribed a " + hailingShip.GetGovernment()->GetName() + " ship "
			+ Format::Credits(bribe) + " credits to avoid paying a fine today."
				, Messages::Importance::High);

		GetUI()->Pop(this);
	}

	return true;
}
