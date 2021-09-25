#pragma once

class SimplexNoise { // Simplex noise in 2D, 3D and 4D

private:

	// This method is a *lot* faster than using (int)Math.floor(x)
	static  int fastfloor(double x);

	static  double dot(int g[], double x, double y);

	static  double dot(int g[], double x, double y, double z);

	static  double dot(int g[], double x, double y, double z, double w);


public:
	SimplexNoise();

	// 2D simplex noise
	static double noise(double xin, double yin);
	// 3D simplex noise
	static double noise(double xin, double yin, double zin);
	// 4D simplex noise
	static double noise(double x, double y, double z, double w);
};