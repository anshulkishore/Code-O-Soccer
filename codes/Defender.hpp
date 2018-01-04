#pragma once
#include "skills.h"

namespace MyStrategy
{
	
	extern int def;
	extern const int x_goalie;
	static int blockade, saviour;

	bool pr_ballinmidfield(BeliefState *state);
	Vec2D getBallDirection(BeliefState *state);
	void gobelowBall(BeliefState *state, int botID, bool gotoTop);
	bool isInside(Point polygon[], int n, Point p);

	bool isInsideDBox(BeliefState *state, Vec2D point, bool oppDbox = true){
		Point polygon[] = { { 3000, 1600 }, { 4275, 1600 }, { 4275, -1600 }, { 3000, 1600 } };
		if (!oppDbox){
			int i;
			for (i = 0; i < 4; i++){
				polygon[i].x *= -1;
			}
		}
		Point p;
		p.x = point.x;
		p.y = point.y;
		return isInside(polygon, 4, p);
	}

	int normalise_coordinateY(int out, bool &yreflection)
	{
		if (out > HALF_FIELD_MAXY){
			yreflection = !yreflection;
			return normalise_coordinateY(HALF_FIELD_MAXY - (out - HALF_FIELD_MAXY), yreflection);
		}

		else if (out < -HALF_FIELD_MAXY){
			yreflection = !yreflection;
			return normalise_coordinateY(-HALF_FIELD_MAXY + abs(out) - HALF_FIELD_MAXY, yreflection);
		}
		else
			return out;
	}

	int normalise_coordinateX(int out, bool &xreflection)
	{
		if (out > HALF_FIELD_MAXX){
			xreflection = !xreflection;
			return normalise_coordinateX(HALF_FIELD_MAXX - (out - HALF_FIELD_MAXX), xreflection);
		}
			
		else if (out < -HALF_FIELD_MAXX){
			xreflection = !xreflection;
			return normalise_coordinateX(-HALF_FIELD_MAXX + abs(out) - HALF_FIELD_MAXX, xreflection);
		}
		else
			return out;
	}
	
	int guess_finalYCoord(const Vector2D<int> pos, const Vector2D<float> vel, int _x)
	{

		float tan_theta = abs(vel.y / vel.x);

		int y_ = ((abs(_x - pos.x )) * tan_theta);
		int dir = (vel.y > 0) ? 1 : -1;
		
		int ret = pos.y + y_ * dir;
		bool ref = false;
		return normalise_coordinateY(ret, ref);
	}

	Vec2D guess_finalCoord(const Vector2D<int> pos, const Vec2D velperframe, int frameinc, bool &xreflection, bool &yreflection){
		return Vec2D(normalise_coordinateX(pos.x + frameinc * velperframe.x, xreflection), normalise_coordinateY(pos.y + frameinc*velperframe.y, yreflection));
	}

	void DivertBalltoOpp(BeliefState *state, int botID){

		//assumming bot can move a maximum distance of 100units in straight direction when in full speed
		int max_botmotion_per_frame = 90;	//taking it as 90 after including the approximations
		
		//now iterate for a given number of maximum frames until the bot can collide with the ball
		//then hit the ball
		int FRAME_INTERPOLATION = 15;
		
		int dfromball = Vec2D::distSq(state->ballPos, state->homePos[botID]);
		if (dfromball < max_botmotion_per_frame * max_botmotion_per_frame)
			FRAME_INTERPOLATION = 2;
		else if (dfromball < 4 * max_botmotion_per_frame * max_botmotion_per_frame)
			FRAME_INTERPOLATION = 4;
		else if (dfromball < 9 * max_botmotion_per_frame * max_botmotion_per_frame)
			FRAME_INTERPOLATION = 6;
		else if (dfromball < 16 * max_botmotion_per_frame * max_botmotion_per_frame)
			FRAME_INTERPOLATION = 9;

		bool reflectionX = false, reflectionY = false;
		Vec2D dpoint = guess_finalCoord(state->ballPos, getBallDirection(state), FRAME_INTERPOLATION, reflectionX, reflectionY);

		//bring bot to the side of the ball 
		//lets do the calculations
		//we have reached near the ball then try to divert it
		
		if (dfromball <= 2 * BOT_BALL_THRESH * BOT_BALL_THRESH && state->ballPos.x > state->homePos[botID].x && state->ballVel.x < 0){
			if (state->ballPos.y > state->homePos[botID].y)
				Spin(botID, MAX_BOT_OMEGA);
			else
				Spin(botID, -MAX_BOT_OMEGA);
		}
		
		//defender is behind the ball
		else if (state->ballPos.x < state->homePos[botID].x ){

				
				int dball_bot = Vec2D::distSq(state->ballPos, state->homePos[botID]);
				if (dball_bot < 7 * (BOT_RADIUS - BALL_RADIUS) * (BOT_RADIUS - BALL_RADIUS)){
					//if ball is in negative y-dir take the bot below the ball (clockwise) and vice-versa decided by the last parameter;
					Vector2D<float> vel = Vector2D<float>(state->ballPos.x - state->homePos[botID].x, state->ballPos.y - state->homePos[botID].y);
					
					
					int y = guess_finalYCoord(state->ballPos, vel, -4270);
					//if (abs(y) < 760){
					//	Stop(botID);
					//}
					//else
					gobelowBall(state, botID, state->ballPos.y >= 0);
					
					print_am("Touch true stop\n");
				}
				else{
					GoToPoint(botID, state, dpoint, Vec2D::angle(dpoint, state->homePos[botID]), true, false);
					print_am("Defend\n");
				}
					

			}
			
		else
		{
			GoToPoint(botID, state, dpoint, Vec2D::angle(dpoint, state->homePos[botID]), true, false);
		}
	}

	void BlockBall(BeliefState *state, int botID, Vec2D dpoint){
		if (Vec2D::distSq(state->homePos[botID], state->ballPos) >= 2 * BOT_BALL_THRESH * BOT_BALL_THRESH)
			GoToPoint(botID, state, dpoint, PI, true, true);
		else {
			//slant the bot so as to deflect the ball
			if (state->homePos[botID].y < 0) TurnToAngle(botID, state, PI + 17 * PI / 180);
			else TurnToAngle(botID, state, PI - 17 * PI / 180);
		}
	}

	void ScissorBlock(BeliefState* state, int botID){
		int x_def = OUR_GOAL_X + DBOX_WIDTH + 200;
		int y_lim = 300 + 2 * DBOX_HEIGHT;
		int err_threshold = 120;

		if (state->ballPos.x < -(x_def + err_threshold)){
			BlockBall(state, botID, Vec2D(state->ballPos.x + BOT_RADIUS + BALL_RADIUS, y_lim));
		}
		else{
			Vec2D dir = getBallDirection(state);
			int g_bally = guess_finalYCoord(state->ballPos, Vector2D<float>((float)dir.x, (float)dir.y), x_def);

			if (abs(g_bally) > y_lim){
				BlockBall(state, botID, Vec2D(state->ballPos.x - BOT_RADIUS - BALL_RADIUS, y_lim));
			}
			else if (abs(g_bally) <= DBOX_HEIGHT)
				GoToBall(botID, state, false);
			else{
				bool reflection;
				Vec2D fpoint = guess_finalCoord(state->ballPos, getBallDirection(state), 1, reflection, reflection);
				float angle = Vec2D::angle(fpoint, state->ballPos);
				if ((state->homeAngle[botID] - angle) > (PI - (state->homeAngle[botID] - angle)))
					angle = PI - (state->homeAngle[botID] - angle);
				
				int dist_ballbot = Vec2D::distSq(state->ballPos, state->homePos[botID]);
				if (dist_ballbot < 4 * BOT_BALL_THRESH * BOT_BALL_THRESH){
					GoToBall(botID, state, false);
				}
				else
				{
					float addslope = 5 * PI / 180;
					if (state->ballPos.y > state->homePos[botID].y){
						//clockwise rotation
						GoToPoint(botID, state, Vec2D(g_bally, x_def), angle + addslope, false, false);
					}
					else{
						GoToPoint(botID, state, Vec2D(g_bally, x_def), angle - addslope, false, false);
					}
				}
			}
		}
		
	}

  void defender(BeliefState *state, int botID, bool defence_commenced)
  {
	  static Vec2D ap = state->ballPos;
	  static Vec2D blockpoint;

	if (defence_commenced){

		if (Vec2D::distSq(state->homePos[1], state->ballPos) < Vec2D::distSq(state->homePos[2], state->ballPos))
		{
			blockade = 2;
			saviour = 1;	
		}
		else 
		{
			blockade = 1;
			saviour = 2;
		}

		Vector2D<float> dirr = Vector2D<float>(getBallDirection(state).x, getBallDirection(state).y);
		int y = guess_finalYCoord(state->ballPos, dirr, -4270);
		if (y > 0)
			blockpoint = Vec2D(-4275, -2222);
		else
			blockpoint = Vec2D(-4275, 2222);
	}
	
	//ScissorBlock(state, blockade);
	
	if(!isInsideDBox(state, state->ballPos, false)) DivertBalltoOpp(state, blockade);
	else
	{
		BlockBall(state, blockade, Vec2D(-4270, 2222));
	}
	
	int d_keep_opp = Vec2D::distSq(state->homePos[0], state->awayPos[state->oppBotNearestToBall]);
	if (d_keep_opp < 16 * BOT_RADIUS * BOT_RADIUS && state->ballPos.x < -3600){
		int d_keep_hom = Vec2D::distSq(state->homePos[0], state->homePos[saviour]);
		if (d_keep_hom < 4 * BOT_RADIUS * BOT_RADIUS){
			GoToBall(saviour, state, false);
		}
		else{
			Vec2D depoint = state->homePos[0];
			int dir = (state->ballPos.y > state->homePos[0].y) ? -1 : 1;
			depoint.y += BOT_RADIUS * 1.5 * dir;
			float angle;
			if (abs(state->homeAngle[0] - PI / 2) < abs(state->homeAngle[0] + PI / 2))
				angle = PI / 2;
			else
				angle = -PI / 2;
			GoToPoint(saviour, state, depoint, angle, true, false);
		}
	}
	else
	{
		DivertBalltoOpp(state, saviour);
	}
	
	  ap = state->ballPos;
  }
}
