// MyCurve.cpp: implementation of the MyCurve class.
//
//////////////////////////////////////////////////////////////////////

#include "MyCurve.h"
#include "matrix.h"

#define DIVISIONS 20
#define GL_PI  3.1415926535f

// Call these to raise a dialog box or log to the javascript console for debugging:
extern void jsAlert( const char* msg );
extern void jsLog( const char* msg );

namespace MyCurve
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyCurve::MyCurve()
{
	picked = NULL;
	style = INVALID_STYLE;
	showCtrl = false;
}

MyCurve::~MyCurve()
{

}

void MyCurve::AddPoint(float x, float y)
{
	Point tmp(x, y);
	interpPoints.push_back(tmp);
	float n;
	if (interpPoints.size() >= 2){
		//If there are more than 1 interpolation point, set up the 2 end points to help determine the curve.
		//They lie on the tangent of the first and last interpolation points.
		tmp = interpPoints[0] - interpPoints[1];
		n = tmp.Norm();
		endPoints[0] = interpPoints[0] + tmp / n * 50;
		tmp = interpPoints[interpPoints.size()-1] - interpPoints[interpPoints.size()-2];
		n = tmp.Norm();
		endPoints[1] = interpPoints[interpPoints.size()-1] + tmp / n * 50;
	}
	
    Recalculate();
}

#if 0
void MyCurve::DrawCircle(Point p)
{
	int vertices = 20;
	double r = 5.0;
	double tmpX,tmpY;
	double Angle, Angle0;
	
	Angle = (2*GL_PI)/vertices;
	Angle0 = 0.0;
	
	tmpX=p.x;
	tmpY=p.y;
	
	glBegin(GL_POLYGON);
	
	for (int i = 0; i < vertices; i++) {
		glVertex2d(tmpX + r * cos(i*Angle+Angle0), tmpY + r * sin(i*Angle+Angle0));
	}
	glEnd();
}
#endif

void MyCurve::PickPoint(float x, float y)
{
	float radius = 5.0;
	picked = NULL;
	Point tmp = Point(x, y);
	if (dist(tmp, endPoints[0]) < radius){
		picked = endPoints;
		return;
	}
	if (dist(tmp, endPoints[1]) < radius){
		picked = endPoints+1;
		return;
	}
	for (unsigned int i = 0; i < interpPoints.size(); i++){
		if (dist(tmp, interpPoints[i]) < radius){
			picked = &(interpPoints[i]);
			return;
		}
	}
}

void MyCurve::MovePicked(float x, float y)
{
	if (picked != NULL){
		(*picked).x = x;
		(*picked).y = y;
		
		Recalculate();
	}
}

void MyCurve::ClearAll()
{
	interpPoints.clear();
	ctrlPoints.clear();
	curve.clear();
}

#if 0
void MyCurve::DrawCurve()
{
	int i;
	if (interpPoints.empty())
		return;
	if (interpPoints.size() == 1){//if there is only 1 interpolation point, draw it.
		glColor3f(0,0,1);
		DrawCircle(interpPoints[0]);
	}else{//if there are more than 1 point, draw the curve.
		if (style != BSPLINE && style != HERMITE){
			glColor3f(0.0,1.0,0.0);
			//Draw the two end points.
			DrawCircle(endPoints[0]);
			DrawCircle(endPoints[1]);
			//Connect end points with interpolation points with straight lines.
			glColor3f(0.0,1.0,0.0);
			glBegin(GL_LINES);
			glVertex2d(endPoints[0].x, endPoints[0].y);
			glVertex2d(interpPoints[0].x, interpPoints[0].y);		
			glVertex2d(interpPoints[interpPoints.size() - 1].x, interpPoints[interpPoints.size() - 1].y);
			glVertex2d(endPoints[1].x, endPoints[1].y);
			glEnd();
		}
		//Calculate control points
		ControlPoints();
		//Interpolate the curve
		Interpolate();
		//Draw the curve
		glColor3f(1.0, 0.0, 0.0);
		for (i = 0; i < int(curve.size() - 1); i++){
			glBegin(GL_LINES);
			glVertex2d(curve[i].x, curve[i].y);
			glVertex2d(curve[i + 1].x, curve[i + 1].y);
			glEnd();
		}
		
		//Draw interpolation points.
		glColor3f(0.0,0.0,1.0);
		for (i = 0; i < interpPoints.size(); i++){ 
			DrawCircle(interpPoints[i]);
		}
		//Draw control points and lines that connect them
		if (showCtrl && ctrlPoints.size() > 0){
			glColor3f(1.0, 1.0, 0.0);
			//B-spline, draw its control points
			if (style == BSPLINE){
				for (i = 0; i < interpPoints.size() + 1; i++){
					DrawCircle(ctrlPoints[i]);
					glBegin(GL_LINES);
					glVertex2d(ctrlPoints[i].x, ctrlPoints[i].y);
					glVertex2d(ctrlPoints[i + 1].x, ctrlPoints[i + 1].y);
					glEnd();
				}
				DrawCircle(ctrlPoints[interpPoints.size() + 1]);
			}else
				if (style == HERMITE){
					for (i = 0; i < interpPoints.size(); i++)
					{
						DrawCircle(interpPoints[i] + ctrlPoints[i]);
						glBegin(GL_LINES);
						glVertex2d(interpPoints[i].x, interpPoints[i].y);
						glVertex2d(interpPoints[i].x + ctrlPoints[i].x, interpPoints[i].y + ctrlPoints[i].y);
						glEnd();
					}
				}
				else{
				//Bezier curve, draw its control points
					for (i = 0; i < interpPoints.size() - 1; i++){
						DrawCircle(ctrlPoints[i * 2]);
						DrawCircle(ctrlPoints[i * 2 + 1]);
						glBegin(GL_LINES);
						glVertex2d(interpPoints[i].x, interpPoints[i].y);
						glVertex2d(ctrlPoints[i * 2].x, ctrlPoints[i * 2].y);
						
						glVertex2d(ctrlPoints[i * 2].x, ctrlPoints[i * 2].y);
						glVertex2d(ctrlPoints[i * 2 + 1].x, ctrlPoints[i * 2 + 1].y);
						
						glVertex2d(ctrlPoints[i * 2 + 1].x, ctrlPoints[i * 2 + 1].y);
						glVertex2d(interpPoints[i + 1].x, interpPoints[i + 1].y);
						glEnd();
					}
			}			
		}				
	}
}
#endif

void MyCurve::GetData( vector<Point>& endPoints_out, vector<Point>& interpPoints_out, vector<Point>& ctrlPoints_out, vector<Point>& curve_out )
{
    endPoints_out.clear();
    if( interpPoints.size() >= 2 )
    {
        endPoints_out.resize( 2 );
        endPoints_out.at(0) = endPoints[0];
        endPoints_out.at(1) = endPoints[1];
    }
    
    interpPoints_out = interpPoints;
    ctrlPoints_out = ctrlPoints;
    curve_out = curve;
}

void MyCurve::Recalculate()
{
    if( interpPoints.size() >= 2 )
    {
        //Calculate control points
        ControlPoints();
        //Interpolate the curve
        Interpolate();
    }
}

void MyCurve::SetInterpolationStyle( InterpolationStyle s )
{
    style = s;
    Recalculate();
}

void MyCurve::Interpolate()
{
	//Clear the old curve points
	curve.clear();
	//Depending on the selected style, interpolate the curve.
	switch( style )
	{
	case BERNSTEIN:	InterpBernstein(); break;
	case CASTELJAU:	InterpCasteljau(); break;
	case MATRIX:	InterpMatrix(); break;
	case BSPLINE:	InterpBSpline(); break;
	case HERMITE:   InterpHermite(); break;
	case INVALID_STYLE: break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Calculate the control points
//////////////////////////////////////////////////////////////////////////
// This function utilizes the following member variables 
// interpPoints	- type: vector<Point>
//				  discription: stores all the interpolation points
// endPoints	- type: Point[2]
//				  discription: stores the two end points tangent to the first and last interpolation point
// This function modifies the following member variables
// ctrlPoints	- type: vector<Point>
//				  discription: stores all the control points for curve interpolation and display.
//                             For Bezier curve, between very pair of consecutive interpolation points,
//                             there should be two control points. These four points determins the curve interpolation.
//                             For B-Spline, there should be interpPoints.size() + 2 control points calculated from Ac = p.
// Hint: If you want to implement B-Spline, you need to write functions to create the A matrix as in the handouts.
//       Then you solve a linear system Ac = p, where p is the interpolation points vector and c are the control points.
//       We have provided you with a datastructure to store and solve the linear system.
//       Below is an example code, read the understand it.
//
//	matrix<float> A(3,3);
//  matrix<float> c(3,1);
//  matrix<float> p(3,1);
//  A(0,0) = 1.0; A(0,1) = 0.0; A(0,2) = 0.0;
//  A(1,0) = 0.0; A(1,1) = 1.0; A(1,2) = 0.0;
//  A(2,0) = 0.0; A(2,1) = 0.0; A(2,2) = 1.0;
//  p(0,0) = 1.0; p(1,0) = 2.0; p(2,0) = 3.0;
//  c = A.Solve(p);
//
//  The result in c is c(0,0) = 1.0; c(1,0) = 2.0; c(3,0) = 3.0, which satisfies Ac = p.

void MyCurve::ControlPoints()
{
	// ADD YOUR CODE HERE

	switch(style)
	{
	case BERNSTEIN:
		break;
	case CASTELJAU:
		break;
	case MATRIX:
		break;
	case BSPLINE:
		break;
	case HERMITE:
		
		
		//////////////////////////////////////////////////////////////////////////
		// In the case for Hermite, you want to implement both "clamped" and "natural" versions.
		// Use the following code the determine the end points slopes.
		// Place them after you have computed the other control points.
		//
		//if (showCtrl)
		//{
		//	ctrlPoints[0] = endPoints[0] - interpPoints[0];
		//	ctrlPoints[interpPoints.size() - 1] = endPoints[1] - interpPoints[interpPoints.size() - 1];
		//}		
		//else
		//{
		//	endPoints[0] = interpPoints[0] + ctrlPoints[0];
		//	endPoints[1] = interpPoints[interpPoints.size() - 1] + ctrlPoints[interpPoints.size() - 1];
		//}
		
		break;
    
    case INVALID_STYLE:
	    break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Cubic BERNSTEIN Bezier Spline
//////////////////////////////////////////////////////////////////////////
// This function utilizes the following member variables 
// interpPoints	- type: vector<Point>
//				  discription: stores all the interpolation points
// ctrlPoints	- type: vector<Point>
//				  discription: stores the control points that helps to determine the curve.
//                             Between very pair of consecutive interpolation points,there should be two control points.
//                             These four points determins the curve interpolation.
// endPoints	- type: Point[2]
//				  discription: stores the two end points tangent to the first and last interpolation point
// This function modifies the following member variables
// curve		- type: vector<Point>
//				  discription: stores all the points that form the curve, including all interpolation points 
void MyCurve::InterpBernstein(){
	// ADD YOUR CODE HERE
}

//////////////////////////////////////////////////////////////////////////
// Cubic de Casteljau Bezier Spline
//////////////////////////////////////////////////////////////////////////
// This function utilizes the following member variables 
// interpPoints	- type: vector<Point>
//				  discription: stores all the interpolation points
// ctrlPoints	- type: vector<Point>
//				  discription: stores the control points that helps to determine the curve.
//                             Between very pair of consecutive interpolation points,there should be two control points.
//                             These four points determins the curve interpolation.
// endPoints	- type: Point[2]
//				  discription: stores the two end points tangent to the first and last interpolation point
// This function modifies the following member variables
// curve		- type: vector<Point>
//				  discription: stores all the points that form the curve, including all interpolation points 
void MyCurve::InterpCasteljau(){
	//ADD YOUR CODE HERE
	
}

//////////////////////////////////////////////////////////////////////////
// Cubic Matrix Form Bezier Spline
//////////////////////////////////////////////////////////////////////////
// This function utilizes the following member variables 
// interpPoints	- type: vector<Point>
//				  discription: stores all the interpolation points
// ctrlPoints	- type: vector<Point>
//				  discription: stores the control points that helps to determine the curve.
//                             Between very pair of consecutive interpolation points,there should be two control points.
//                             These four points determins the curve interpolation.
// endPoints	- type: Point[2]
//				  discription: stores the two end points tangent to the first and last interpolation point
// This function modifies the following member variables
// curve		- type: vector<Point>
//				  discription: stores all the points that form the curve, including all interpolation points 
void MyCurve::InterpMatrix(){
	//ADD YOUR CODE HERE

}

//////////////////////////////////////////////////////////////////////////
// BSpline curve
//////////////////////////////////////////////////////////////////////////
// This function utilizes the following member variables 
// interpPoints	- type: vector<Point>
//				  discription: stores all the interpolation points
// ctrlPoints	- type: vector<Point>
//				  discription: stores the control points that helps to determine the curve.
//                             There should be interpPoints.size() + 2 control points.
// endPoints	- type: Point[2]
//				  discription: stores the two end points tangent to the first and last interpolation point
// This function modifies the following member variables
// curve		- type: vector<Point>
//				  discription: stores all the points that form the curve, including all interpolation points
void MyCurve::InterpBSpline(){
	//ADD YOUR CODE HERE
	
}

//////////////////////////////////////////////////////////////////////////
// Bonus Points: Hermite Spline curve
//////////////////////////////////////////////////////////////////////////
// This function utilizes the following member variables 
// interpPoints	- type: vector<Point>
//				  discription: stores all the interpolation points
// ctrlPoints	- type: vector<Point>
//				  discription: stores the control points that helps to determine the curve.
//                             There should be interpPoints.size() control points.
// This function modifies the following member variables
// curve		- type: vector<Point>
//				  discription: stores all the points that form the curve, including all interpolation points
void MyCurve::InterpHermite()
{
	//ADD YOUR CODE HERE

}

} // ~MyCurve
