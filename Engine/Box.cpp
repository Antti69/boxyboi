#include "Box.h"
#include "ColorTrait.h"
IndexedTriangleList<Vec2> Box::model;




std::unique_ptr<Box> Box::Spawn( float size,const Boundaries& bounds,b2World& world,std::mt19937& rng )
{
	std::uniform_real_distribution<float> pos_dist(
		-bounds.GetSize() + size * 2.0f,
		bounds.GetSize() - size * 2.0f
	);
	std::uniform_real_distribution<float> power_dist( 0.0f,6.0f );
	std::uniform_real_distribution<float> angle_dist( -PI,PI );
	std::uniform_int_distribution<int> type_dist( 0,4 );

	const auto linVel = (Vec2{ 1.0f,0.0f } * Mat2::Rotation( angle_dist( rng ) )) * power_dist( rng );
	const auto pos = Vec2{ pos_dist( rng ),pos_dist( rng ) };
	const auto ang = angle_dist( rng );
	const auto angVel = angle_dist( rng ) * 1.5f;

	std::unique_ptr<ColorTrait> pColorTrait;
	switch( type_dist( rng ) )
	{
	case 0:
		pColorTrait = std::make_unique<RedTrait>();
		break;
	case 1:
		pColorTrait = std::make_unique<GreenTrait>();
		break;
	case 2:
		pColorTrait = std::make_unique<BlueTrait>();
		break;
	case 3:
		pColorTrait = std::make_unique<WhiteTrait>();
		break;
	case 4:
		pColorTrait = std::make_unique<YellowTrait>();
		break;
	}
	
	return std::make_unique<Box>( std::move( pColorTrait ),world,pos,size,ang,linVel,angVel );
}

std::vector<std::unique_ptr<Box>> Box::slice(b2World& world)
{
	std::vector<std::unique_ptr<Box>> boxes;

	const Vec2 pos = GetPosition();
	const Vec2 Vel = GetVelocity();
	const auto linVel = GetLinVel();
	const float angle = GetAngle();
	const float angleVel = GetAngularVelocity();
	const float size = GetSize() / 2.0f;

	for (int i = 0; i < 4; i++)
	{
		if (GetSize() > 0.09)
		{
			boxes.push_back(std::make_unique<Box>(pColorTrait->Clone(), world, pos, size, angle, linVel, angleVel));
			effect = Box::ColEffect::Dead;
		}
		
	}
	return boxes;
}
