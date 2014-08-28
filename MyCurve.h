// MyCurve.h: interface for the MyCurve class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MyCurve_h__
#define __MyCurve_h__

#include <vector>
#include <cmath>
#include <Eigen/Core>

namespace MyCurve
{

typedef Eigen::Vector2d Point;

/*
 *	MyCurve class implements the spline curves given interpolation points
 */
class MyCurve  
{
public:
	//Constructor
	MyCurve();
	//Deconstructor
	virtual ~MyCurve();

//////////////////////////////////////////////////////////////////////////
// Types of interpolations
    enum InterpolationStyle { BERNSTEIN, CASTELJAU, MATRIX, BSPLINE, HERMITE, INVALID_STYLE };


//////////////////////////////////////////////////////////////////////////
// Member functions

	//Add a data point, also set up the two end points if there are more than 1 point.
	void AddPoint( const Point& p );
	//Pick a interpolation point on the screen.
	void PickPoint(float x, float y);
	//Move the picked interpolation point to mouse position.
	void MovePicked(float x, float y);
	//Clear screen. Reset all data vectors to empty and reset number of points to 0.
	void ClearAll();
	//Select the interpolation style
	void SetInterpolationStyle( InterpolationStyle s );
	//Choose whether or not control points will be shown (affects curve calculation).
	void SetShowControlPoints( bool whether );
	//Places the data into the output vectors.
	void GetData( std::vector<Point>& endPoints, std::vector<Point>& interpPoints, std::vector<Point>& ctrlPoints, std::vector<Point>& curve );
	
private:
    // Calls the helper functions below.
    void Recalculate();
    
    //Prepare data for interpolation:1.Calculate the control points 
	void ControlPoints();
	//Interpolation selection
	void Interpolate();
	//Interpolation using CatmullRom method
	void InterpBernstein();
	//Interpolation using de Casteljau method
	void InterpCasteljau();
	//Interpolation using matrix form
	void InterpMatrix();
	//Interpolation using BSpline
	void InterpBSpline();
	//Interpolation using Hermite
	void InterpHermite();
	
	float computeN(int n, int j, float t);
	float computeDN(int n, int j, int t, int d);
	Point BSpline(int degree, int j, float t);

//////////////////////////////////////////////////////////////////////////
// Member variables
private:
    
	//The point that is picked by the user
	Point* picked;
	//Show control points on screen or not
	bool showCtrl;
	//Interpolation style, refer to enum InterpolationStyle
	InterpolationStyle style;
	//Two end points that helps to shape the curve.
	Point endPoints[2];
	//Vector of interpolation points.
	std::vector<Point> interpPoints;
	//Vector of control points generated from all interpolation points.
	std::vector<Point> ctrlPoints;
	//Vector of points that displays the curve.
	std::vector<Point> curve;
	//Vector of Lamda.
	std::vector<float> L;
};

} // ~MyCurve

#endif // __MyCurve_h__
