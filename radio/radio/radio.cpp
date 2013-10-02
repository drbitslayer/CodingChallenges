// radio.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

inline float pwr2(float a){ 
	return a * a; 
}

class Point;
class xy {   
    public:
	  float x;
	  float y;
	  operator Point();
};

class Point : public xy {
	public:		
		Point(){ x = 0; y = 0; };
		Point(float _x, float _y) { x = _x; y = _y; };
		void Translate( float dx, float dy ) { x += dx; y += dy; };		
		float Distance( Point const &p ) {
			return sqrt( pwr2( p.x - x ) + pwr2( p.y - y) );
		}
};

xy::operator Point(){
	return Point(x,y);
}

class Circle {
	
	public:
		typedef enum { InfiniteResults, NoResults, OneResult, TwoResults } IntersectionType;
		struct IntersectionResult {
			 IntersectionType resultType;
			 Point p1;
			 Point p2;
			 IntersectionResult() {
				 resultType = TwoResults;
			 };
		};

		Circle( float x, float y, float _r) { c.x = x; c.y = y; r = _r; };

		Circle( Point p, float _r) { c = p; r = _r; };

		IntersectionResult Intersection( Circle& p2 );
		Point const& center(){ return c; };
		float radius(){ return r; };

	protected:
		Point c; // center
		float r; // radius
};

Circle::IntersectionResult Circle::Intersection( Circle& c2 ) {
	float d = c.Distance( c2.center() );
	IntersectionResult result;
	
	// no solution - if the distance between the two circles is further than the combined radius
	if ( d > radius() + c2.radius() ){     
	   result.resultType = NoResults;
	}
	// no results - if the one circle is completely inside the other
	else if ( d < abs(radius() - c2.radius() ) ){
		result.resultType = NoResults; 
	}
	// circles are on top of each other
	else if ( d == 0 && radius() == c2.radius() ) {
		result.resultType = NoResults;  
	}
	else if ( d == radius() + c2.radius() ) {
		result.resultType = OneResult;
	}

	if ( result.resultType != OneResult && result.resultType != TwoResults ) {
	   return result;
	}

	float h; // common side of the right triangles that result from the interesection of the circle

	// distance of the leg from this perpendicular to +-h leading to point(s)
	// however if the distance between points is also the same as the sum of radius then there is only one point
	float legDist = radius(); 
	
	legDist = ( pwr2( radius() ) - pwr2( c2.radius() ) + pwr2( d ) )/(2 * d);
	h = sqrt( pwr2( radius() ) - pwr2( legDist ) );
	
	Point bp;  // point that lies on the line between P1 and P2 that makes the two right triangles between them

	bp.x = center().x + legDist * ( c2.center().x - center().x ) / d;
	bp.y = center().y + legDist * ( c2.center().y - center().y ) / d;
	
	Point p1;
	Point p2;
	p1 = c2.center();
	p2 = c2.center();

	p1.x = bp.x + h * ( c2.center().y - center().y) / d;
	p1.y = bp.y - h * ( c2.center().x - center().x) / d;

	p2.x = bp.x - h * ( c2.center().y - center().y) / d;	
	p2.y = bp.y + h * ( c2.center().x - center().x) / d;
	result.p1 = p1;
	result.p2 = p2;

	return result;
}

void OutputTestCase( ofstream& file, Point txmitter, Point p1, Point p2 ) {

	file << p1.x << " " << p1.y << " " << p1.Distance( txmitter ) << "\n";
	file << p2.x << " " << p2.y << " " << p2.Distance( txmitter ) << "\n";

	if ( p1.x == p2.x && p1.y == p2.y ) {
		file << -1 << -1 << "\n";
	}
	else {
	   file << txmitter.x << " " << txmitter.y << "\n";
	}

}

float randomAngle() {
	static float pi = (float)(atan(1)*4);
	return (float)((rand() % 360 ) * pi/180.0f);
}

Point PolarToXY( xy const & origin , float angle, float dist ) {
	Point p;
	p.x = origin.x + cos( angle ) * dist;
	p.y = origin.y + sin( angle ) * dist;
	return p;
}

void GenerateTestCases() {

   ofstream myfile;
   myfile.open( "testcases.txt" );
   
   xy transmitters[] = { {3, 4 }, { 3, 2 }, { 5, 4 }, { 8, 2 }, { 1, 9 }, { 7, 4 } };
   int distances[] = { 5, 4, 1, 10, 4, 3 }; 

   int numTransmitters = sizeof( transmitters ) /sizeof( xy );
   
   // include basic test case of a 3,4,5 triangle and one solution not two
   Point p1(0,0);
   Point p2(6,8);

   OutputTestCase( myfile, transmitters[0], p1, p2 );
   // generate random distances from point
   for( int i = 1; i < numTransmitters; i++ ) {
	  Point xmit;
	  
	  float dist1 = 2 + (float)(rand() % 10);  // first distance is always 2 or more
	  float dist2 = 3 + (float)(rand() % 15);  // second distance is always 3 or more

	  float angle = randomAngle();

	  p1 = PolarToXY( transmitters[i], randomAngle(), dist1 );
	  p2 = PolarToXY( transmitters[i], randomAngle(), dist2 ); 	  
	  OutputTestCase( myfile, transmitters[i], p1, p2 );   
   }
   myfile.close();

}

void ShowResult(Circle::IntersectionResult r ) {
	switch ( r.resultType ) {
	   case Circle::IntersectionType::InfiniteResults:
		   cout << "Can't compute, there are infinite results\n";
		   break;
	   case Circle::IntersectionType::NoResults:
		   cout << "No results possible, distances between points is further than the combined beacon distance \n";
		   break;

	   case Circle::IntersectionType::OneResult:
		   cout << r.p1.x << " " << r.p1.y << "\n";		
		   break;
	   case Circle::IntersectionType::TwoResults:
		   cout << r.p1.x << " " << r.p1.y << "\n";		
		   cout << r.p2.x << " " << r.p2.y << "\n";		
		   break;
	   default:
		   cout << "Should not get here...\n";
		   break;	  
	}
}

Circle::IntersectionResult FindRadio( Circle& c1, Circle &c2 ) {
	return c1.Intersection( c2 );
}

void PerformTests() {

   ifstream myfile;
   myfile.open( "testcases.txt" );

   float x, y, r;
   float rx, ry;
   while( !myfile.eof() ) {
	   myfile >> x >> y >> r;	   
	   cout << "P1 is @ " << x << " " << y << " radio is " << r << " units away\n";
	   Circle c1( x, y, r );
	   myfile >> x >> y >> r;
	   cout << "P2 is @ " << x << " " << y << " radio is " << r << " units away\n";
	   Circle c2( x, y, r );
	   myfile >> rx >> ry;
	   cout << "Answer is " << rx << " " << ry << "\n";

	   Circle::IntersectionResult result = FindRadio( c1, c2 );
	   cout << "Computed solution\n";
	   ShowResult( result );
	   
   }

   myfile.close();
   
}


int _tmain(int argc, _TCHAR* argv[])
{

	GenerateTestCases();
	PerformTests();

	char ch;
	cin >> ch;
	return 0;
}

