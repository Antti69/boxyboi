/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Game.cpp																			  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#include "MainWindow.h"
#include "Game.h"
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include <functional>
#include <iterator>



Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	world( { 0.0f,-2.5f } ),
	pepe( gfx )
{
	pepe.effect.vs.cam.SetPos( { 0.0,0.0f } );
	pepe.effect.vs.cam.SetZoom( 1.0f / boundarySize );

	std::generate_n( std::back_inserter( boxPtrs ),nBoxes,[this]() {
		return Box::Spawn( boxSize,bounds,world,rng );
	} );

	class Listener : public b2ContactListener
	{
	public:
		void BeginContact( b2Contact* contact ) override
		{
			b2Body* bodyPtrs[] = {contact->GetFixtureA()->GetBody(),contact->GetFixtureB()->GetBody()};
			if( bodyPtrs[0]->GetType() == b2BodyType::b2_dynamicBody &&
				bodyPtrs[1]->GetType() == b2BodyType::b2_dynamicBody )
			{
				Box* pboxPtrs[] = 
				{ 
					reinterpret_cast<Box*>(bodyPtrs[0]->GetUserData().pointer),
					reinterpret_cast<Box*>(bodyPtrs[1]->GetUserData().pointer)
				};

				ColResult Cr;

				Cr.Case(TypePair(typeid(RedTrait), typeid(BlueTrait))) = [&](Box& a, Box& b) //Destroying stuff
				{
					b.effect = Box::ColEffect::Dead;
				};
				Cr.Case(TypePair(typeid(BlueTrait), typeid(RedTrait))) = [&](Box& a, Box& b)
				{
					a.effect = Box::ColEffect::Dead;
				};


				/*Cr.Case(TypePair(typeid(RedTrait), typeid(YellowTrait))) = [&](Box& a, Box& b)	//Color changing stuff
				{
					b.CloneColorTrait(a);
				};
				Cr.Case(TypePair(typeid(YellowTrait), typeid(RedTrait))) = [&](Box& a, Box& b)
				{
					a.CloneColorTrait(b);
				}; */

				Cr.Case(TypePair(typeid(WhiteTrait), typeid(GreenTrait))) = [&](Box& a, Box& b)	//Slicing stuff
				{
					a.effect = Box::ColEffect::Slice;
					b.effect = Box::ColEffect::Slice;
				};
				Cr.Case(TypePair(typeid(GreenTrait), typeid(WhiteTrait))) = [&](Box& a, Box& b)
				{
					a.effect = Box::ColEffect::Slice;
					b.effect = Box::ColEffect::Slice;
				};

				Cr.Case(TypePair(typeid(BlueTrait), typeid(YellowTrait))) = [&](Box& a, Box& b)	//Density changes
				{
					a.effect = Box::ColEffect::MassChange;
					b.effect = Box::ColEffect::MassChange;

				};
				Cr.Case(TypePair(typeid(YellowTrait), typeid(BlueTrait))) = [&](Box& a, Box& b)
				{
					a.effect = Box::ColEffect::MassChange;
					b.effect = Box::ColEffect::MassChange;
				};



				Cr.Switch(*pboxPtrs[0], *pboxPtrs[1]);
				
			}
		}

	};
	static Listener mrLister;
	world.SetContactListener( &mrLister );
	
}

void Game::Go()
{
	gfx.BeginFrame();
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	const float dt = ft.Mark();

	world.Step( dt,8,3 );

	for (unsigned int i = 0; i < boxPtrs.size(); i++)
	{
		if (boxPtrs[i]->effect == Box::ColEffect::Slice)
		{
			auto b = boxPtrs[i].get()->slice(world);
			auto tb = std::make_move_iterator(b.begin());
			auto te = std::make_move_iterator(b.end());
			boxPtrs.insert(boxPtrs.end(), tb, te);
		}
		/*if (boxPtrs[i]->effect == Box::ColEffect::DensityChange)
		{
			boxPtrs[i]->ChangeDensity(10.0f);
			
		}

		if (boxPtrs[i]->effect == Box::ColEffect::MassChange)
		{
			boxPtrs[i]->ChangeMass(3.0f);
		}*/

	}


	boxPtrs.erase(std::remove_if(boxPtrs.begin(), boxPtrs.end(),			//Kills the boxes that are marked for death!
		[](std::unique_ptr<Box>& b)
	{
		return b->effect == Box::ColEffect::Dead;
	}), boxPtrs.end());

}

void Game::ComposeFrame()
{
	for( const auto& p : boxPtrs )
	{
		p->Draw( pepe );
	}
}