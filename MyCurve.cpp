// MyCurve.cpp: implementation of the MyCurve class.
//
//////////////////////////////////////////////////////////////////////

#include "MyCurve.h"
#include <Eigen/Core>
#include <Eigen/LU>
using Eigen::MatrixXd;
using std::vector;

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

void MyCurve::AddPoint( const Point& p )
{
	interpPoints.push_back(p);
	if (interpPoints.size() >= 2){
		//If there are more than 1 interpolation point, set up the 2 end points to help determine the curve.
		//They lie on the tangent of the first and last interpolation points.
		Point tmp = interpPoints[0] - interpPoints[1];
		float n = tmp.norm();
		endPoints[0] = interpPoints[0] + tmp / n * 50;
		tmp = interpPoints[interpPoints.size()-1] - interpPoints[interpPoints.size()-2];
		n = tmp.norm();
		endPoints[1] = interpPoints[interpPoints.size()-1] + tmp / n * 50;
	}
	
    Recalculate();
}

void MyCurve::PickPoint(float x, float y)
{
	float radius = 10.0;
	picked = NULL;
	Point tmp = Point(x, y);
	if ((tmp - endPoints[0]).norm() < radius){
		picked = endPoints;
		return;
	}
	if ((tmp - endPoints[1]).norm() < radius){
		picked = endPoints+1;
		return;
	}
	for (unsigned int i = 0; i < interpPoints.size(); i++){
		if ((tmp - interpPoints[i]).norm() < radius){
			picked = &(interpPoints[i]);
			return;
		}
	}
}

void MyCurve::MovePicked(float x, float y)
{
	if (picked != NULL){
		(*picked).x() = x;
		(*picked).y() = y;
		
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


float MyCurve::computeN(int n, int j, float t)
{
	if (t < L[j] || t >= L[j+1+n])
		return 0.0;
	if (n == 0){
		if (t >= L[j] && t < L[j+1])
			return 1.0;
		else
			return 0.0;
	}
	else{
		float v1 = (t - L[j]) / (L[j + n] - L[j]) * computeN(n-1, j, t);
		float v2 = (L[j+n+1] - t) / (L[j + n + 1] - L[j +1]) * computeN(n-1, j+1, t);
		return v1 + v2;
	}
}


float MyCurve::computeDN(int n, int j, int t, int d)
{
	if (d == 0){
		return computeN(n,j,t);
	}else{
		float v1 = 1 / (L[j+n] - L[j]) * computeDN(n-1, j, t, d-1);
		float v2 = 1 / (L[j+n+1] - L[j+1]) * computeDN(n-1, j+1, t, d-1);
		return n * (v1 - v2);
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
// totalPoints	- type: int
//				  discription: total number of interpolation points
// This function modifies the following member variables
// ctrlPoints	- type: vector<Point>
//				  discription: stores all the control points for curve interpolation and display.
//                             For Bezier curve, between very pair of consecutive interpolation points,
//                             there should be two control points. These four points determins the curve interpolation.
//                             For B-Spline, there should be interpPoints.size() + 2 control points calculated from Ac = p.
// Hint: To implement B-Spline, you need to write functions to create the A matrix as in the handouts.
//       Then you solve a linear system Ac = p, where p is the interpolation points vector and c are the control points.
//       We have provided you with a data structure to store and solve the linear system.
//       Below is an example code, read the understand it.
//
//	MatrixXd A(3,3);
//  MatrixXd c(3,1);
//  MatrixXd p(3,1);
//  A(0,0) = 1.0; A(0,1) = 0.0; A(0,2) = 0.0;
//  A(1,0) = 0.0; A(1,1) = 1.0; A(1,2) = 0.0;
//  A(2,0) = 0.0; A(2,1) = 0.0; A(2,2) = 1.0;
//  p(0,0) = 1.0; p(1,0) = 2.0; p(2,0) = 3.0;
//  c = A.fullPivLu().solve(p);
//
//  The result in c is c(0,0) = 1.0; c(1,0) = 2.0; c(3,0) = 3.0, which satisfies Ac = p.

void MyCurve::ControlPoints()
{
	// ADD YOUR CODE HERE
    int totalPoints = interpPoints.size();
    
	// Prepare data
	if (totalPoints < 2)
		return;
	Point tan1, tan2, tmp;
	ctrlPoints.clear();
	int degree = 3;
	int dim;
	if (style == BSPLINE)
		dim = totalPoints + 2;
	else
		dim = totalPoints;
	MatrixXd A(dim, dim);
	MatrixXd C(dim, 2);
	MatrixXd P(dim, 2);

	// Based on style use appropriate ways to compute control points
	switch(style)
	{
	case BERNSTEIN:
	case CASTELJAU:
	case MATRIX:
		for (int i = 0; i < totalPoints-1; i++){
			if (i == 0)
				tan1 = interpPoints[0] + (interpPoints[0]-endPoints[0]);
			else
				tan1 = interpPoints[i] + (interpPoints[i+1]-interpPoints[i-1])/6;
			if (i == totalPoints - 2)
				tan2 = interpPoints[i+1] + (interpPoints[i+1]-endPoints[1]);
			else
				tan2 = interpPoints[i+1] + (interpPoints[i]-interpPoints[i+2])/6;
			ctrlPoints.push_back(tan1);
			ctrlPoints.push_back(tan2);
		}
		break;
	case BSPLINE:
		// Compute coefficients
		L.clear();
		for (int i = 0; i < totalPoints + degree * 2; i++){
			L.push_back(float(i - degree));	
		}
		for (int i = 0; i < dim; i++){
			for (int j = 0; j < dim; j++){
				A(i,j) = 0.0;
			}
		}

		// These computeDN and computeN are recursive functions
		A(0,0) = computeDN(3,0,0,2); A(0,1) = computeDN(3,1,0,2); A(0,2) = computeDN(3,2,0,2); A(0,3) = computeDN(3,3,0,2);
		for (int i = 1; i < dim - 2; i++){
			A(i,i-1) = computeN(3, i - 1, i - 1); 
			A(i, i) = computeN(3, i, i - 1);
			A(i, i+1) = computeN(3, i + 1, i - 1);
			A(i, i+2) = computeN(3, i + 2, i - 1);
		}
		A(dim - 2, dim - 4) = computeN(3, dim - 4, dim - 3);
		A(dim - 2, dim - 3) = computeN(3, dim - 3, dim - 3);
		A(dim - 2, dim - 2) = computeN(3, dim - 2, dim - 3);
		A(dim - 2, dim - 1) = computeN(3, dim - 1, dim - 3);
		A(dim - 1, dim - 4) = computeDN(3, dim - 4, dim - 3, 2); 
		A(dim - 1, dim - 3) = computeDN(3, dim - 3, dim - 3, 2);
		A(dim - 1, dim - 2) = computeDN(3, dim - 2, dim - 3, 2); 
		A(dim - 1, dim - 1) = computeDN(3, dim - 1, dim - 3, 2);
		P(0,0) = 0; P(0,1) = 0;
		for (int i = 1; i < totalPoints + 1; i++){
			P(i,0) = interpPoints[i-1].x();
			P(i,1) = interpPoints[i-1].y();
		}
		P(dim-1,0) = 0; P(dim-1,1) = 0;
		C = A.fullPivLu().solve(P);
		for (int i = 0; i < totalPoints + 2; i++){
			ctrlPoints.push_back(Point(C(i,0), C(i,1)));
		}
		break;
	case HERMITE:
	    // Zero A:
	    A.setZero(dim, dim);
	    
	    // Based on showCtrl, determine the boundary slope either by endPoints or automatically
		if( !showCtrl )
		{
            // Additional constraints: second derivative are zero at end points
            // first row	
            A(0,0) = 2;
            A(0,1) = 1;
            P.row(0) = 3 * (interpPoints[1] - interpPoints[0]);
            
            // last row
            A(dim-1, dim-2) = 1;
            A(dim-1, dim-1) = 2;
            P.row(dim-1) =  3 * (interpPoints[dim-1] - interpPoints[dim-2]);
        }
        else
        {
            // First derivatives are specified by endPoints.
            A(0,0) = 1;
            P.row(0) = endPoints[0] - interpPoints[0];
            
            A(dim-1,dim-1) = 1;
            P.row(dim-1) = endPoints[1] - interpPoints[totalPoints - 1];
        }
		
		//middle rows
		for (int i = 1; i < dim - 1; i++)
		{
			A(i, i-1) = 1; A(i, i) = 4; A(i, i+1) = 1;
		}
		for (int i = 1; i < dim - 1; i++)
		{
			P.row(i) = 3 * (interpPoints[i+1] - interpPoints[i-1]);
		}
		
		C = A.fullPivLu().solve(P);
		for (int i = 0; i < totalPoints; i++){
			ctrlPoints.push_back(Point(C(i,0), C(i,1)));
		}

		// Based on showCtrl, determine the boundary slope either by endPoints or automatically
		if( !showCtrl )
		{
			endPoints[0] = interpPoints[0] + ctrlPoints[0];
			endPoints[1] = interpPoints[totalPoints - 1] + ctrlPoints[totalPoints - 1];
		}
		break;
    case INVALID_STYLE: break;
	}
}

namespace
{
Point Bernstein(Point &p0, Point &p1, Point &p2, Point &p3, float t)
{
	Point tmp;
	tmp = (1-t) * (1-t) * (1-t) * p0
		+ 3 * t * (1-t) * (1-t) * p1
		+ 3 * t * t * (1-t) * p2
		+ t * t * t * p3;
	return tmp;
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
// totalPoints	- type: int
//				  discription: total number of interpolation points
// This function modifies the following member variables
// curve		- type: vector<Point>
//				  discription: stores all the points that form the curve, including all interpolation points 
void MyCurve::InterpBernstein(){
	// ADD YOUR CODE HERE
	int totalPoints = interpPoints.size();
	float t;
	Point p;
	for (int i = 0; i < totalPoints - 1; i++){
		for (int j = 0; j < DIVISIONS; j++){
			t = (float)j / (float)DIVISIONS;
			p = Bernstein(interpPoints[i], ctrlPoints[i * 2], ctrlPoints[i * 2 + 1], interpPoints[i + 1], t);
			curve.push_back(p);
		}
	}
	curve.push_back(interpPoints[totalPoints - 1]);	
}

namespace
{
Point Casteljau(Point &p0, Point &p1, Point &p2, Point &p3, float t)
{
	Point tmp0, tmp1, tmp2;
	tmp0 = p0 * (1-t) + p1 * t;
	tmp1 = p1 * (1-t) + p2 * t;
	tmp2 = p2 * (1-t) + p3 * t;
	tmp0 = tmp0 * (1-t) + tmp1 * t;
	tmp1 = tmp1 * (1-t) + tmp2 * t;
	tmp0 = tmp0 * (1-t) + tmp1 * t;
	return tmp0;
}
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
// totalPoints	- type: int
//				  discription: total number of interpolation points
// This function modifies the following member variables
// curve		- type: vector<Point>
//				  discription: stores all the points that form the curve, including all interpolation points 
void MyCurve::InterpCasteljau(){
	//ADD YOUR CODE HERE
	int totalPoints = interpPoints.size();
	float t;
	Point p;
	for (int i = 0; i < totalPoints - 1; i++){
		for (int j = 0; j < DIVISIONS; j++){
			t = (float)j / (float)DIVISIONS;
			p = Casteljau(interpPoints[i], ctrlPoints[i * 2], ctrlPoints[i * 2 + 1], interpPoints[i + 1], t);
			curve.push_back(p);
		}
	}
	curve.push_back(interpPoints[totalPoints - 1]);
}

namespace
{
Point Matrix(Point &p0, Point &p1, Point &p2, Point &p3, float t)
{
	Point p = (-1*p0 + 3*p1 - 3*p2 + 1*p3)*t*t*t 
		    + ( 3*p0 - 6*p1 + 3*p2)*t*t
		    + (-3*p0 + 3*p1)*t
		    + 1.0*p0;
	return p;
}
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
// totalPoints	- type: int
//				  discription: total number of interpolation points
// This function modifies the following member variables
// curve		- type: vector<Point>
//				  discription: stores all the points that form the curve, including all interpolation points 
void MyCurve::InterpMatrix(){
	//ADD YOUR CODE HERE
	int totalPoints = interpPoints.size();
	float t;
	Point p;
	for (int i = 0; i < totalPoints - 1; i++){
		for (int j = 0; j < DIVISIONS; j++){
			t = (float)j / (float)DIVISIONS;
			p = Matrix(interpPoints[i], ctrlPoints[i * 2], ctrlPoints[i * 2 + 1], interpPoints[i + 1], t);
			curve.push_back(p);
		}
	}
	curve.push_back(interpPoints[totalPoints - 1]);
}

Point MyCurve::BSpline(int degree, int j, float t)
{
	Point p(0,0);
	for (int i = 0; i <= degree; i++){
		p = p + ctrlPoints[j - i] * computeN(degree, j - i, t);
	}
	return p;
}

//////////////////////////////////////////////////////////////////////////
// Bonus Point: BSpline curve
//////////////////////////////////////////////////////////////////////////
// This function utilizes the following member variables 
// interpPoints	- type: vector<Point>
//				  discription: stores all the interpolation points
// ctrlPoints	- type: vector<Point>
//				  discription: stores the control points that helps to determine the curve.
//                             There should be totalPoints + 2 control points.
// endPoints	- type: Point[2]
//				  discription: stores the two end points tangent to the first and last interpolation point
// totalPoints	- type: int
//				  discription: total number of interpolation points
// This function modifies the following member variables
// curve		- type: vector<Point>
//				  discription: stores all the points that form the curve, including all interpolation points
void MyCurve::InterpBSpline(){
	//ADD YOUR CODE HERE
	int totalPoints = interpPoints.size();
	int degree = 3;
	float t;
	int i,j;
	Point p;
	for (i = 0; i < totalPoints - 1; i++){
		for (j = 0; j < DIVISIONS; j++){
			t = float(i) + float(j) / float(DIVISIONS);
			p = BSpline(degree, i + degree, t);
			curve.push_back(p);
		}
	}
	curve.push_back(interpPoints[totalPoints - 1]);
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
Point Hermite(Point& p0, Point& p1, Point& q0, Point& q1, float t)
{
	Point p = (2 * t * t * t - 3 * t * t + 1) * p0 
		    + (t * t * t - 2 * t * t + t) * q0
			+ (-2 * t * t * t + 3 * t * t) * p1
			+ (t * t * t - t * t) * q1;
	return p;
}
void MyCurve::InterpHermite()
{
    int totalPoints = interpPoints.size();
	float t;
	Point p;
	for (int i = 0; i < totalPoints - 1; i++)
		for (int j = 0; j <= DIVISIONS; j++)
		{
			t = float(j) / float(DIVISIONS);
			p = Hermite(interpPoints[i], interpPoints[i + 1], ctrlPoints[i], ctrlPoints[i + 1], t);
			curve.push_back(p);
		}
}

}
