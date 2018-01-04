// For adding header files define your includes here and add the headers in Game folder.
// For example, You may see these files - Attacker.hpp,Defender.hpp and Goalkeeper.hpp
// For checking out the skills you may see skills.h placed in Skills folder.
#pragma once
#include "skills.h"
#include "Attacker.hpp"
#include "Defender.hpp"
#include "GoalKeeper.hpp"

// Change your team color here (BLUE_TEAM/YELLOW_TEAM)face

Simulator::TeamColor teamColor = Simulator::BLUE_TEAM;

// Make usingDebugger is false when playing against an opponent
bool usingDebugger = true;

namespace MyStrategy
{
	
	static Vec2D ap;
	int def = 1, att = 2;
	const enum phase{ ATTACK_PHASE, DEFENCE_PHASE };

	int guess_finalYCoord(const Vector2D<int> pos, const Vector2D<int> vel, int _x);
	Vec2D guess_finalCoord(const Vector2D<int> pos, const Vec2D velperframe, int frameinc, bool &xreflection, bool &yreflection);
	// Write your strategy here in game function.
	
	phase ret_currphase(BeliefState *state){	
		
		if (state->ballPos.x > HALF_FIELD_MAXX / 2.0){
			static bool defenceon = false;
			if (state->homePos[1].x < state->ballPos.x && state->homePos[2].x < state->ballPos.x){
				return ATTACK_PHASE;
			}
			else if (state->homePos[1].x < state->ballPos.x || state->homePos[2].x < state->ballPos.x)
			{
				if (defenceon){
					if (state->ballVel.x > 0){
						defenceon = false;
						return ATTACK_PHASE;
					}
					else
						return DEFENCE_PHASE;
				}
				else return ATTACK_PHASE;
			}
			else if (state->homePos[1].x > state->ballPos.x && state->homePos[2].x > state->ballPos.x){
				defenceon = true;
				return DEFENCE_PHASE;
			}

		}
		else if (state->ballPos.x < -4000)
		{
			return DEFENCE_PHASE;
		}

		if (state->ballVel.x > 0)
			return ATTACK_PHASE;
		else
			return DEFENCE_PHASE;

		int c;

		if (state->ballPos.x < -HALF_FIELD_MAXX / 2.0)
			return DEFENCE_PHASE;
		/*else if (state->ballPos.x > HALF_FIELD_MAXX / 3.0)
			return ATTACK_PHASE;*/
		//both defenders are in front of the ball
		else if (state->homePos[1].x > state->ballPos.x && state->homePos[2].x > state->ballPos.x)
			return DEFENCE_PHASE;
		//both are behind the ball
		else if (state->homePos[1].x < state->ballPos.x && state->homePos[2].x < state->ballPos.x)
			return ATTACK_PHASE;
		//one is behind the ball
		else
		{
			return DEFENCE_PHASE;
			//if (state->homePos[1].x < state->ballPos.x)c = 1;
			//else if (state->homePos[2].x < state->ballPos.x)c = 2;
			//
			//int d_ball = Vec2D::distSq(state->homePos[c], state->ballPos);
			//const int bot_max_avg_vel = 85;
			//float frames_req_totouch = d_ball / ((float)bot_max_avg_vel * bot_max_avg_vel);

			//bool reflections;
			//Vec2D fut_ballpoint = guess_finalCoord(state->ballPos, getBallDirection(state), frames_req_totouch, reflections, reflections);
			//
			////the point where defender cathes the ball if it is already in defence phase then initiate the defence phase from this moment
			//if (fut_ballpoint.x < -HALF_FIELD_MAXX / 2.0){
			//	return DEFENCE_PHASE;
			//}
			//else{
			//	return ATTACK_PHASE;
			//}
		}


	}

	bool isballstuck(BeliefState *state)
	{
		static Vector2D<int> lastsetting(-1, -1);
		return false;
		//after each second)
		if(!(state->currFrameNum % 60))
		{
			if(abs(lastsetting.x - state->ballPos.x) < 3 && abs(lastsetting.y - state->ballPos.y) < 3)
				return true;
			lastsetting = state->ballPos;
		}

		return false;
	}

	Vec2D getBallDirection(BeliefState *state){
		return Vec2D(state->ballPos.x - ap.x, state->ballPos.y - ap.y);
	}

	// You can also make new functions and call them from game function.
  void game(BeliefState *state)
  {
	  static bool set = false;
	  if (!set){
		  set = true;
		  ap.x = ap.y = 0;
	  }

	static phase lastphase = DEFENCE_PHASE;
	if (ret_currphase(state) == DEFENCE_PHASE)
	{
		if(lastphase == DEFENCE_PHASE) defender(state, def, false);
		else{
			lastphase = DEFENCE_PHASE;
			defender(state, def, true);
		}
	}
	else
	{
		if(lastphase == ATTACK_PHASE) attacker(state, att, false);
		else{
			lastphase = ATTACK_PHASE;
			attacker(state, att, true);
		}
	}
	
	goalkeeper(state,0);
	

	//Spin(0, 20);
	
	//print_am("bot(%d %d) ball(%d %d)", state->homePos[1].x, state->homePos[1].y, state->ballPos.x, state->ballPos.y);
	  ap = state->ballPos;
  }
}