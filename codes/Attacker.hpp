#pragma once
#include "skills.h"

namespace MyStrategy
{

	extern int att;
	Vec2D guess_finalCoord(const Vector2D<int> pos, const Vec2D velperframe, int frameinc, bool &xreflection, bool &yreflection);
	Vec2D getBallDirection(BeliefState *state);
	int guess_finalYCoord(const Vector2D<int> pos, const Vector2D<float> vel, int _x);


	// Define Infinite (Using INT_MAX caused overflow problems
	#define INF 10000
	struct Point
	{
		int x;
		int y;
	};

	// Given three colinear points p, q, r, the function checks if
	// point q lies on line segment 'pr'
	bool onSegment(Point p, Point q, Point r)
	{
		if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
			q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
			return true;
		return false;
	}

	// To find orientation of ordered triplet (p, q, r).
	// The function returns following values
	// 0 --> p, q and r are colinear
	// 1 --> Clockwise
	// 2 --> Counterclockwise
	int orientation(Point p, Point q, Point r)
	{
		int val = (q.y - p.y) * (r.x - q.x) -
			(q.x - p.x) * (r.y - q.y);

		if (val == 0) return 0;  // colinear
		return (val > 0) ? 1 : 2; // clock or counterclock wise
	}

	// The function that returns true if line segment 'p1q1'
	// and 'p2q2' intersect.
	bool doIntersect(Point p1, Point q1, Point p2, Point q2)
	{
		// Find the four orientations needed for general and
		// special cases
		int o1 = orientation(p1, q1, p2);
		int o2 = orientation(p1, q1, q2);
		int o3 = orientation(p2, q2, p1);
		int o4 = orientation(p2, q2, q1);

		// General case
		if (o1 != o2 && o3 != o4)
			return true;

		// Special Cases
		// p1, q1 and p2 are colinear and p2 lies on segment p1q1
		if (o1 == 0 && onSegment(p1, p2, q1)) return true;

		// p1, q1 and p2 are colinear and q2 lies on segment p1q1
		if (o2 == 0 && onSegment(p1, q2, q1)) return true;

		// p2, q2 and p1 are colinear and p1 lies on segment p2q2
		if (o3 == 0 && onSegment(p2, p1, q2)) return true;

		// p2, q2 and q1 are colinear and q1 lies on segment p2q2
		if (o4 == 0 && onSegment(p2, q1, q2)) return true;

		return false; // Doesn't fall in any of the above cases
	}

	// Returns true if the point p lies inside the polygon[] with n vertices
	bool isInside(Point polygon[], int n, Point p)
	{
		// There must be at least 3 vertices in polygon[]
		if (n < 3)  return false;

		// Create a point for line segment from p to infinite
		Point extreme = { INF, p.y };

		// Count intersections of the above line with sides of polygon
		int count = 0, i = 0;
		do
		{
			int next = (i + 1) % n;

			// Check if the line segment from 'p' to 'extreme' intersects
			// with the line segment from 'polygon[i]' to 'polygon[next]'
			if (doIntersect(polygon[i], polygon[next], p, extreme))
			{
				// If the point 'p' is colinear with line segment 'i-next',
				// then check if it lies on segment. If it lies, return true,
				// otherwise false
				if (orientation(polygon[i], p, polygon[next]) == 0)
					return onSegment(polygon[i], p, polygon[next]);

				count++;
			}
			i = next;
		} while (i != 0);

		// Return true if count is odd, false otherwise
		return count & 1;  // Same as (count%2 == 1)
	}
	void gobehindBall(BeliefState *state, int botID){
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
		//calculate the frame interpolation which is efficient based on the distance of the ball and the bot

		bool reflections;
		Vec2D dpoint = guess_finalCoord(state->ballPos, getBallDirection(state), FRAME_INTERPOLATION, reflections, reflections);
		dpoint.x -= BOT_RADIUS - BALL_RADIUS;

		Vec2D goal = Vec2D(HALF_FIELD_MAXX, 0);
		float angle = Vec2D::angle(goal, state->homePos[botID]);

		GoToPoint(botID, state, dpoint, angle, true, false);
	}

	void follow_and_kick(BeliefState *state, int supporter, int kicker){
		Vec2D tem = getBallDirection(state);
		int yfin = guess_finalYCoord(state->ballPos, Vector2D<float>(tem.x, tem.y), 4270);
		int goalYrange = 720;

		if (abs(yfin) < goalYrange)	//ball is going towards the goal
		{
			GoToBall(supporter, state, true);

			Vec2D temp = state->homePos[supporter];
			temp.y *= -1;
			GoToPoint(kicker, state, temp, -state->homeAngle[supporter], true, false);
		}
		else{
			//drive the ball to the goal
			//if (yfin > goalYrange)	//ball is going below the goal
			//	gobelowBall(state, supporter, false); //go below ball
			//else gobelowBall(state, supporter, true);

			//GoToBall(kicker, state, false);
			int FRAME_INTERPOLATION = 35;
			int max_botmotion_per_frame = 90;

			int dfromball = Vec2D::distSq(state->ballPos, state->homePos[kicker]);
			if (dfromball < max_botmotion_per_frame * max_botmotion_per_frame)
				FRAME_INTERPOLATION = 2;
			else if (dfromball < 4 * max_botmotion_per_frame * max_botmotion_per_frame)
				FRAME_INTERPOLATION = 4;
			else if (dfromball < 9 * max_botmotion_per_frame * max_botmotion_per_frame)
				FRAME_INTERPOLATION = 6;
			else if (dfromball < 16 * max_botmotion_per_frame * max_botmotion_per_frame)
				FRAME_INTERPOLATION = 9;
			else if (dfromball < 25 * max_botmotion_per_frame * max_botmotion_per_frame)
				FRAME_INTERPOLATION = 13;
			else if (dfromball < 36 * max_botmotion_per_frame * max_botmotion_per_frame)
				FRAME_INTERPOLATION = 18;
			else if (dfromball < 64 * max_botmotion_per_frame * max_botmotion_per_frame)
				FRAME_INTERPOLATION = 24;
			else if (dfromball < 100 * max_botmotion_per_frame * max_botmotion_per_frame)
				FRAME_INTERPOLATION = 30;


			GoToBall(supporter, state, false);
			Vec2D dpoint = Vec2D(HALF_FIELD_MAXX / 2.0, 1000);
			if (state->ballPos.y < 0) dpoint.y *= -1;

			Point a[4];
			a[0].x = a[1].x = 3300;
			a[0].y = a[3].y = 1000; a[1].y = a[2].y = 2250;
			a[3].x = a[2].x = 4275;

			if (state->ballPos.y < 0){
				a[0].y *= -1; a[1].y *= -1; a[2].y *= -1; a[3].y *= -1;
			}
			Point bp;
			bp.x = state->ballPos.x;
			bp.y = state->ballPos.y;
			if (isInside(a, 4, bp)){
				gobehindBall(state, kicker);
			}
			else if (abs(state->ballPos.y) < goalYrange){
				GoToBall(kicker, state, false);
			}
			else
				GoToPoint(kicker, state, dpoint, Vec2D::angle(Vec2D(4230, dpoint.y), state->homePos[kicker]), true, false);
		}
	}

	void gobelowBall(BeliefState *state, int botID, bool gotoTop = false){
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
		//calculate the frame interpolation which is efficient based on the distance of the ball and the bot
		
		bool reflections;
		Vec2D dpoint = guess_finalCoord(state->ballPos, getBallDirection(state), FRAME_INTERPOLATION, reflections, reflections);
		if (gotoTop)
			dpoint.y -= BOT_RADIUS - BALL_RADIUS;
		else
			dpoint.y += BOT_RADIUS - BALL_RADIUS;

		Vec2D goal = Vec2D(HALF_FIELD_MAXX, 0);
		float angle = Vec2D::angle(goal, state->homePos[botID]);

		GoToPoint(botID, state, dpoint, angle, true, false);
	}

	void below_above_attack(BeliefState *state, int botID, int botID1){
		int d, d1;
		d = Vec2D::distSq(state->ballPos, state->homePos[botID]);
		d1 = Vec2D::distSq(state->ballPos, state->homePos[botID1]);
		if (d < 3 * BOT_BALL_THRESH * BOT_BALL_THRESH && d1 < 3 * BOT_BALL_THRESH * BOT_BALL_THRESH){
			if (d < d1){
				Spin(botID, MAX_BOT_OMEGA);
				GoToBall(botID1, state, false);
			}
			else{
				Spin(botID1, -MAX_BOT_OMEGA);//clockwise
				GoToBall(botID, state, false);
			}
		}
		else{
			gobelowBall(state, botID, false);
			gobelowBall(state, botID1, true);//above
		}

	}

  void attacker(BeliefState *state,int botID, bool attack_commenced)
  {
	Vec2D fpoint(HALF_FIELD_MAXX,0);

	static int kicker = 1, supporter = 2;
	Vec2D last_kicker_coords = state->homePos[kicker];
	//decide the roles if this is the attack phase just started
	if (attack_commenced){
		if (state->ballPos.x > state->homePos[1].x && state->ballPos.x > state->homePos[2].x){
			if (Vec2D::distSq(state->ballPos, state->homePos[1]) < Vec2D::distSq(state->ballPos, state->homePos[2])){
				kicker = 2;
				supporter = 1;
			}
			else{
				kicker = 1;
				supporter = 2;
			}
		}
		else if (state->ballPos.x > state->homePos[1].x){
			kicker = 1;
			supporter = 2;
		}
		else{
			kicker = 2;
			supporter = 1;
		}
	}
	
	//follow_and_kick(state, supporter, kicker);

	below_above_attack(state, supporter, kicker);

	last_kicker_coords = state->homePos[kicker];
  }
  

}