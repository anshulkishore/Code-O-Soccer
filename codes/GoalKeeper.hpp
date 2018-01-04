#pragma once
#include "skills.h"
#include <cmath>

/*
	Vec2D can be used to make variables that can store points as (x,y) and provide utility functions.
	Vec2D point1;
	Vec2D point2;
	point1.x = HALF_FIELD_MAXX;
	point1.y = 0;
	similarly for point2 can be specified
	Some utility functions:
	These will appear automatically as you type Vec2D::
	float angle = Vec2D::angle(point1,point2);   
	int distance = Vec2D::distSq(point1,point2);
*/

namespace MyStrategy
{

	const int addlen = 500;
	const int addlen2=1200;
	const int x_goalie =  500+OUR_GOAL_X;
	const float err_anglethreshold = 0.1;	//0.087radian = 5deg
	const float err_motion = BOT_RADIUS - 10;

	
	bool pr_ballinmidfield(BeliefState *state)
	{
		const int midlen = HALF_FIELD_MAXX/(2.0);
		if(state->ballPos.x > -midlen && state->ballPos.x < midlen)
			return true;
		else
			return false;
	}
	
	bool pr_ballingoalarena(BeliefState *state)
	{
		int y_ = state->ballPos.y;

		return ((y_ < OUR_GOAL_MAXY + addlen) && (y_ > OUR_GOAL_MINY - addlen) && (state->ballPos.x < 0) && (!pr_ballinmidfield(state)));
	}

	
  void goalkeeper(BeliefState *state,int botID)
  {
    int a=state->oppBotNearestToBall;
	int d = Vector2D<int>::distSq(state->awayPos[a], state->ballPos);
	int d2 = Vector2D<int>::distSq(state->homePos[0], state->ballPos);


	int y_g2,x_g=state->ballPos.x, y_g = state->ballPos.y;
	int x1=state->homePos[0].x,y1=state->homePos[0].y,x3,y3;
	y_g2 = guess_finalYCoord(state->ballPos, state->ballVel, x_goalie);
    
	int d3=((x1 - x_goalie) * (x1-x_goalie) + (y1 - y_g2) * (y1 - y_g2));
   
	if( state->ballPos.x > HALF_FIELD_MAXX / 2.5)
	{
		GoToPoint(0, state, Vector2D<int> (x_goalie, 0), PI/2, true, false);
		return;
	}
	
	else 
		if(d>d2 && d2<1400 && (state->ballPos.x>OUR_GOAL_MAXY || state->ballPos.x<OUR_GOAL_MINY))
		{
			if(state->ballPos.y > state->homePos[0].y)
			Spin(0, MAX_BOT_OMEGA);
		   else
	       Spin(0, -MAX_BOT_OMEGA);
	}
		else if(d2<25*BOT_RADIUS*BOT_RADIUS&& (y1<4*DBOX_HEIGHT)&&(y1>-4*DBOX_HEIGHT))
	{ 
		x3=x_g+(5*BOT_RADIUS)*(x_g-x1)/sqrt(d2);
		y3=y_g+(5*BOT_RADIUS)*(y_g-y1)/sqrt(d2);
		GoToPointStraight(0, state, Vector2D<int> (x3,y3), PI/2, true, false);
	}

	else if(y_g>2*DBOX_HEIGHT ||y_g<-2*DBOX_HEIGHT || x_g>-HALF_FIELD_MAXX+DBOX_WIDTH  )
	{
		if((x_g< -HALF_FIELD_MAXX + 0.5* DBOX_WIDTH)&&(y_g>OUR_GOAL_MAXY))
		{
			
		GoToPointStraight(0, state, Vector2D<int> (x_goalie,OUR_GOAL_MAXY + addlen ), PI/2, true, false);
		}
		else if((x_g< -HALF_FIELD_MAXX + 0.5* DBOX_WIDTH)&&(y_g<OUR_GOAL_MINY))

			GoToPointStraight(0, state, Vector2D<int> (x_goalie,OUR_GOAL_MINY - addlen ), PI/2, true, false);

	
		else if( state->ballVel.x > 0) 
	
	{
		if(y_g > OUR_GOAL_MAXY + addlen)
			y_g = OUR_GOAL_MAXY/2;
		
		else if(	y_g < OUR_GOAL_MINY - addlen)
			y_g = OUR_GOAL_MINY/2;
		

		GoToPointStraight(0, state, Vector2D<int> (x_goalie, y_g), PI/2, true, false);
	}
	 else if(y_g2<OUR_GOAL_MAXY && y_g2>OUR_GOAL_MINY)
	 {
	    
		 y3=y_g2+sqrt(d3)*(y_g2-y1)/sqrt(d3);
		
		 GoToPointStraight(0, state, Vector2D<int> (x_goalie, y3), PI/2, true, false);
	 }

	

	else if(state->ballVel.x < 0 && (y_g > OUR_GOAL_MAXY + addlen|| y_g < OUR_GOAL_MINY-addlen))	//ball is coming towards us
	{
		
		

		if((y_g > OUR_GOAL_MAXY + addlen)&&(y_g2>OUR_GOAL_MAXY+addlen))
			y_g = OUR_GOAL_MAXY + addlen;
		
		else if(	(y_g<OUR_GOAL_MINY-addlen )&&(y_g2< OUR_GOAL_MINY - addlen))
			y_g = OUR_GOAL_MINY - addlen;
		else if(y_g>OUR_GOAL_MAXY)
			y_g=0;
		else if(y_g<OUR_GOAL_MINY)
			y_g=0;
		

		GoToPointStraight(0, state, Vector2D<int> (x_goalie, y_g), PI/2, true, false);
		
	}
	else
	{
		y_g2 = guess_finalYCoord(state->ballPos, state->ballVel, x_goalie);
		if(y_g2>OUR_GOAL_MAXY)
			y_g = OUR_GOAL_MAXY;
		else if(y_g2<OUR_GOAL_MINY)
			y_g = OUR_GOAL_MINY;
		
		GoToPointStraight(0, state, Vector2D<int> (x_goalie, y_g), PI/2, true, false);

		
	}
	}



	else if(y_g>OUR_GOAL_MAXY+addlen || y_g<OUR_GOAL_MINY-addlen )
	{ 
		if(state->ballVel.x>0)
		{
			 if(y_g>OUR_GOAL_MAXY)
			y_g=OUR_GOAL_MAXY;
		  else if(y_g<OUR_GOAL_MINY)
			y_g=OUR_GOAL_MINY;

		GoToPointStraight(0, state, Vector2D<int> (x_goalie, y_g), PI/2, true, false);
		}
		else
		{
			y_g2 = guess_finalYCoord(state->ballPos, state->ballVel, x_goalie);
			
			if(y_g2<OUR_GOAL_MAXY || y_g2>OUR_GOAL_MINY)
             GoToPointStraight(0, state, Vector2D<int> (x_goalie, y_g2), PI/2, true, false);
			else if(state->ballPos.x==0)
			{
				if(y_g>OUR_GOAL_MAXY+addlen)
			 GoToPointStraight(0, state, Vector2D<int> (x_goalie,OUR_GOAL_MAXY+addlen ), PI/2, true, false);
				if(y_g<OUR_GOAL_MINY-addlen)
			 GoToPointStraight(0, state, Vector2D<int> (x_goalie,OUR_GOAL_MAXY-addlen ), PI/2, true, false);
			}

		  else if((y_g<(OUR_GOAL_MINY -addlen) )&&(y_g2>0)||(y_g>(OUR_GOAL_MAXY +addlen))&&(y_g2<0))
           GoToPointStraight(0, state, Vector2D<int> (x_goalie, 0), PI/2, true, false);

		 else if((y_g > OUR_GOAL_MAXY + addlen)&&(y_g2>OUR_GOAL_MAXY+addlen))
			 GoToPointStraight(0, state, Vector2D<int> (x_goalie,OUR_GOAL_MAXY + addlen ), PI/2, true, false);

		else if(	(y_g<OUR_GOAL_MINY-addlen )&&(y_g2< OUR_GOAL_MINY - addlen))
			GoToPointStraight(0, state, Vector2D<int> (x_goalie, OUR_GOAL_MINY-addlen), PI/2, true, false);
		else
			GoToPointStraight(0, state, Vector2D<int> (x_goalie, y_g2), PI/2, true, false);
			
		}
	}


	else{
		y_g2 = guess_finalYCoord(state->ballPos, state->ballVel, x_goalie);
		if(x_g>-HALF_FIELD_MAXX+ addlen2)
		{
			if(state->ballVel.x>0)
             GoToPointStraight(0, state, Vector2D<int> (x_goalie, 0), PI/2, true, false);
			else if(y_g2<OUR_GOAL_MAXY || y_g2>OUR_GOAL_MINY)
             GoToPointStraight(0, state, Vector2D<int> (x_goalie, y_g2), PI/2, true, false);
			else if(y_g2>OUR_GOAL_MAXY)
				GoToPointStraight(0, state, Vector2D<int> (x_goalie, OUR_GOAL_MAXY), PI/2, true, false);
			else if(y_g2<OUR_GOAL_MINY)
				GoToPointStraight(0, state, Vector2D<int> (x_goalie, OUR_GOAL_MINY), PI/2, true, false);
		}
		
		else 
		{
			GoToPointStraight(0, state, Vector2D<int> (x_g,y_g ), PI/2, true, false);
			//int diffy=state->ballPos.y-state->homePos[0].y;
			//int diffx=state->ballPos.x-state->homePos[0].x;
			//float tantheta = abs(diffy / diffx);
		}

	}

		


		
	
	  
  }
}
