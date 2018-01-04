#pragma once
#include "skills.h"

namespace MyStrategy
{


	void attacker2(BeliefState *state, int botID)
	{
		int d = Vector2D<int>::distSq(state->homePos[4], state->ballPos);
		int x_g = state->ballPos.x, y_g = state->ballPos.y, x1 = state->homePos[4].x, y1 = state->homePos[4].y;
		if (x_g > HALF_FIELD_MAXX - 1600 && (y_g > 1600 || y_g < -1600))
			GoToPoint(4, state, Vec2D(HALF_FIELD_MAXX - 2000, 0), PI / 2, true, false);
		else if (x_g > HALF_FIELD_MAXX - 1600)
			GoToPoint(4, state, Vec2D(x_g, y_g), PI / 2, true, false);
		else if(x_g - x1 < 0 && x1 > 0)
			GoToPoint(4, state, Vec2D(-100, y_g), PI / 2, true, false);
		else if (x1 > 0 && d < 4 * BOT_BALL_THRESH * BOT_BALL_THRESH){
			if (state->ballPos.y > state->homePos[4].y)
				Spin(0, MAX_BOT_OMEGA);
			else
				Spin(0, -MAX_BOT_OMEGA);
		}
		else if (x_g - x1 > 0 && x1 > 0)
			GoToPoint(4, state, Vec2D(x_g, y_g), PI / 2, true, false);
		else if (x1 > -HALF_FIELD_MAXX + 2100 && x_g - x1 > 0 && state->ballVel.x > 0)
			GoToPoint(4, state, Vec2D(x_g, y_g), PI / 2, true, false);
		else if (x1 > -HALF_FIELD_MAXX + 2600)
			GoToPoint(4, state, Vec2D(x_g - 500, y_g), PI / 2, true, false);
		else if (y_g > 0)
			GoToPoint(4, state, Vec2D(-1200, 2500), PI / 2, true, false);
		else 
			GoToPoint(4, state, Vec2D(-1200, -2500), PI / 2, true, false);

	}
}