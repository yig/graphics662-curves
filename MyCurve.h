// MyCurve.h: interface for the MyCurve class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MyCurve_h__
#define __MyCurve_h__

#include <vector>
using std::vector;
#include <cmath>

/*
 *	Point structure
 */
struct Point {
	Point(){
		x = 0;
		y = 0;
	}
	Point(float px, float py){
		x = px;
		y = py;
	}
	Point& operator = (const Point &p){
		x = p.x;
		y = p.y;
		return (*this);
	}
	Point operator - (const Point &p){
		Point tmp;
		tmp.x = x - p.x;
		tmp.y = y - p.y;
		return tmp;
	}
	Point operator + (const Point &p){
		Point tmp;
		tmp.x = x + p.x;
		tmp.y = y + p.y;
		return tmp;
	}
	Point operator / (const double s){
		Point tmp;
		tmp.x = x / s;
		tmp.y = y / s;
		return tmp;
	}	
	Point operator * (const double s){
		Point tmp;
		tmp.x = x * s;
		tmp.y = y * s;
		return tmp;
	}
	float Norm(){
		return sqrt(x*x + y*y);
	}
	float x;
	float y;
};


inline Point operator - (Point& p1){
	Point tmp;
	tmp.x = -p1.x;
	tmp.y = -p1.y;
	return tmp;
}

inline Point operator + (Point& p1, Point& p2){
	Point tmp;
	tmp.x = p1.x + p2.x;
	tmp.y = p1.y + p2.y;
	return tmp;
}

inline Point operator - (Point& p1, Point& p2){
	Point tmp;
	tmp.x = p1.x - p2.x;
	tmp.y = p1.y - p2.y;
	return tmp;
}

inline Point operator * (Point& p1, float s){
	Point tmp;
	tmp.x = p1.x * s;
	tmp.y = p1.y * s;
	return tmp;
}

inline Point operator * (float s , Point& p1){
	Point tmp;
	tmp.x = p1.x * s;
	tmp.y = p1.y * s;
	return tmp;
}

inline Point operator / (Point& p1, float s){
	Point tmp;
	tmp.x = p1.x / s;
	tmp.y = p1.y / s;
	return tmp;
}

inline float dist(Point &p1, Point &p2){
	Point tmp = p1 - p2;
	return tmp.Norm();
}

/*
 *	Types of interpolations
 */
enum {BERSTEIN, CASTELJAU, MATRIX, BSPLINE, HERMITE};

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
// Member functions

	//Draw the curve.
	void DrawCurve();
	//Draw a circle to indicate the points.
	void DrawCircle(Point p);
	//Add a data point, also set up the two end points if there are more than 1 point.
	void AddPoint(float x, float y);
	//Pick a interpolation point on the screen.
	void PickPoint(float x, float y);
	//Move the picked interpolation point to mouse position.
	void MovePicked(float x, float y);
	//Clear screen. Reset all data vectors to empty and reset number of points to 0.
	void ClearAll();
	//Prepare data for interpolation:1.Calculate the control points 
	void ControlPoints();
	//Interpolation selection
	void Interpolate();
	//Interpolation using CatmullRom method
	void InterpBerstein();
	//Interpolation using de Casteljau method
	void InterpCasteljau();
	//Interpolation using matrix form
	void InterpMatrix();
	//Interpolation using BSpline
	void InterpBSpline();
	//Interpolation using Hermite
	void InterpHermite();

	
//////////////////////////////////////////////////////////////////////////
// Member variables

	//Total number of interpolation points
	int totalPoints;
	//The point that is picked by the user
	Point* picked;
	//Show control points on screen or not
	bool showCtrl;
	//Interpolation style, refer to enum {CATMULLROM, CASTELJAU, MATRIX, BSPLINE, HERMITE}.
	int style;
	//Two end points that helps to shape the curve.
	Point endPoints[2];
	//Vector of interpolation points.
	vector<Point> interpPoints;
	//Vector of control points generated from all interpolation points.
	vector<Point> ctrlPoints;
	//Vector of points that displays the curve.
	vector<Point> curve;
	//Vector of Lamda.
	vector<float> L;
};

#endif // __MyCurve_h__
