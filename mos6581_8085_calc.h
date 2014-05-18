//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6581_8085_calc.h                   //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS_6581_8085_CALC_H
#define MOS_6581_8085_CALC_H

#if SPLINE_BRUTE_FORCE
#define interpolate_segment interpolate_brute_force
#else
#define interpolate_segment interpolate_forward_difference
#endif

inline
void cubic_coefficients(double x1, double y1, double x2, double y2,
			double k1, double k2,
			double& a, double& b, double& c, double& d)
{
  double dx = x2 - x1, dy = y2 - y1;

  a = ((k1 + k2) - 2*dy/dx)/(dx*dx);
  b = ((k2 - k1)/dx - 3*(x1 + x2)*a)/2;
  c = k1 - (3*x1*a + 2*b)*x1;
  d = y1 - ((x1*a + b)*x1 + c)*x1;
}

template<class PointPlotter>

inline
void interpolate_brute_force(double x1, double y1, double x2, double y2,
			     double k1, double k2,
			     PointPlotter plot, double res)
{
  double a, b, c, d;
  cubic_coefficients(x1, y1, x2, y2, k1, k2, a, b, c, d);
  
  // Calculate each point.
  for (double x = x1; x <= x2; x += res) {
    double y = ((a*x + b)*x + c)*x + d;
    plot(x, y);
  }
}

template<class PointPlotter>
inline
void interpolate_forward_difference(double x1, double y1, double x2, double y2,
				    double k1, double k2,
				    PointPlotter plot, double res)
{
  double a, b, c, d;
  cubic_coefficients(x1, y1, x2, y2, k1, k2, a, b, c, d);
  
  double y = ((a*x1 + b)*x1 + c)*x1 + d;
  double dy = (3*a*(x1 + res) + 2*b)*x1*res + ((a*res + b)*res + c)*res;
  double d2y = (6*a*(x1 + res) + 2*b)*res*res;
  double d3y = 6*a*res*res*res;
    
  // Calculate each point.
  for (double x = x1; x <= x2; x += res) {
    plot(x, y);
    y += dy; dy += d2y; d2y += d3y;
  }
}

template<class PointIter>
inline
double x(PointIter p)
{
  return (*p)[0];
}

template<class PointIter>
inline
double y(PointIter p)
{
  return (*p)[1];
}

template<class PointIter, class PointPlotter>
inline
void interpolate(PointIter p0, PointIter pn, PointPlotter plot, double res)
{
  double k1, k2;

  // Set up points for first curve segment.
  PointIter p1 = p0; ++p1;
  PointIter p2 = p1; ++p2;
  PointIter p3 = p2; ++p3;

  // Draw each curve segment.
  for (; p2 != pn; ++p0, ++p1, ++p2, ++p3) {
    // p1 and p2 equal; single point.
    if (x(p1) == x(p2)) {
      continue;
    }
    // Both end points repeated; straight line.
    if (x(p0) == x(p1) && x(p2) == x(p3)) {
      k1 = k2 = (y(p2) - y(p1))/(x(p2) - x(p1));
    }
    // p0 and p1 equal; use f''(x1) = 0.
    else if (x(p0) == x(p1)) {
      k2 = (y(p3) - y(p1))/(x(p3) - x(p1));
      k1 = (3*(y(p2) - y(p1))/(x(p2) - x(p1)) - k2)/2;
    }
    // p2 and p3 equal; use f''(x2) = 0.
    else if (x(p2) == x(p3)) {
      k1 = (y(p2) - y(p0))/(x(p2) - x(p0));
      k2 = (3*(y(p2) - y(p1))/(x(p2) - x(p1)) - k1)/2;
    }
    // Normal curve.
    else {
      k1 = (y(p2) - y(p0))/(x(p2) - x(p0));
      k2 = (y(p3) - y(p1))/(x(p3) - x(p1));
    }

    interpolate_segment(x(p1), y(p1), x(p2), y(p2), k1, k2, plot, res);
  }
}

template<class F>
class PointPlotter
{
 protected:
  F* f;

 public:
  PointPlotter(F* arr) : f(arr)
  {
  }

  void operator ()(double x, double y)
  {
    // Clamp negative values to zero.
    if (y < 0) {
      y = 0;
    }

    f[F(x)] = F(y);
  }
};
#endif // MOS_6581_8085_CALC_H
