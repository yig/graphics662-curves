// MyCurve.cpp: implementation of the MyCurve class.
//
//////////////////////////////////////////////////////////////////////

#include "MyCurve.h"
#include "matrix.h"
using math::matrix;

// Call these to raise a dialog box or log to the javascript console for debugging.
// NOTE: You can pass either a const char* or an std::string.
extern void jsAlert( const std::string& msg );
extern void jsLog( const std::string& msg );

#define DIVISIONS 20

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

void MyCurve::SetShowControlPoints( bool whether )
{
    showCtrl = whether;
    Recalculate();
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
// Hint: To implement B-Spline, you need to write functions to create the A matrix as in the handouts.
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
// Hermite Spline curve
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
