/*
    FLAM3 - cosmic recursive fractal flames
    Copyright (C) 1992-2009 Spotworks LLC

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stddef.h>
#include <limits.h>
#include <float.h>
#include "variations.h"
#include "interpolation.h" 

#define badvalue(x) (((x)!=(x))||((x)>1e10)||((x)<-1e10))

/* Wrap the sincos function for Macs */
#if defined(__APPLE__) || defined(_MSC_VER)
#define sincos(x,s,c) *(s)=sin(x); *(c)=cos(x);
#else
extern void sincos(double x, double *s, double *c);
#endif

#ifdef _MSC_VER
#define trunc (int)
#endif

char *flam3_variation_names[1+flam3_nvariations] = {
  "linear",
  "sinusoidal",
  "spherical",
  "swirl",
  "horseshoe",
  "polar",
  "handkerchief",
  "heart",
  "disc",
  "spiral",
  "hyperbolic",
  "diamond",
  "ex",
  "julia",
  "bent",
  "waves",
  "fisheye",
  "popcorn",
  "exponential",
  "power",
  "cosine",
  "rings",
  "fan",
  "blob",
  "pdj",
  "fan2",
  "rings2",
  "eyefish",
  "bubble",
  "cylinder",
  "perspective",
  "noise",
  "julian",
  "juliascope",
  "blur",
  "gaussian_blur",
  "radial_blur",
  "pie",
  "ngon",
  "curl",
  "rectangles",
  "arch",
  "tangent",
  "square",
  "rays",
  "blade",
  "secant2",
  "twintrian",
  "cross",
  "disc2",
  "super_shape",
  "flower",
  "conic",
  "parabola",
  "bent2",
  "bipolar",
  "boarders",
  "butterfly",
  "cell",
  "cpow",
  "curve",
  "edisc",
  "elliptic",
  "escher",
  "foci",
  "lazysusan",
  "loonie",
  "pre_blur",
  "modulus",
  "oscilloscope",
  "polar2",
  "popcorn2",
  "scry",
  "separation",
  "split",
  "splits",
  "stripes",
  "wedge",
  "wedge_julia",
  "wedge_sph",
  "whorl",
  "waves2",
  "exp",
  "log",
  "sin",
  "cos",
  "tan",
  "sec",
  "csc",
  "cot",
  "sinh",
  "cosh",
  "tanh",
  "sech",
  "csch",
  "coth",
  "auger",
  "flux",
  "mobius",
  "flatten",
  "pre_zscale",
  "pre_ztranslate",
  "pre_rotate_x",
  "pre_rotate_y",
  "zscale",
  "ztranslate",
  "zcone",
  "post_rotate_x",
  "post_rotate_y",
  "zblur",
  "blur3D",
  "hemisphere",
  "julia3D",
  "julia3Dz",
  "curl3D",
  "blur_circle",
  "blur_zoom",
  "blur_pixelize",
  "bwraps",
  "crop",
  "falloff2",
  "epispiral",
  "pre_spherical",
  "pre_sinusoidal",
  "pre_disc",
  "pre_bwraps",
  "pre_crop",
  "pre_falloff2",
  "post_bwraps",
  "post_curl",
  "post_curl3D",
  "post_crop",
  "post_falloff2",
  "barycentroid",
  "dc_bubble",
  "dc_cube",
  "dc_image",
  "dc_linear",
  "dc_mandelbrot",
  "dc_triangle",
  "dc_ztransl",
  "extrude",
  "falloff",
  "gdoffs",
  "octapol",
  "polynomial",
  "post_dcztransl",
  "post_mirror_z",
  "pre_dcztransl",
  "psphere",
  "sinusgrid",
  "linear2D",
  "circlize",
  "Spherical3D",
  "scry_3D",
  "zeta3D",
  0
};

/*
 * VARIATION FUNCTIONS
 * must be of the form void (void *, double)
 */
/* z passthrough of the 2D variations, added in Apophysis 7X 15C */
#define ZPASS(f, weight) do { if ((f)->zpass) (f)->pz += (weight) * (f)->tz; } while (0)

void var0_linear (flam3_iter_helper *f, double weight) {
   /* linear */
   /* nx = tx;
      ny = ty;
      p[0] += v * nx;
      p[1] += v * ny; */

   f->p0 += weight * f->tx;
   f->p1 += weight * f->ty;
   f->pz += weight * f->tz;
}

void var1_sinusoidal (flam3_iter_helper *f, double weight) {
   /* sinusoidal */
   /* nx = sin(tx);
      ny = sin(ty);
      p[0] += v * nx;
      p[1] += v * ny; */

   f->p0 += weight * sin(f->tx);
   f->p1 += weight * sin(f->ty);
   ZPASS(f, weight);
}

void var2_spherical (flam3_iter_helper *f, double weight) {
   /* spherical */
   /* double r2 = tx * tx + ty * ty + 1e-6;
      nx = tx / r2;
      ny = ty / r2;
      p[0] += v * nx;
      p[1] += v * ny; */

   double r2 = weight / ( f->precalc_sumsq + EPS);

   f->p0 += r2 * f->tx;
   f->p1 += r2 * f->ty;
   ZPASS(f, weight);
}

void var3_swirl (flam3_iter_helper *f, double weight) {
   /* swirl */
   /* double r2 = tx * tx + ty * ty;    /k here is fun
      double c1 = sin(r2);
      double c2 = cos(r2);
      nx = c1 * tx - c2 * ty;
      ny = c2 * tx + c1 * ty;
      p[0] += v * nx;
      p[1] += v * ny; */

   double r2 = f->precalc_sumsq;
   double c1,c2;
   double nx,ny;
   
   sincos(r2,&c1,&c2);
//   double c1 = sin(r2);
//   double c2 = cos(r2);
   nx = c1 * f->tx - c2 * f->ty;
   ny = c2 * f->tx + c1 * f->ty;

   f->p0 += weight * nx;
   f->p1 += weight * ny;
   ZPASS(f, weight);
}

void var4_horseshoe (flam3_iter_helper *f, double weight) {
   /* horseshoe */
   /* a = atan2(tx, ty);
      c1 = sin(a);
      c2 = cos(a);
      nx = c1 * tx - c2 * ty;
      ny = c2 * tx + c1 * ty;
      p[0] += v * nx;
      p[1] += v * ny;  */

   double r = weight / (f->precalc_sqrt + EPS);

   f->p0 += (f->tx - f->ty) * (f->tx + f->ty) * r;
   f->p1 += 2.0 * f->tx * f->ty * r;
   ZPASS(f, weight);
}

void var5_polar (flam3_iter_helper *f, double weight) {
   /* polar */
   /* nx = atan2(tx, ty) / M_PI;
      ny = sqrt(tx * tx + ty * ty) - 1.0;
      p[0] += v * nx;
      p[1] += v * ny; */

   double nx = f->precalc_atan * M_1_PI;
   double ny = f->precalc_sqrt - 1.0;

   f->p0 += weight * nx;
   f->p1 += weight * ny;
   ZPASS(f, weight);
}

void var6_handkerchief (flam3_iter_helper *f, double weight) {
   /* folded handkerchief */
   /* a = atan2(tx, ty);
      r = sqrt(tx*tx + ty*ty);
      p[0] += v * sin(a+r) * r;
      p[1] += v * cos(a-r) * r; */

   double a = f->precalc_atan;
   double r = f->precalc_sqrt;

   f->p0 += weight * r * sin(a+r);
   f->p1 += weight * r * cos(a-r);
}

void var7_heart (flam3_iter_helper *f, double weight) {
   /* heart */
   /* a = atan2(tx, ty);
      r = sqrt(tx*tx + ty*ty);
      a *= r;
      p[0] += v * sin(a) * r;
      p[1] += v * cos(a) * -r; */

   double a = f->precalc_sqrt * f->precalc_atan;
   double ca,sa;
   double r = weight * f->precalc_sqrt;
   
   sincos(a,&sa,&ca);

   f->p0 += r * sa;
   f->p1 += (-r) * ca;
}

void var8_disc (flam3_iter_helper *f, double weight) {
   /* disc */
   /* nx = tx * M_PI;
      ny = ty * M_PI;
      a = atan2(nx, ny);
      r = sqrt(nx*nx + ny*ny);
      p[0] += v * sin(r) * a / M_PI;
      p[1] += v * cos(r) * a / M_PI; */

   double a = f->precalc_atan * M_1_PI;
   double r = M_PI * f->precalc_sqrt;
   double sr,cr;
   sincos(r,&sr,&cr);

   f->p0 += weight * sr * a;
   f->p1 += weight * cr * a;
   ZPASS(f, weight);
}

void var9_spiral (flam3_iter_helper *f, double weight) {
   /* spiral */
   /* a = atan2(tx, ty);
      r = sqrt(tx*tx + ty*ty) + 1e-6;
      p[0] += v * (cos(a) + sin(r)) / r;
      p[1] += v * (sin(a) - cos(r)) / r; */

   double r = f->precalc_sqrt + EPS;
   double r1 = weight/r;
   double sr,cr;
   sincos(r,&sr,&cr);

   f->p0 += r1 * (f->precalc_cosa + sr);
   f->p1 += r1 * (f->precalc_sina - cr);
   ZPASS(f, weight);
}

void var10_hyperbolic (flam3_iter_helper *f, double weight) {
   /* hyperbolic */
   /* a = atan2(tx, ty);
      r = sqrt(tx*tx + ty*ty) + 1e-6;
      p[0] += v * sin(a) / r;
      p[1] += v * cos(a) * r; */

   double r = f->precalc_sqrt + EPS;

   f->p0 += weight * f->precalc_sina / r;
   f->p1 += weight * f->precalc_cosa * r;
   ZPASS(f, weight);
}

void var11_diamond (flam3_iter_helper *f, double weight) {
   /* diamond */
   /* a = atan2(tx, ty);
      r = sqrt(tx*tx + ty*ty);
      p[0] += v * sin(a) * cos(r);
      p[1] += v * cos(a) * sin(r); */

   double r = f->precalc_sqrt;
   double sr,cr;
   sincos(r,&sr,&cr);

   f->p0 += weight * f->precalc_sina * cr;
   f->p1 += weight * f->precalc_cosa * sr;
   ZPASS(f, weight);
}

void var12_ex (flam3_iter_helper *f, double weight) {
   /* ex */
   /* a = atan2(tx, ty);
      r = sqrt(tx*tx + ty*ty);
      n0 = sin(a+r);
      n1 = cos(a-r);
      m0 = n0 * n0 * n0 * r;
      m1 = n1 * n1 * n1 * r;
      p[0] += v * (m0 + m1);
      p[1] += v * (m0 - m1); */

   double a = f->precalc_atan;
   double r = f->precalc_sqrt;

   double n0 = sin(a+r);
   double n1 = cos(a-r);

   double m0 = n0 * n0 * n0 * r;
   double m1 = n1 * n1 * n1 * r;

   f->p0 += weight * (m0 + m1);
   f->p1 += weight * (m0 - m1);
}

void var13_julia (flam3_iter_helper *f, double weight) {
   /* julia */
   /* a = atan2(tx, ty)/2.0;
      if (flam3_random_bit()) a += M_PI;
      r = pow(tx*tx + ty*ty, 0.25);
      nx = r * cos(a);
      ny = r * sin(a);
      p[0] += v * nx;
      p[1] += v * ny; */

   double r;
   double a = 0.5 * f->precalc_atan;
   double sa,ca;

   if (flam3_random_isaac_bit(f->rc)) //(flam3_random_bit())
      a += M_PI;

   r = weight * sqrt(f->precalc_sqrt);
   
   sincos(a,&sa,&ca);

   f->p0 += r * ca;
   f->p1 += r * sa;
}

void var14_bent (flam3_iter_helper *f, double weight) {
   /* bent */
   /* nx = tx;
      ny = ty;
      if (nx < 0.0) nx = nx * 2.0;
      if (ny < 0.0) ny = ny / 2.0;
      p[0] += v * nx;
      p[1] += v * ny; */

   double nx = f->tx;
   double ny = f->ty;

   if (nx < 0.0)
      nx = nx * 2.0;
   if (ny < 0.0)
      ny = ny / 2.0;

   f->p0 += weight * nx;
   f->p1 += weight * ny;
}

void var15_waves (flam3_iter_helper *f, double weight) {
   /* waves */
   /* dx = coef[2][0];
      dy = coef[2][1];
      nx = tx + coef[1][0]*sin(ty/((dx*dx)+EPS));
      ny = ty + coef[1][1]*sin(tx/((dy*dy)+EPS));
      p[0] += v * nx;
      p[1] += v * ny; */

   double c10 = f->xform->c[1][0];
   double c11 = f->xform->c[1][1];

   double nx = f->tx + c10 * sin( f->ty * f->xform->waves_dx2 );
   double ny = f->ty + c11 * sin( f->tx * f->xform->waves_dy2 );

   f->p0 += weight * nx;
   f->p1 += weight * ny;
}

void var16_fisheye (flam3_iter_helper *f, double weight) {
   /* fisheye */
   /* a = atan2(tx, ty);
      r = sqrt(tx*tx + ty*ty);
      r = 2 * r / (r + 1);
      nx = r * cos(a);
      ny = r * sin(a);
      p[0] += v * nx;
      p[1] += v * ny; */

   double r = f->precalc_sqrt;

   r = 2 * weight / (r+1);

   f->p0 += r * f->ty;
   f->p1 += r * f->tx;
}

void var17_popcorn (flam3_iter_helper *f, double weight) {
   /* popcorn */
   /* dx = tan(3*ty);
      dy = tan(3*tx);
      nx = tx + coef[2][0] * sin(dx);
      ny = ty + coef[2][1] * sin(dy);
      p[0] += v * nx;
      p[1] += v * ny; */

   double dx = tan(3*f->ty);
   double dy = tan(3*f->tx);

   double nx = f->tx + f->xform->c[2][0] * sin(dx);
   double ny = f->ty + f->xform->c[2][1] * sin(dy);

   f->p0 += weight * nx;
   f->p1 += weight * ny;
}

void var18_exponential (flam3_iter_helper *f, double weight) {
   /* exponential */
   /* dx = exp(tx-1.0);
      dy = M_PI * ty;
      nx = cos(dy) * dx;
      ny = sin(dy) * dx;
      p[0] += v * nx;
      p[1] += v * ny; */

   double dx = weight * exp(f->tx - 1.0);
   double dy = M_PI * f->ty;
   double sdy,cdy;
   
   sincos(dy,&sdy,&cdy);
   

   f->p0 += dx * cdy;
   f->p1 += dx * sdy;
}

void var19_power (flam3_iter_helper *f, double weight) {
   /* power */
   /* a = atan2(tx, ty);
      sa = sin(a);
      r = sqrt(tx*tx + ty*ty);
      r = pow(r, sa);
      nx = r * precalc_cosa;
      ny = r * sa;
      p[0] += v * nx;
      p[1] += v * ny; */

   double r = weight * pow(f->precalc_sqrt, f->precalc_sina);

   f->p0 += r * f->precalc_cosa;
   f->p1 += r * f->precalc_sina;
}

void var20_cosine (flam3_iter_helper *f, double weight) {
   /* cosine */
   /* nx = cos(tx * M_PI) * cosh(ty);
      ny = -sin(tx * M_PI) * sinh(ty);
      p[0] += v * nx;
      p[1] += v * ny; */

   double a = f->tx * M_PI;
   double sa,ca;
   double nx,ny;
   
   sincos(a,&sa,&ca);
   nx =  ca * cosh(f->ty);
   ny = -sa * sinh(f->ty);

   f->p0 += weight * nx;
   f->p1 += weight * ny;
}

void var21_rings (flam3_iter_helper *f, double weight) {
   /* rings */
   /* dx = coef[2][0];
      dx = dx * dx + EPS;
      r = sqrt(tx*tx + ty*ty);
      r = fmod(r + dx, 2*dx) - dx + r*(1-dx);
      a = atan2(tx, ty);
      nx = cos(a) * r;
      ny = sin(a) * r;
      p[0] += v * nx;
      p[1] += v * ny; */

   double dx = f->xform->c[2][0] * f->xform->c[2][0] + EPS;
   double r = f->precalc_sqrt;
   r = weight * (fmod(r+dx, 2*dx) - dx + r * (1 - dx));

   f->p0 += r * f->precalc_cosa;
   f->p1 += r * f->precalc_sina;
}

void var22_fan (flam3_iter_helper *f, double weight) {
   /* fan */
   /* dx = coef[2][0];
      dy = coef[2][1];
      dx = M_PI * (dx * dx + EPS);
      dx2 = dx/2;
      a = atan(tx,ty);
      r = sqrt(tx*tx + ty*ty);
      a += (fmod(a+dy, dx) > dx2) ? -dx2 : dx2;
      nx = cos(a) * r;
      ny = sin(a) * r;
      p[0] += v * nx;
      p[1] += v * ny; */

   double dx = M_PI * (f->xform->c[2][0] * f->xform->c[2][0] + EPS);
   double dy = f->xform->c[2][1];
   double dx2 = 0.5 * dx;

   double a = f->precalc_atan;
   double r = weight * f->precalc_sqrt;
   double sa,ca;

   a += (fmod(a+dy,dx) > dx2) ? -dx2 : dx2;
   sincos(a,&sa,&ca);

   f->p0 += r * ca;
   f->p1 += r * sa;
}

void var23_blob (flam3_iter_helper *f, double weight) {
   /* blob */
   /* a = atan2(tx, ty);
      r = sqrt(tx*tx + ty*ty);
      r = r * (bloblow + (blobhigh-bloblow) * (0.5 + 0.5 * sin(blobwaves * a)));
      nx = sin(a) * r;
      ny = cos(a) * r;

      p[0] += v * nx;
      p[1] += v * ny; */

   double r = f->precalc_sqrt;
   double a = f->precalc_atan;
   double bdiff = f->xform->blob_high - f->xform->blob_low;

   r = r * (f->xform->blob_low +
            bdiff * (0.5 + 0.5 * sin(f->xform->blob_waves * a)));

   f->p0 += weight * f->precalc_sina * r;
   f->p1 += weight * f->precalc_cosa * r;
}

void var24_pdj (flam3_iter_helper *f, double weight) {
   /* pdj */
   /* nx1 = cos(pdjb * tx);
      nx2 = sin(pdjc * tx);
      ny1 = sin(pdja * ty);
      ny2 = cos(pdjd * ty);

      p[0] += v * (ny1 - nx1);
      p[1] += v * (nx2 - ny2); */

   double nx1 = cos(f->xform->pdj_b * f->tx);
   double nx2 = sin(f->xform->pdj_c * f->tx);
   double ny1 = sin(f->xform->pdj_a * f->ty);
   double ny2 = cos(f->xform->pdj_d * f->ty);

   f->p0 += weight * (ny1 - nx1);
   f->p1 += weight * (nx2 - ny2);
   ZPASS(f, weight);
}

void var25_fan2 (flam3_iter_helper *f, double weight) {
   /* fan2 */
   /* a = precalc_atan;
      r = precalc_sqrt;

      dy = fan2y;
      dx = M_PI * (fan2x * fan2x + EPS);
      dx2 = dx / 2.0;

      t = a + dy - dx * (int)((a + dy)/dx);

      if (t > dx2)
         a = a - dx2;
      else
         a = a + dx2;

      nx = sin(a) * r;
      ny = cos(a) * r;

      p[0] += v * nx;
      p[1] += v * ny; */

   double dy = f->xform->fan2_y;
   double dx = M_PI * (f->xform->fan2_x * f->xform->fan2_x + EPS);
   double dx2 = 0.5 * dx;
   double a = f->precalc_atan;
   double sa,ca;
   double r = weight * f->precalc_sqrt;

   double t = a + dy - dx * (int)((a + dy)/dx);

   if (t>dx2)
      a = a-dx2;
   else
      a = a+dx2;
      
   sincos(a,&sa,&ca);

   f->p0 += r * sa;
   f->p1 += r * ca;
   ZPASS(f, weight);
}

void var26_rings2 (flam3_iter_helper *f, double weight) {
   /* rings2 */
   /* r = precalc_sqrt;
      dx = rings2val * rings2val + EPS;
      r += dx - 2.0*dx*(int)((r + dx)/(2.0 * dx)) - dx + r * (1.0-dx);
      nx = precalc_sina * r;
      ny = precalc_cosa * r;
      p[0] += v * nx;
      p[1] += v * ny; */

   double r = f->precalc_sqrt;
   double dx = f->xform->rings2_val * f->xform->rings2_val + EPS;

   r += -2.0*dx*(int)((r+dx)/(2.0*dx)) + r * (1.0-dx);

   f->p0 += weight * f->precalc_sina * r;
   f->p1 += weight * f->precalc_cosa * r;
   ZPASS(f, weight);
}

void var27_eyefish (flam3_iter_helper *f, double weight) {
   /* eyefish */
   /* r = 2.0 * v / (precalc_sqrt + 1.0);
      p[0] += r*tx;
      p[1] += r*ty; */

   double r = (weight * 2.0) / (f->precalc_sqrt + 1.0);

   f->p0 += r * f->tx;
   f->p1 += r * f->ty;
   ZPASS(f, weight);
}

void var28_bubble (flam3_iter_helper *f, double weight) {
   /* bubble */

   double r = weight / (0.25 * (f->precalc_sumsq) + 1);

  f->p0 += r * f->tx;
  f->p1 += r * f->ty;
  f->pz += weight * (2.0 / (0.25 * (f->precalc_sumsq) + 1) - 1.0);
}

void var29_cylinder (flam3_iter_helper *f, double weight) {
   /* cylinder (01/06) */

   f->p0 += weight * sin(f->tx);
   f->p1 += weight * f->ty;
   f->pz += weight * cos(f->tx);
}

void var30_perspective (flam3_iter_helper *f, double weight) {
   /* perspective (01/06) */

   double t = 1.0 / (f->xform->perspective_dist - f->ty * f->xform->persp_vsin);

   f->p0 += weight * f->xform->perspective_dist * f->tx * t;
   f->p1 += weight * f->xform->persp_vfcos * f->ty * t;
}

void var31_noise (flam3_iter_helper *f, double weight) {
   /* noise (03/06) */

   double tmpr, sinr, cosr, r;

   tmpr = flam3_random_isaac_01(f->rc) * 2 * M_PI;
   sincos(tmpr,&sinr,&cosr);

   r = weight * flam3_random_isaac_01(f->rc);

   f->p0 += f->tx * r * cosr;
   f->p1 += f->ty * r * sinr;
   ZPASS(f, weight);
}

void var32_juliaN_generic (flam3_iter_helper *f, double weight) {
   /* juliaN (03/06) */

   int t_rnd = trunc((f->xform->julian_rN)*flam3_random_isaac_01(f->rc));
   
   double tmpr = (f->precalc_atanyx + 2 * M_PI * t_rnd) / f->xform->julian_power;

   double r = weight * pow(f->precalc_sumsq, f->xform->julian_cn);
   double sina, cosa;
   sincos(tmpr,&sina,&cosa);

   f->p0 += r * cosa;
   f->p1 += r * sina;
   ZPASS(f, weight);
}

void var33_juliaScope_generic (flam3_iter_helper *f, double weight) {
   /* juliaScope (03/06) */

   int t_rnd = trunc((f->xform->juliascope_rN) * flam3_random_isaac_01(f->rc));

   double tmpr, r;
   double sina, cosa;

   if ((t_rnd & 1) == 0)
      tmpr = (2 * M_PI * t_rnd + f->precalc_atanyx) / f->xform->juliascope_power;
   else
      tmpr = (2 * M_PI * t_rnd - f->precalc_atanyx) / f->xform->juliascope_power;

   sincos(tmpr,&sina,&cosa);

   r = weight * pow(f->precalc_sumsq, f->xform->juliascope_cn);

   f->p0 += r * cosa;
   f->p1 += r * sina;
   ZPASS(f, weight);
}

void var34_blur (flam3_iter_helper *f, double weight) {
   /* blur (03/06) */

   double tmpr, sinr, cosr, r;

   tmpr = flam3_random_isaac_01(f->rc) * 2 * M_PI;
   sincos(tmpr,&sinr,&cosr);

   r = weight * flam3_random_isaac_01(f->rc);

   f->p0 += r * cosr;
   f->p1 += r * sinr;
   ZPASS(f, weight);
}

void var35_gaussian (flam3_iter_helper *f, double weight) {
   /* gaussian (09/06) */

   double ang, r, sina, cosa;

   ang = flam3_random_isaac_01(f->rc) * 2 * M_PI;
   sincos(ang,&sina,&cosa);

   r = weight * ( flam3_random_isaac_01(f->rc) + flam3_random_isaac_01(f->rc)
                   + flam3_random_isaac_01(f->rc) + flam3_random_isaac_01(f->rc) - 2.0 );

   f->p0 += r * cosa;
   f->p1 += r * sina;
   ZPASS(f, weight);
}

void var36_radial_blur (flam3_iter_helper *f, double weight) {
   /* radial blur (09/06) */
   /* removed random storage 6/07 */

   double rndG, ra, rz, tmpa, sa, ca;

   /* Get pseudo-gaussian */
   rndG = weight * (flam3_random_isaac_01(f->rc) + flam3_random_isaac_01(f->rc)
                   + flam3_random_isaac_01(f->rc) + flam3_random_isaac_01(f->rc) - 2.0);

   /* Calculate angle & zoom */
   ra = f->precalc_sqrt;
   tmpa = f->precalc_atanyx + f->xform->radialBlur_spinvar*rndG;
   sincos(tmpa,&sa,&ca);
   rz = f->xform->radialBlur_zoomvar * rndG - 1;

   f->p0 += ra * ca + rz * f->tx;
   f->p1 += ra * sa + rz * f->ty;
   ZPASS(f, weight);
}

void var37_pie(flam3_iter_helper *f, double weight) {
   /* pie by Joel Faber (June 2006) */

   double a, r, sa, ca;
   int sl;

   sl = (int) (flam3_random_isaac_01(f->rc) * f->xform->pie_slices + 0.5);
   a = f->xform->pie_rotation +
       2.0 * M_PI * (sl + flam3_random_isaac_01(f->rc) * f->xform->pie_thickness) / f->xform->pie_slices;
   r = weight * flam3_random_isaac_01(f->rc);
   sincos(a,&sa,&ca);

   f->p0 += r * ca;
   f->p1 += r * sa;
}

void var38_ngon(flam3_iter_helper *f, double weight) {
   /* ngon by Joel Faber (09/06) */

   double r_factor,theta,phi,b, amp;

   r_factor = pow(f->precalc_sumsq, f->xform->ngon_power/2.0);

   theta = f->precalc_atanyx;
   b = 2*M_PI/f->xform->ngon_sides;

   phi = theta - (b*floor(theta/b));
   if (phi > b/2)
      phi -= b;

   amp = f->xform->ngon_corners * (1.0 / (cos(phi) + EPS) - 1.0) + f->xform->ngon_circle;
   amp /= (r_factor + EPS);

   f->p0 += weight * f->tx * amp;
   f->p1 += weight * f->ty * amp;
   ZPASS(f, weight);
}

void var39_curl(flam3_iter_helper *f, double weight)
{
    double re = 1.0 + f->xform->curl_c1 * f->tx + f->xform->curl_c2 * (f->tx * f->tx - f->ty * f->ty);
    double im = f->xform->curl_c1 * f->ty + 2.0 * f->xform->curl_c2 * f->tx * f->ty;

    double r = weight / (re*re + im*im);

    f->p0 += (f->tx * re + f->ty * im) * r;
    f->p1 += (f->ty * re - f->tx * im) * r;
    ZPASS(f, weight);
}

void var40_rectangles(flam3_iter_helper *f, double weight)
{
    if (f->xform->rectangles_x==0)
       f->p0 += weight * f->tx;
    else
       f->p0 += weight * ((2 * floor(f->tx / f->xform->rectangles_x) + 1) * f->xform->rectangles_x - f->tx);

    if (f->xform->rectangles_y==0)
       f->p1 += weight * f->ty;
    else
       f->p1 += weight * ((2 * floor(f->ty / f->xform->rectangles_y) + 1) * f->xform->rectangles_y - f->ty);
    ZPASS(f, weight);

}

void var41_arch(flam3_iter_helper *f, double weight)
{
   /* Z+ variation Jan 07
   procedure TXForm.Arch;
   var
     sinr, cosr: double;
   begin
     SinCos(random * vars[29]*pi, sinr, cosr);
     FPx := FPx + sinr*vars[29];
     FPy := FPy + sqr(sinr)/cosr*vars[29];
   end;
   */
   
   /*
    * !!! Note !!!
    * This code uses the variation weight in a non-standard fashion, and
    * it may change or even be removed in future versions of flam3.
    */

   double ang = flam3_random_isaac_01(f->rc) * weight * M_PI;
   double sinr,cosr;
   sincos(ang,&sinr,&cosr);

   f->p0 += weight * sinr;
   f->p1 += weight * (sinr*sinr)/cosr;

}

void var42_tangent(flam3_iter_helper *f, double weight)
{
   /* Z+ variation Jan 07
   procedure TXForm.Tangent;
   begin
     FPx := FPx + vars[30] * (sin(FTx)/cos(FTy));
     FPy := FPy + vars[30] * (sin(FTy)/cos(FTy));
   end;
   */

   f->p0 += weight * sin(f->tx)/cos(f->ty);
   f->p1 += weight * tan(f->ty);

}

void var43_square(flam3_iter_helper *f, double weight)
{
   /* Z+ variation Jan 07
   procedure TXForm.SquareBlur;
   begin
     FPx := FPx + vars[31] * (random - 0.5);
     FPy := FPy + vars[31] * (random - 0.5);
   end;
   */

   f->p0 += weight * (flam3_random_isaac_01(f->rc) - 0.5);
   f->p1 += weight * (flam3_random_isaac_01(f->rc) - 0.5);

}

void var44_rays(flam3_iter_helper *f, double weight)
{
   /* Z+ variation Jan 07
   procedure TXForm.Rays;
   var
     r, sinr, cosr, tgr: double;
   begin
     SinCos(random * vars[32]*pi, sinr, cosr);
     r := vars[32] / (sqr(FTx) + sqr(FTy) + EPS);
     tgr := sinr/cosr;
     FPx := FPx + tgr * (cos(FTx)*vars[32]) * r;
     FPy := FPy + tgr * (sin(FTy)*vars[32]) * r;
   end;
   */

   /*
    * !!! Note !!!
    * This code uses the variation weight in a non-standard fashion, and
    * it may change or even be removed in future versions of flam3.
    */

   double ang = weight * flam3_random_isaac_01(f->rc) * M_PI;
   double r = weight / (f->precalc_sumsq + EPS);
   double tanr = weight * tan(ang) * r;


   f->p0 += tanr * cos(f->tx);
   f->p1 += tanr * sin(f->ty);

}

void var45_blade(flam3_iter_helper *f, double weight)
{
   /* Z+ variation Jan 07
   procedure TXForm.Blade;
   var
     r, sinr, cosr: double;
   begin
     r := sqrt(sqr(FTx) + sqr(FTy))*vars[33];
     SinCos(r*random, sinr, cosr);
     FPx := FPx + vars[33] * FTx * (cosr + sinr);
     FPy := FPy + vars[33] * FTx * (cosr - sinr);
   end;
   */

   /*
    * !!! Note !!!
    * This code uses the variation weight in a non-standard fashion, and
    * it may change or even be removed in future versions of flam3.
    */

   double r = flam3_random_isaac_01(f->rc) * weight * f->precalc_sqrt;
   double sinr,cosr;
   
   sincos(r,&sinr,&cosr);

   f->p0 += weight * f->tx * (cosr + sinr);
   f->p1 += weight * f->tx * (cosr - sinr);

}

void var46_secant2(flam3_iter_helper *f, double weight)
{
   /* Intended as a 'fixed' version of secant */

   /*
    * !!! Note !!!
    * This code uses the variation weight in a non-standard fashion, and
    * it may change or even be removed in future versions of flam3.
    */

   double r = weight * f->precalc_sqrt;
   double cr = cos(r);
   double icr = 1.0/cr;

   f->p0 += weight * f->tx;
   
   if (cr<0)
      f->p1 += weight*(icr + 1);
   else
      f->p1 += weight*(icr - 1);
}

void var47_twintrian(flam3_iter_helper *f, double weight)
{
   /* Z+ variation Jan 07
   procedure TXForm.TwinTrian;
   var
     r, diff, sinr, cosr: double;
   begin
     r := sqrt(sqr(FTx) + sqr(FTy))*vars[35];
     SinCos(r*random, sinr, cosr);
     diff := Math.Log10(sinr*sinr)+cosr;
     FPx := FPx + vars[35] * FTx * diff;
     FPy := FPy + vars[35] * FTx * (diff - (sinr*pi));
   end;
   */

   /*
    * !!! Note !!!
    * This code uses the variation weight in a non-standard fashion, and
    * it may change or even be removed in future versions of flam3.
    */

   double r = flam3_random_isaac_01(f->rc) * weight * f->precalc_sqrt;
   double sinr,cosr,diff;
   
   sincos(r,&sinr,&cosr);
   diff = log10(sinr*sinr)+cosr;
   
   if (badvalue(diff))
      diff = -30.0;      

   f->p0 += weight * f->tx * diff;
   f->p1 += weight * f->tx * (diff - sinr*M_PI);

}

void var48_cross(flam3_iter_helper *f, double weight)
{
   /* Z+ variation Jan 07
   procedure TXForm.Cross;
   var
     r: double;
   begin
     r := vars[36]*sqrt(1/(sqr(sqr(FTx)-sqr(FTy))+EPS));
     FPx := FPx + FTx * r;
     FPy := FPy + FTy * r;
   end;
   */

   double s = f->tx*f->tx - f->ty*f->ty;
   double r = weight * sqrt(1.0 / (s*s+EPS));

   f->p0 += f->tx * r;
   f->p1 += f->ty * r;
   ZPASS(f, weight);

}

void var49_disc2(flam3_iter_helper *f, double weight)
{
   /* Z+ variation Jan 07
   c := vvar/PI;
   k := rot*PI;
     sinadd := Sin(add);
     cosadd := Cos(add);
   cosadd := cosadd - 1;
   if (add > 2*PI) then begin
     cosadd := cosadd * (1 + add - 2*PI);
     sinadd := sinadd * (1 + add - 2*PI)
   end
   else if (add < -2*PI) then begin
     cosadd := cosadd * (1 + add + 2*PI);
     sinadd := sinadd * (1 + add + 2*PI)
   end
   end;
   procedure TVariationDisc2.CalcFunction;
   var
     r, sinr, cosr: extended;
   begin
     SinCos(k * (FTx^+FTy^), sinr, cosr);   //rot*PI
     r := c * arctan2(FTx^, FTy^); //vvar/PI
     FPx^ := FPx^ + (sinr + cosadd) * r;
     FPy^ := FPy^ + (cosr + sinadd) * r;
   */

   double r,t,sinr, cosr;

   t = f->xform->disc2_timespi * (f->tx + f->ty);
   sincos(t,&sinr,&cosr);
   r = weight * f->precalc_atan / M_PI;

   f->p0 += (sinr + f->xform->disc2_cosadd) * r;
   f->p1 += (cosr + f->xform->disc2_sinadd) * r;

}

void var50_supershape(flam3_iter_helper *f, double weight) {

   double theta;
   double t1,t2,r;
   double st,ct;
   double myrnd;

   theta = f->xform->super_shape_pm_4 * f->precalc_atanyx + M_PI_4;
   
   sincos(theta,&st,&ct);

   t1 = fabs(ct);
   t1 = pow(t1,f->xform->super_shape_n2);

   t2 = fabs(st);
   t2 = pow(t2,f->xform->super_shape_n3);
   
   myrnd = f->xform->super_shape_rnd;

   r = weight * ( (myrnd*flam3_random_isaac_01(f->rc) + (1.0-myrnd)*f->precalc_sqrt) - f->xform->super_shape_holes) 
      * pow(t1+t2,f->xform->super_shape_pneg1_n1) / f->precalc_sqrt;

   f->p0 += r * f->tx;
   f->p1 += r * f->ty;
}

void var51_flower(flam3_iter_helper *f, double weight) {
    /* cyberxaos, 4/2007 */
    /*   theta := arctan2(FTy^, FTx^);
         r := (random-holes)*cos(petals*theta);
         FPx^ := FPx^ + vvar*r*cos(theta);
         FPy^ := FPy^ + vvar*r*sin(theta);*/
 
    double theta = f->precalc_atanyx;
    double r = weight * (flam3_random_isaac_01(f->rc) - f->xform->flower_holes) * 
                    cos(f->xform->flower_petals*theta) / f->precalc_sqrt;

    f->p0 += r * f->tx;
    f->p1 += r * f->ty;
}
    
void var52_conic(flam3_iter_helper *f, double weight) {
    /* cyberxaos, 4/2007 */
    /*   theta := arctan2(FTy^, FTx^);
         r :=  (random - holes)*((eccentricity)/(1+eccentricity*cos(theta)));
         FPx^ := FPx^ + vvar*r*cos(theta);
         FPy^ := FPy^ + vvar*r*sin(theta); */
 
    double ct = f->tx / f->precalc_sqrt;
    double r = weight * (flam3_random_isaac_01(f->rc) - f->xform->conic_holes) * 
                    f->xform->conic_eccentricity / (1 + f->xform->conic_eccentricity*ct) / f->precalc_sqrt;

    f->p0 += r * f->tx;
    f->p1 += r * f->ty;
}

void var53_parabola(flam3_iter_helper *f, double weight) {
    /* cyberxaos, 4/2007 */
    /*   r := sqrt(sqr(FTx^) + sqr(FTy^));
         FPx^ := FPx^ + parabola_height*vvar*sin(r)*sin(r)*random;  
         FPy^ := FPy^ + parabola_width*vvar*cos(r)*random; */
 
    double r = f->precalc_sqrt;
    double sr,cr;
    
    sincos(r,&sr,&cr);
    
    f->p0 += f->xform->parabola_height * weight * sr*sr * flam3_random_isaac_01(f->rc);
    f->p1 += f->xform->parabola_width * weight * cr * flam3_random_isaac_01(f->rc);
    
}      

void var54_bent2 (flam3_iter_helper *f, double weight) {

   /* Bent2 in the Apophysis Plugin Pack */   
   
   double nx = f->tx;
   double ny = f->ty;

   if (nx < 0.0)
      nx = nx * f->xform->bent2_x;
   if (ny < 0.0)
      ny = ny * f->xform->bent2_y;

   f->p0 += weight * nx;
   f->p1 += weight * ny;
}

void var55_bipolar (flam3_iter_helper *f, double weight) {

   /* Bipolar in the Apophysis Plugin Pack */   
   
   double x2y2 = f->precalc_sumsq;
   double t = x2y2+1;
   double x2 = 2*f->tx;
   double ps = -M_PI_2 * f->xform->bipolar_shift;
   double y = 0.5 * atan2(2.0 * f->ty, x2y2 - 1.0) + ps;
   
   if (y > M_PI_2)
       y = -M_PI_2 + fmod(y + M_PI_2, M_PI);
   else if (y < -M_PI_2)
       y = M_PI_2 - fmod(M_PI_2 - y, M_PI);

   f->p0 += weight * 0.25 * M_2_PI * log ( (t+x2) / (t-x2) );
   f->p1 += weight * M_2_PI * y;
   ZPASS(f, weight);
}

void var56_boarders (flam3_iter_helper *f, double weight) {

   /* Boarders in the Apophysis Plugin Pack */   
   
   double roundX, roundY, offsetX, offsetY;
    
   roundX = rint(f->tx);
   roundY = rint(f->ty);
   offsetX = f->tx - roundX;
   offsetY = f->ty - roundY;
    
   if (flam3_random_isaac_01(f->rc) >= 0.75) {
      f->p0 += weight*(offsetX*0.5 + roundX);
      f->p1 += weight*(offsetY*0.5 + roundY);
   } else {
      
      if (fabs(offsetX) >= fabs(offsetY)) {
         
         if (offsetX >= 0.0) {
            f->p0 += weight*(offsetX*0.5 + roundX + 0.25);
            f->p1 += weight*(offsetY*0.5 + roundY + 0.25 * offsetY / offsetX);
         } else {
            f->p0 += weight*(offsetX*0.5 + roundX - 0.25);
            f->p1 += weight*(offsetY*0.5 + roundY - 0.25 * offsetY / offsetX);  
         }
         
      } else {
         
         if (offsetY >= 0.0) {
            f->p1 += weight*(offsetY*0.5 + roundY + 0.25);
            f->p0 += weight*(offsetX*0.5 + roundX + offsetX/offsetY*0.25);
         } else {
            f->p1 += weight*(offsetY*0.5 + roundY - 0.25);
            f->p0 += weight*(offsetX*0.5 + roundX - offsetX/offsetY*0.25);
         }
      }
   }
}

void var57_butterfly (flam3_iter_helper *f, double weight) {

   /* Butterfly in the Apophysis Plugin Pack */   
   
   /* wx is weight*4/sqrt(3*pi) */
   double wx = weight*1.3029400317411197908970256609023;
   
   double y2 = f->ty*2.0;
   double r = wx*sqrt(fabs(f->ty * f->tx)/(EPS + f->tx*f->tx + y2*y2));
   
   f->p0 += r * f->tx;
   f->p1 += r * y2;
   
}

void var58_cell (flam3_iter_helper *f, double weight) {

   /* Cell in the Apophysis Plugin Pack */   

   double inv_cell_size = 1.0/f->xform->cell_size;
    
   /* calculate input cell */
   int x = floor(f->tx*inv_cell_size);
   int y = floor(f->ty*inv_cell_size);

   /* Offset from cell origin */
   double dx = f->tx - x*f->xform->cell_size;
   double dy = f->ty - y*f->xform->cell_size;
   
   /* interleave cells */
   if (y >= 0) {
      if (x >= 0) {
         y *= 2;
         x *= 2;
      } else {
         y *= 2;
         x = -(2*x+1);
      }
   } else {
      if (x >= 0) {
         y = -(2*y+1);
         x *= 2;
      } else {
         y = -(2*y+1);
         x = -(2*x+1);
      }
   }
   
   f->p0 += weight * (dx + x*f->xform->cell_size);
   f->p1 -= weight * (dy + y*f->xform->cell_size);
   
}

void var59_cpow (flam3_iter_helper *f, double weight) {

   /* Cpow in the Apophysis Plugin Pack */   

   double a = f->precalc_atanyx;
   double lnr = 0.5 * log(f->precalc_sumsq);
   double va = 2.0 * M_PI / f->xform->cpow_power;
   double vc = f->xform->cpow_r / f->xform->cpow_power;
   double vd = f->xform->cpow_i / f->xform->cpow_power;
   double ang = vc*a + vd*lnr + va*floor(f->xform->cpow_power*flam3_random_isaac_01(f->rc));
   double sa,ca;
   
   double m = weight * exp(vc * lnr - vd * a);
   
   sincos(ang,&sa,&ca);
   
   f->p0 += m * ca;
   f->p1 += m * sa;
   
}

void var60_curve (flam3_iter_helper *f, double weight) {

   /* Curve in the Apophysis Plugin Pack */   
   
   double pc_xlen = f->xform->curve_xlength*f->xform->curve_xlength;
   double pc_ylen = f->xform->curve_ylength*f->xform->curve_ylength;
   
   if (pc_xlen<1E-20) pc_xlen = 1E-20;
   
   if (pc_ylen<1E-20) pc_ylen = 1E-20;

   f->p0 += weight * (f->tx + f->xform->curve_xamp * exp(-f->ty*f->ty/pc_xlen));
   f->p1 += weight * (f->ty + f->xform->curve_yamp * exp(-f->tx*f->tx/pc_ylen));
      
}

void var61_edisc (flam3_iter_helper *f, double weight) {

   /* Edisc in the Apophysis Plugin Pack */   
   
   double tmp = f->precalc_sumsq + 1.0;
   double tmp2 = 2.0 * f->tx;
   double r1 = sqrt(tmp+tmp2);
   double r2 = sqrt(tmp-tmp2);
   double xmax = (r1+r2) * 0.5;
   double a1 = log(xmax + sqrt(xmax - 1.0));
   double a2 = -acos(f->tx/xmax);
   double w = weight / 11.57034632;
   double snv,csv,snhu,cshu;
   
   sincos(a1,&snv,&csv);
   
   snhu = sinh(a2);
   cshu = cosh(a2);
   
   if (f->ty > 0.0) snv = -snv;
   
   f->p0 += w * cshu * csv;
   f->p1 += w * snhu * snv;
   
}

void var62_elliptic (flam3_iter_helper *f, double weight) {

   /* Elliptic in the Apophysis Plugin Pack */

   double tmp = f->precalc_sumsq + 1.0;
   double x2 = 2.0 * f->tx;
   double xmax = 0.5 * (sqrt(tmp+x2) + sqrt(tmp-x2));
   double a = f->tx / xmax;
   double b = 1.0 - a*a;
   double ssx = xmax - 1.0;
   double w = weight / M_PI_2;
   
   if (b<0)
      b = 0;
   else
      b = sqrt(b);
      
   if (ssx<0)
      ssx = 0;
   else
      ssx = sqrt(ssx);
      
   f->p0 += w * atan2(a,b);
   
   if (f->ty > 0)
      f->p1 += w * log(xmax + ssx);
   else
      f->p1 -= w * log(xmax + ssx);
   ZPASS(f, weight);
      
}

void var63_escher (flam3_iter_helper *f, double weight) {

   /* Escher in the Apophysis Plugin Pack */
   
   double seb,ceb;
   double vc,vd;
   double m,n;
   double sn,cn;

   double a = f->precalc_atanyx;
   double lnr = 0.5 * log(f->precalc_sumsq);

   sincos(f->xform->escher_beta,&seb,&ceb);
   
   vc = 0.5 * (1.0 + ceb);
   vd = 0.5 * seb;

   m = weight * exp(vc*lnr - vd*a);
   n = vc*a + vd*lnr;
   
   sincos(n,&sn,&cn);
   
   f->p0 += m * cn;
   f->p1 += m * sn;
   ZPASS(f, weight);
      
}

void var64_foci (flam3_iter_helper *f, double weight) {

   /* Foci in the Apophysis Plugin Pack */

   double expx = exp(f->tx) * 0.5;
   double expnx = 0.25 / expx;
   double sn,cn,tmp;
   
   sincos(f->ty,&sn,&cn);
   tmp = weight/(expx + expnx - cn);
   
   f->p0 += tmp * (expx - expnx);
   f->p1 += tmp * sn;
   ZPASS(f, weight);
      
}

void var65_lazysusan (flam3_iter_helper *f, double weight) {

   /* Lazysusan in the Apophysis Plugin Pack */
   
   double x = f->tx - f->xform->lazysusan_x;
   double y = f->ty + f->xform->lazysusan_y;
   double r = sqrt(x*x + y*y);
   double sina, cosa;
   
   if (r<weight) {
      double a = atan2(y,x) + f->xform->lazysusan_spin +
                 f->xform->lazysusan_twist*(weight-r);
      sincos(a,&sina,&cosa);
      r = weight * r;
      
      f->p0 += r*cosa + f->xform->lazysusan_x;
      f->p1 += r*sina - f->xform->lazysusan_y;
   } else {
      
      r = weight * (1.0 + f->xform->lazysusan_space / r);
      
      f->p0 += r*x + f->xform->lazysusan_x;
      f->p1 += r*y - f->xform->lazysusan_y;
   
   }
   ZPASS(f, weight);
      
}

void var66_loonie (flam3_iter_helper *f, double weight) {

   /* Loonie in the Apophysis Plugin Pack */

   /*
    * !!! Note !!!
    * This code uses the variation weight in a non-standard fashion, and
    * it may change or even be removed in future versions of flam3.
    */
   
   double r2 = f->precalc_sumsq;
   double w2 = weight*weight;
   
   if (r2 < w2) {
      double r = weight * sqrt(w2/r2 - 1.0);
      f->p0 += r * f->tx;
      f->p1 += r * f->ty;
   } else {
      f->p0 += weight * f->tx;
      f->p1 += weight * f->ty;
   }
   ZPASS(f, weight);
         
}

void var67_pre_blur (flam3_iter_helper *f, double weight) {

   /* pre-xform: PreBlur (Apo 2.08) */
   
   /* Get pseudo-gaussian */
   double rndG = weight * (flam3_random_isaac_01(f->rc) + flam3_random_isaac_01(f->rc)
                   + flam3_random_isaac_01(f->rc) + flam3_random_isaac_01(f->rc) - 2.0);
   double rndA = flam3_random_isaac_01(f->rc) * 2.0 * M_PI;
   double sinA,cosA;
   
   sincos(rndA,&sinA,&cosA);
   
   /* Note: original coordinate changed */
   f->tx += rndG * cosA;
   f->ty += rndG * sinA;
         
}

void var68_modulus (flam3_iter_helper *f, double weight) {

   /* Modulus in the Apophysis Plugin Pack */
   
   double xr = 2*f->xform->modulus_x;
   double yr = 2*f->xform->modulus_y;
   
   if (f->tx > f->xform->modulus_x)
      f->p0 += weight * (-f->xform->modulus_x + fmod(f->tx + f->xform->modulus_x, xr));
   else if (f->tx < -f->xform->modulus_x)
      f->p0 += weight * ( f->xform->modulus_x - fmod(f->xform->modulus_x - f->tx, xr));
   else
      f->p0 += weight * f->tx;
      
   if (f->ty > f->xform->modulus_y)
      f->p1 += weight * (-f->xform->modulus_y + fmod(f->ty + f->xform->modulus_y, yr));
   else if (f->ty < -f->xform->modulus_y)
      f->p1 += weight * ( f->xform->modulus_y - fmod(f->xform->modulus_y - f->ty, yr));
   else
      f->p1 += weight * f->ty;
         
}

void var69_oscope (flam3_iter_helper *f, double weight) {

   /* oscilloscope from the apophysis plugin pack */
   
   double tpf = 2 * M_PI * f->xform->oscope_frequency;
   double t;
   
   if (f->xform->oscope_damping == 0.0)
      t = f->xform->oscope_amplitude * cos(tpf*f->tx) + f->xform->oscope_separation;
   else {
      t = f->xform->oscope_amplitude * exp(-fabs(f->tx)*f->xform->oscope_damping)
          * cos(tpf*f->tx) + f->xform->oscope_separation;
   }
   
   if (fabs(f->ty) <= t) {
      f->p0 += weight*f->tx;
      f->p1 -= weight*f->ty;
   } else {
      f->p0 += weight*f->tx;
      f->p1 += weight*f->ty;
   } 
}

void var70_polar2 (flam3_iter_helper *f, double weight) {

   /* polar2 from the apophysis plugin pack */
   
   double p2v = weight / M_PI;
   
   f->p0 += p2v * f->precalc_atan;
   f->p1 += p2v/2.0 * log(f->precalc_sumsq);
   ZPASS(f, weight);
}

void var71_popcorn2 (flam3_iter_helper *f, double weight) {

   /* popcorn2 from the apophysis plugin pack */
   
   f->p0 += weight * ( f->tx + f->xform->popcorn2_x * sin(tan(f->ty*f->xform->popcorn2_c)));
   f->p1 += weight * ( f->ty + f->xform->popcorn2_y * sin(tan(f->tx*f->xform->popcorn2_c)));

}

void var72_scry (flam3_iter_helper *f, double weight) {

   /* scry from the apophysis plugin pack */
   /* note that scry does not multiply by weight, but as the */
   /* values still approach 0 as the weight approaches 0, it */
   /* should be ok                                           */ 

   /*
    * !!! Note !!!
    * This code uses the variation weight in a non-standard fashion, and
    * it may change or even be removed in future versions of flam3.
    */
   
   double t = f->precalc_sumsq;
   double r = 1.0 / (f->precalc_sqrt * (t + 1.0/(weight+EPS)));
   
   f->p0 += f->tx * r;
   f->p1 += f->ty * r;
   ZPASS(f, weight);

}

void var73_separation (flam3_iter_helper *f, double weight) {

   /* separation from the apophysis plugin pack */

   double sx2 = f->xform->separation_x * f->xform->separation_x;
   double sy2 = f->xform->separation_y * f->xform->separation_y;
   
   if (f->tx > 0.0)
      f->p0 += weight * (sqrt(f->tx*f->tx + sx2)- f->tx*f->xform->separation_xinside);
   else
      f->p0 -= weight * (sqrt(f->tx*f->tx + sx2)+ f->tx*f->xform->separation_xinside);
   
   if (f->ty > 0.0)
      f->p1 += weight * (sqrt(f->ty*f->ty + sy2)- f->ty*f->xform->separation_yinside);
   else
      f->p1 -= weight * (sqrt(f->ty*f->ty + sy2)+ f->ty*f->xform->separation_yinside);
   ZPASS(f, weight);
   
}

void var74_split (flam3_iter_helper *f, double weight) {
   
   /* Split from apo plugins pack */

   if (cos(f->tx*f->xform->split_xsize*M_PI) >= 0)
      f->p1 += weight*f->ty;
   else
      f->p1 -= weight*f->ty;
      
   if (cos(f->ty*f->xform->split_ysize*M_PI) >= 0)
      f->p0 += weight * f->tx;
   else
      f->p0 -= weight * f->tx;

}

void var75_splits (flam3_iter_helper *f, double weight) {
   
   /* Splits from apo plugins pack */

   if (f->tx >= 0)
      f->p0 += weight*(f->tx+f->xform->splits_x);
   else
      f->p0 += weight*(f->tx-f->xform->splits_x);
      
   if (f->ty >= 0)
      f->p1 += weight*(f->ty+f->xform->splits_y);
   else
      f->p1 += weight*(f->ty-f->xform->splits_y);
   ZPASS(f, weight);

}

void var76_stripes (flam3_iter_helper *f, double weight) {
   
   /* Stripes from apo plugins pack */

   double roundx,offsetx;
   
   roundx = floor(f->tx + 0.5);
   offsetx = f->tx - roundx;

   f->p0 += weight * (offsetx*(1.0-f->xform->stripes_space)+roundx);
   f->p1 += weight * (f->ty + offsetx*offsetx*f->xform->stripes_warp);

}

void var77_wedge (flam3_iter_helper *f, double weight) {
   
   /* Wedge from apo plugins pack */

   double r = f->precalc_sqrt;
   double a = f->precalc_atanyx + f->xform->wedge_swirl * r;
   double c = floor( (f->xform->wedge_count * a + M_PI)*M_1_PI*0.5);
   
   double comp_fac = 1 - f->xform->wedge_angle*f->xform->wedge_count*M_1_PI*0.5;
   double sa, ca;
   
   a = a * comp_fac + c * f->xform->wedge_angle;
   
   sincos(a,&sa,&ca);

   r = weight * (r + f->xform->wedge_hole);
   
   f->p0 += r*ca;
   f->p1 += r*sa;
   ZPASS(f, weight);

}

void var78_wedge_julia (flam3_iter_helper *f, double weight) {

   /* wedge_julia from apo plugin pack */

   double r = weight * pow(f->precalc_sumsq, f->xform->wedgeJulia_cn);
   int t_rnd = (int)((f->xform->wedgeJulia_rN)*flam3_random_isaac_01(f->rc));
   double a = (f->precalc_atanyx + 2 * M_PI * t_rnd) / f->xform->wedge_julia_power;
   double c = floor( (f->xform->wedge_julia_count * a + M_PI)*M_1_PI*0.5 );
   double sa,ca;
   
   a = a * f->xform->wedgeJulia_cf + c * f->xform->wedge_julia_angle;
   
   sincos(a,&sa,&ca);

   f->p0 += r * ca;
   f->p1 += r * sa;
}

void var79_wedge_sph (flam3_iter_helper *f, double weight) {
   
   /* Wedge_sph from apo plugins pack */

   double r = 1.0/(f->precalc_sqrt+EPS);
   double a = f->precalc_atanyx + f->xform->wedge_sph_swirl * r;
   double c = floor( (f->xform->wedge_sph_count * a + M_PI)*M_1_PI*0.5);
   
   double comp_fac = 1 - f->xform->wedge_sph_angle*f->xform->wedge_sph_count*M_1_PI*0.5;
   double sa, ca;
   
   a = a * comp_fac + c * f->xform->wedge_sph_angle;

   sincos(a,&sa,&ca);   
   r = weight * (r + f->xform->wedge_sph_hole);
   
   f->p0 += r*ca;
   f->p1 += r*sa;

}

void var80_whorl (flam3_iter_helper *f, double weight) {
   
   /* whorl from apo plugins pack */
   
   /*
    * !!! Note !!!
    * This code uses the variation weight in a non-standard fashion, and
    * it may change or even be removed in future versions of flam3.
    */

   double r = f->precalc_sqrt;
   double a,sa,ca;

   if (r<weight)
      a = f->precalc_atanyx + f->xform->whorl_inside/(weight-r);
   else
      a = f->precalc_atanyx + f->xform->whorl_outside/(weight-r);
   
   sincos(a,&sa,&ca);
   
   f->p0 += weight*r*ca;
   f->p1 += weight*r*sa;

}

void var81_waves2 (flam3_iter_helper *f, double weight) {
   
   /* waves2 from Joel F */
   
   f->p0 += weight*(f->tx + f->xform->waves2_scalex*sin(f->ty * f->xform->waves2_freqx));
   f->p1 += weight*(f->ty + f->xform->waves2_scaley*sin(f->tx * f->xform->waves2_freqy));
   if (f->zpass)
      f->pz += weight*(f->tz + f->xform->waves2_scalez*sin(f->precalc_sqrt * f->xform->waves2_freqz));

}

/* complex vars by cothe */
/* exp log sin cos tan sec csc cot sinh cosh tanh sech csch coth */

void var82_exp (flam3_iter_helper *f, double weight) {
   //Exponential EXP
   double expe = exp(f->tx);
   double expcos,expsin;
   sincos(f->ty,&expsin,&expcos);
   f->p0 += weight * expe * expcos;
   f->p1 += weight * expe * expsin;
}
        
void var83_log (flam3_iter_helper *f, double weight) {
   //Natural Logarithm LOG
   // needs precalc_atanyx and precalc_sumsq
   // log_base (Apophysis) defaults to e, which gives the natural log
   double base = f->xform->log_base < 1e-6 ? 1e-6 : f->xform->log_base;
   f->p0 += weight * 0.5 * log(f->precalc_sumsq) / log(base);
   f->p1 += weight * f->precalc_atanyx;
   ZPASS(f, weight);
}

void var84_sin (flam3_iter_helper *f, double weight) {
   //Sine SIN
   double sinsin,sinacos,sinsinh,sincosh;
   sincos(f->tx,&sinsin,&sinacos);
   sinsinh = sinh(f->ty);
   sincosh = cosh(f->ty);
   f->p0 += weight * sinsin * sincosh;
   f->p1 += weight * sinacos * sinsinh;
}

void var85_cos (flam3_iter_helper *f, double weight) {
   //Cosine COS
   double cossin,coscos,cossinh,coscosh;
   sincos(f->tx,&cossin,&coscos);
   cossinh = sinh(f->ty);
   coscosh = cosh(f->ty);
   f->p0 += weight * coscos * coscosh;
   f->p1 -= weight * cossin * cossinh;
}

void var86_tan (flam3_iter_helper *f, double weight) {
   //Tangent TAN
   double tansin,tancos,tansinh,tancosh;
   double tanden;
   sincos(2*f->tx,&tansin,&tancos);
   tansinh = sinh(2.0*f->ty);
   tancosh = cosh(2.0*f->ty);
   tanden = 1.0/(tancos + tancosh);
   f->p0 += weight * tanden * tansin;
   f->p1 += weight * tanden * tansinh;
}

void var87_sec (flam3_iter_helper *f, double weight) {
   //Secant SEC
   double secsin,seccos,secsinh,seccosh;
   double secden;
   sincos(f->tx,&secsin,&seccos);
   secsinh = sinh(f->ty);
   seccosh = cosh(f->ty);
   secden = 2.0/(cos(2*f->tx) + cosh(2*f->ty));
   f->p0 += weight * secden * seccos * seccosh;
   f->p1 += weight * secden * secsin * secsinh;
}

void var88_csc (flam3_iter_helper *f, double weight) {
   //Cosecant CSC
   double cscsin,csccos,cscsinh,csccosh;
   double cscden;
   sincos(f->tx,&cscsin,&csccos);
   cscsinh = sinh(f->ty);
   csccosh = cosh(f->ty);
   cscden = 2.0/(cosh(2.0*f->ty) - cos(2.0*f->tx));
   f->p0 += weight * cscden * cscsin * csccosh;
   f->p1 -= weight * cscden * csccos * cscsinh;
}

void var89_cot (flam3_iter_helper *f, double weight) {
   //Cotangent COT
   double cotsin,cotcos,cotsinh,cotcosh;
   double cotden;
   sincos(2.0*f->tx,&cotsin,&cotcos);
   cotsinh = sinh(2.0*f->ty);
   cotcosh = cosh(2.0*f->ty);
   cotden = 1.0/(cotcosh - cotcos);
   f->p0 += weight * cotden * cotsin;
   f->p1 += weight * cotden * -1 * cotsinh;
}

void var90_sinh (flam3_iter_helper *f, double weight) {
   //Hyperbolic Sine SINH
   double sinhsin,sinhcos,sinhsinh,sinhcosh;
   sincos(f->ty,&sinhsin,&sinhcos);
   sinhsinh = sinh(f->tx);
   sinhcosh = cosh(f->tx);
   f->p0 += weight * sinhsinh * sinhcos;
   f->p1 += weight * sinhcosh * sinhsin;
}

void var91_cosh (flam3_iter_helper *f, double weight) {
   //Hyperbolic Cosine COSH
   double coshsin,coshcos,coshsinh,coshcosh;
   sincos(f->ty,&coshsin,&coshcos);
   coshsinh = sinh(f->tx);
   coshcosh = cosh(f->tx);
   f->p0 += weight * coshcosh * coshcos;
   f->p1 += weight * coshsinh * coshsin;
}

void var92_tanh (flam3_iter_helper *f, double weight) {
   //Hyperbolic Tangent TANH
   double tanhsin,tanhcos,tanhsinh,tanhcosh;
   double tanhden;
   sincos(2.0*f->ty,&tanhsin,&tanhcos);
   tanhsinh = sinh(2.0*f->tx);
   tanhcosh = cosh(2.0*f->tx);
   tanhden = 1.0/(tanhcos + tanhcosh);
   f->p0 += weight * tanhden * tanhsinh;
   f->p1 += weight * tanhden * tanhsin;
}

void var93_sech (flam3_iter_helper *f, double weight) {
   //Hyperbolic Secant SECH
   double sechsin,sechcos,sechsinh,sechcosh;
   double sechden;
   sincos(f->ty,&sechsin,&sechcos);
   sechsinh = sinh(f->tx);
   sechcosh = cosh(f->tx);
   sechden = 2.0/(cos(2.0*f->ty) + cosh(2.0*f->tx));
   f->p0 += weight * sechden * sechcos * sechcosh;
   f->p1 -= weight * sechden * sechsin * sechsinh;
}

void var94_csch (flam3_iter_helper *f, double weight) {
   //Hyperbolic Cosecant CSCH
   double cschsin,cschcos,cschsinh,cschcosh;
   double cschden;
   sincos(f->ty,&cschsin,&cschcos);
   cschsinh = sinh(f->tx);
   cschcosh = cosh(f->tx);
   cschden = 2.0/(cosh(2.0*f->tx) - cos(2.0*f->ty));
   f->p0 += weight * cschden * cschsinh * cschcos;
   f->p1 -= weight * cschden * cschcosh * cschsin;
}

void var95_coth (flam3_iter_helper *f, double weight) {
   //Hyperbolic Cotangent COTH
   double cothsin,cothcos,cothsinh,cothcosh;
   double cothden;
   sincos(2.0*f->ty,&cothsin,&cothcos);
   cothsinh = sinh(2.0*f->tx);
   cothcosh = cosh(2.0*f->tx);
   cothden = 1.0/(cothcosh - cothcos);
   f->p0 += weight * cothden * cothsinh;
   f->p1 += weight * cothden * cothsin;
}

void var96_auger (flam3_iter_helper *f, double weight) {

    // Auger, by Xyrus01
    double s = sin(f->xform->auger_freq * f->tx);
    double t = sin(f->xform->auger_freq * f->ty);
    double dy = f->ty + f->xform->auger_weight*(f->xform->auger_scale*s/2.0 + fabs(f->ty)*s);
    double dx = f->tx + f->xform->auger_weight*(f->xform->auger_scale*t/2.0 + fabs(f->tx)*t);

    f->p0 += weight * (f->tx + f->xform->auger_sym*(dx-f->tx));
    f->p1 += weight * dy;
    ZPASS(f, weight);
}

void var97_flux (flam3_iter_helper *f, double weight) {

    // Flux, by meckie
    double xpw = f->tx + weight;
    double xmw = f->tx - weight;
    double avgr = weight * (2 + f->xform->flux_spread) * sqrt( sqrt(f->ty*f->ty + xpw*xpw) / sqrt(f->ty*f->ty + xmw*xmw));
    double avga = ( atan2(f->ty, xmw) - atan2(f->ty,xpw) ) * 0.5;

    f->p0 += avgr * cos(avga);
    f->p1 += avgr * sin(avga);
}

void var98_mobius (flam3_iter_helper *f, double weight) {

    // Mobius, by eralex
    double re_u, im_u, re_v, im_v, rad_v;

    re_u = f->xform->mobius_re_a * f->tx - f->xform->mobius_im_a * f->ty + f->xform->mobius_re_b;
    im_u = f->xform->mobius_re_a * f->ty + f->xform->mobius_im_a * f->tx + f->xform->mobius_im_b;
    re_v = f->xform->mobius_re_c * f->tx - f->xform->mobius_im_c * f->ty + f->xform->mobius_re_d;
    im_v = f->xform->mobius_re_c * f->ty + f->xform->mobius_im_c * f->tx + f->xform->mobius_im_d;

    rad_v = weight / (re_v*re_v + im_v*im_v);

    f->p0 += rad_v * (re_u*re_v + im_u*im_v);
    f->p1 += rad_v * (im_u*re_v - re_u*im_v);
    ZPASS(f, weight);
}
    

/*
 * Variations from the Apophysis 7X "3D hack".  They are ported from the
 * Delphi sources (XForm.pas and Variations/var*.pas) and keep their
 * behaviour, including the way they use the z coordinate.
 */

/* Pseudo-gaussian random number in [-2,2], as used by Apophysis */
static double apo_gauss(flam3_iter_helper *f) {
   return flam3_random_isaac_01(f->rc) + flam3_random_isaac_01(f->rc)
        + flam3_random_isaac_01(f->rc) + flam3_random_isaac_01(f->rc) - 2.0;
}

/* Integer power parameter as rounded by Apophysis (never zero).   */
/* rint() rounds half to even like Delphi's Round() does.           */
static int apo_power(double p) {
   int n = (int)rint(p);
   return n == 0 ? 1 : n;
}

void var99_flatten (flam3_iter_helper *f, double weight) {
   /* post-variation: drops the z coordinate */
   f->pz = 0.0;
}

void var100_pre_zscale (flam3_iter_helper *f, double weight) {
   f->tz *= weight;
}

void var101_pre_ztranslate (flam3_iter_helper *f, double weight) {
   f->tz += weight;
}

void var102_pre_rotate_x (flam3_iter_helper *f, double weight) {
   double s,c,z;
   sincos(weight * M_PI_2, &s, &c);
   z = c * f->tz - s * f->ty;
   f->ty = s * f->tz + c * f->ty;
   f->tz = z;
}

void var103_pre_rotate_y (flam3_iter_helper *f, double weight) {
   double s,c,x;
   sincos(weight * M_PI_2, &s, &c);
   x = c * f->tx - s * f->tz;
   f->tz = s * f->tx + c * f->tz;
   f->tx = x;
}

void var104_zscale (flam3_iter_helper *f, double weight) {
   f->pz += weight * f->tz;
}

void var105_ztranslate (flam3_iter_helper *f, double weight) {
   f->pz += weight;
}

void var106_zcone (flam3_iter_helper *f, double weight) {
   f->pz += weight * f->precalc_sqrt;
}

void var107_post_rotate_x (flam3_iter_helper *f, double weight) {
   double s,c,z;
   sincos(weight * M_PI_2, &s, &c);
   z = c * f->pz - s * f->p1;
   f->p1 = s * f->pz + c * f->p1;
   f->pz = z;
}

void var108_post_rotate_y (flam3_iter_helper *f, double weight) {
   double s,c,x;
   sincos(weight * M_PI_2, &s, &c);
   x = c * f->p0 - s * f->pz;
   f->pz = s * f->p0 + c * f->pz;
   f->p0 = x;
}

void var109_zblur (flam3_iter_helper *f, double weight) {
   f->pz += weight * apo_gauss(f);
}

void var110_blur3D (flam3_iter_helper *f, double weight) {
   double sina,cosa,sinb,cosb;
   double r = weight * apo_gauss(f);

   sincos(flam3_random_isaac_01(f->rc) * 2 * M_PI, &sina, &cosa);
   sincos(flam3_random_isaac_01(f->rc) * M_PI, &sinb, &cosb);

   f->p0 += r * sinb * cosa;
   f->p1 += r * sinb * sina;
   f->pz += r * cosb;
}

void var111_hemisphere (flam3_iter_helper *f, double weight) {
   double t = weight / sqrt(f->precalc_sumsq + 1.0);

   f->p0 += f->tx * t;
   f->p1 += f->ty * t;
   f->pz += t;
}

void var112_julia3D (flam3_iter_helper *f, double weight) {
   int n = apo_power(f->xform->julia3D_power);
   int absn = abs(n);
   double cn = (1.0 / n - 1.0) / 2.0;
   double z = f->tz / absn;
   double r2d = f->precalc_sumsq;
   double r = weight * pow(r2d + z*z, cn);
   double tmp = r * sqrt(r2d);
   int k = (int)trunc(absn * flam3_random_isaac_01(f->rc));
   double sina, cosa;

   sincos((f->precalc_atanyx + 2 * M_PI * k) / n, &sina, &cosa);

   f->p0 += tmp * cosa;
   f->p1 += tmp * sina;
   f->pz += r * z;
}

void var113_julia3Dz (flam3_iter_helper *f, double weight) {
   int n = apo_power(f->xform->julia3Dz_power);
   int absn = abs(n);
   double cn = 1.0 / n / 2.0;
   double r2d = f->precalc_sumsq;
   double r = weight * pow(r2d, cn);
   int k = (int)trunc(absn * flam3_random_isaac_01(f->rc));
   double sina, cosa;

   sincos((f->precalc_atanyx + 2 * M_PI * k) / n, &sina, &cosa);

   f->p0 += r * cosa;
   f->p1 += r * sina;
   f->pz += r * f->tz / (f->precalc_sqrt * absn);
}

void var114_curl3D (flam3_iter_helper *f, double weight) {
   double cx = f->xform->curl3D_cx;
   double cy = f->xform->curl3D_cy;
   double cz = f->xform->curl3D_cz;
   double r2 = f->precalc_sumsq + f->tz * f->tz;
   double r = weight / (r2 * (cx*cx + cy*cy + cz*cz)
                        + 2*cx * f->tx - 2*cy * f->ty + 2*cz * f->tz + 1);

   f->p0 += r * (f->tx + cx * r2);
   f->p1 += r * (f->ty - cy * r2);
   f->pz += r * (f->tz + cz * r2);
}

void var115_blur_circle (flam3_iter_helper *f, double weight) {
   double x = 2.0 * flam3_random_isaac_01(f->rc) - 1.0;
   double y = 2.0 * flam3_random_isaac_01(f->rc) - 1.0;
   double absx = fabs(x), absy = fabs(y);
   double side, perimeter, r, sina, cosa;

   if (absx >= absy) {
      if (x >= absy)
         perimeter = absx + y;
      else
         perimeter = 5.0 * absx - y;
      side = absx;
   } else {
      if (y >= absx)
         perimeter = 3.0 * absy - x;
      else
         perimeter = 7.0 * absy + x;
      side = absy;
   }

   r = weight * side;
   sincos(M_PI_4 * perimeter / side - M_PI_4, &sina, &cosa);

   f->p0 += r * cosa;
   f->p1 += r * sina;
   ZPASS(f, weight);
}

void var116_blur_zoom (flam3_iter_helper *f, double weight) {
   double bx = f->xform->blur_zoom_x;
   double by = f->xform->blur_zoom_y;
   double z = 1.0 + f->xform->blur_zoom_length * flam3_random_isaac_01(f->rc);

   f->p0 += weight * ((f->tx - bx) * z + bx);
   f->p1 += weight * ((f->ty - by) * z - by);
   ZPASS(f, weight);
}

void var117_blur_pixelize (flam3_iter_helper *f, double weight) {
   double size = f->xform->blur_pixelize_size < 1e-6 ? 1e-6 : f->xform->blur_pixelize_size;
   double scale = f->xform->blur_pixelize_scale;
   double v = weight * size;
   double x = floor(f->tx / size);
   double y = floor(f->ty / size);

   f->p0 += v * (x + scale * (flam3_random_isaac_01(f->rc) - 0.5) + 0.5);
   f->p1 += v * (y + scale * (flam3_random_isaac_01(f->rc) - 0.5) + 0.5);
   ZPASS(f, weight);
}

/* Bubble wrap: returns 1 and warps (x,y) if the point lies in a bubble */
static int bwraps_warp(flam3_bwraps_params *bp, double *x, double *y) {
   double radius = 0.5 * (bp->cellsize / (1.0 + bp->space * bp->space));
   double g2 = bp->gain * bp->gain / (radius + 1e-6) + 1e-6;
   double max_bubble = g2 * radius;
   double r2, rfactor, cx, cy, lx, ly, r, theta, s, c;

   if (bp->cellsize == 0.0)
      return 0;

   if (max_bubble > 2.0)
      max_bubble = 1.0;
   else
      max_bubble *= 1.0 / (max_bubble * max_bubble / 4.0 + 1.0);

   r2 = radius * radius;
   rfactor = radius / max_bubble;

   cx = (floor(*x / bp->cellsize) + 0.5) * bp->cellsize;
   cy = (floor(*y / bp->cellsize) + 0.5) * bp->cellsize;
   lx = *x - cx;
   ly = *y - cy;

   if (lx*lx + ly*ly > r2)
      return 0;

   lx *= g2;
   ly *= g2;
   r = rfactor / ((lx*lx + ly*ly) / 4.0 + 1.0);
   lx *= r;
   ly *= r;
   r = (lx*lx + ly*ly) / r2;
   theta = bp->inner_twist * (1.0 - r) + bp->outer_twist * r;
   sincos(theta, &s, &c);

   *x = cx + c * lx + s * ly;
   *y = cy - s * lx + c * ly;
   return 1;
}

void var118_bwraps (flam3_iter_helper *f, double weight) {
   double x = f->tx, y = f->ty;

   bwraps_warp(&f->xform->bwraps, &x, &y);

   f->p0 += weight * x;
   f->p1 += weight * y;
   f->pz += weight * f->tz;
}

/* Crop: moves (x,y) into the crop rectangle (or to 0,0 with crop_zero) */
static void crop_apply(flam3_crop_params *cp, double *x, double *y, randctx *rc) {
   double x0 = cp->left < cp->right ? cp->left : cp->right;
   double x1 = cp->left < cp->right ? cp->right : cp->left;
   double y0 = cp->top < cp->bottom ? cp->top : cp->bottom;
   double y1 = cp->top < cp->bottom ? cp->bottom : cp->top;
   double s = cp->scatter_area < -1 ? -1 : (cp->scatter_area > 1 ? 1 : cp->scatter_area);
   double w = (x1 - x0) * 0.5 * s;
   double h = (y1 - y0) * 0.5 * s;
   int zero = rint(cp->zero) >= 1;

   if ((*x < x0 || *x > x1 || *y < y0 || *y > y1) && zero) {
      *x = 0.0;
      *y = 0.0;
   } else {
      if (*x < x0)
         *x = x0 + flam3_random_isaac_01(rc) * w;
      else if (*x > x1)
         *x = x1 - flam3_random_isaac_01(rc) * w;
      if (*y < y0)
         *y = y0 + flam3_random_isaac_01(rc) * h;
      else if (*y > y1)
         *y = y1 - flam3_random_isaac_01(rc) * h;
   }
}

void var119_crop (flam3_iter_helper *f, double weight) {
   double x = f->tx, y = f->ty;

   crop_apply(&f->xform->crop, &x, &y, f->rc);

   f->p0 += weight * x;
   f->p1 += weight * y;
   f->pz += weight * f->tz;
}

static double clamp01(double x) {
   return x < 0 ? 0 : (x > 1 ? 1 : x);
}

/* Falloff2: blurs v (x,y,z) depending on its distance from a center.  */
/* May also shift the color coordinate.                                */
static void falloff2_apply(flam3_falloff2_params *fp, double v[3], double *color, randctx *rc) {
   double scatter = fp->scatter < 1e-6 ? 1e-6 : fp->scatter;
   double mindist = fp->mindist < 0 ? 0 : fp->mindist;
   double mul_x = clamp01(fp->mul_x), mul_y = clamp01(fp->mul_y);
   double mul_z = clamp01(fp->mul_z), mul_c = clamp01(fp->mul_c);
   int type = (int)rint(fp->type);
   double dx = v[0] - fp->x0, dy = v[1] - fp->y0, dz = v[2] - fp->z0;
   double d = sqrt(dx*dx + dy*dy + dz*dz);
   double c;

   if (rint(fp->invert) >= 1) d = 1 - d;
   if (d < 0) d = 0;
   d = (d - mindist) * 0.04 * scatter;
   if (d < 0) d = 0;

   if (type == 1) {
      /* radial */
      double r_in = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) + 1e-6;
      double sigma = asin(v[2] / r_in) + mul_z * flam3_random_isaac_01(rc) * d;
      double phi = atan2(v[1], v[0]) + mul_y * flam3_random_isaac_01(rc) * d;
      double r = r_in + mul_x * flam3_random_isaac_01(rc) * d;
      double sins, coss, sinp, cosp;
      sincos(sigma, &sins, &coss);
      sincos(phi, &sinp, &cosp);
      v[0] = r * coss * cosp;
      v[1] = r * coss * sinp;
      v[2] = sins;
   } else if (type >= 2) {
      /* gaussian */
      double sigma = d * flam3_random_isaac_01(rc) * 2 * M_PI;
      double phi = d * flam3_random_isaac_01(rc) * M_PI;
      double r = d * flam3_random_isaac_01(rc);
      double sins, coss, sinp, cosp;
      sincos(sigma, &sins, &coss);
      sincos(phi, &sinp, &cosp);
      v[0] += mul_x * r * coss * cosp;
      v[1] += mul_y * r * coss * sinp;
      v[2] += mul_z * r * sins;
   } else {
      v[0] += mul_x * flam3_random_isaac_01(rc) * d;
      v[1] += mul_y * flam3_random_isaac_01(rc) * d;
      v[2] += mul_z * flam3_random_isaac_01(rc) * d;
   }

   c = *color + mul_c * flam3_random_isaac_01(rc) * d;
   *color = fabs(c - trunc(c));
}

void var120_falloff2 (flam3_iter_helper *f, double weight) {
   double v[3] = { f->tx, f->ty, f->tz };

   falloff2_apply(&f->xform->falloff2, v, &f->color, f->rc);

   f->p0 += weight * v[0];
   f->p1 += weight * v[1];
   f->pz += weight * v[2];
}

void var121_epispiral (flam3_iter_helper *f, double weight) {
   double theta = f->precalc_atanyx;
   double t = (flam3_random_isaac_01(f->rc) * f->xform->epispiral_thickness)
              / cos(f->xform->epispiral_n * theta) - f->xform->epispiral_holes;

   if (fabs(t) != 0) {
      f->p0 += weight * t * cos(theta);
      f->p1 += weight * t * sin(theta);
   }
}

void var122_pre_spherical (flam3_iter_helper *f, double weight) {
   double r = weight / (f->tx * f->tx + f->ty * f->ty + 10e-6);

   f->tx *= r;
   f->ty *= r;
   if (f->zpass)
      f->tz *= weight;
}

void var123_pre_sinusoidal (flam3_iter_helper *f, double weight) {
   f->tx = weight * sin(f->tx);
   f->ty = weight * sin(f->ty);
   if (f->zpass)
      f->tz *= weight;
}

void var124_pre_disc (flam3_iter_helper *f, double weight) {
   double sinr, cosr;
   double r = weight * M_1_PI * atan2(f->tx, f->ty);

   sincos(M_PI * sqrt(f->tx * f->tx + f->ty * f->ty), &sinr, &cosr);

   f->tx = sinr * r;
   f->ty = cosr * r;
   if (f->zpass)
      f->tz *= weight;
}

void var125_pre_bwraps (flam3_iter_helper *f, double weight) {
   double x = f->tx, y = f->ty;

   /* Like Apophysis, only points inside a bubble are scaled */
   if (bwraps_warp(&f->xform->pre_bwraps, &x, &y)) {
      f->tx = weight * x;
      f->ty = weight * y;
      f->tz *= weight;
   }
}

void var126_pre_crop (flam3_iter_helper *f, double weight) {
   double x = f->tx, y = f->ty;

   crop_apply(&f->xform->pre_crop, &x, &y, f->rc);

   f->tx = weight * x;
   f->ty = weight * y;
}

void var127_pre_falloff2 (flam3_iter_helper *f, double weight) {
   double v[3] = { f->tx, f->ty, f->tz };

   falloff2_apply(&f->xform->pre_falloff2, v, &f->color, f->rc);

   f->tx = weight * v[0];
   f->ty = weight * v[1];
   f->tz = weight * v[2];
}

void var128_post_bwraps (flam3_iter_helper *f, double weight) {
   flam3_bwraps_params bp = f->xform->post_bwraps;
   double x = f->p0, y = f->p1;

   if (bp.cellsize == 0.0)
      bp.cellsize = 1e-6;

   /* Like Apophysis, only points inside a bubble are scaled */
   if (bwraps_warp(&bp, &x, &y)) {
      f->p0 = weight * x;
      f->p1 = weight * y;
      f->pz *= weight;
   }
}

void var129_post_curl (flam3_iter_helper *f, double weight) {
   double c1 = f->xform->post_curl_c1 * weight;
   double c2 = f->xform->post_curl_c2 * weight;
   double x = f->p0, y = f->p1;
   double re = 1 + c1 * x + c2 * (x*x - y*y);
   double im = c1 * y + 2 * c2 * x * y;
   double r = re*re + im*im;

   f->p0 = (x * re + y * im) / r;
   f->p1 = (y * re - x * im) / r;
}

static double clamp_huge(double x) {
   return x < -1e100 ? -1e100 : (x > 1e100 ? 1e100 : x);
}

void var130_post_curl3D (flam3_iter_helper *f, double weight) {
   double cx = weight * f->xform->post_curl3D_cx;
   double cy = weight * f->xform->post_curl3D_cy;
   double cz = weight * f->xform->post_curl3D_cz;
   double x = clamp_huge(f->p0), y = clamp_huge(f->p1), z = clamp_huge(f->pz);
   double r2 = x*x + y*y + z*z;
   double r = 1.0 / (r2 * (cx*cx + cy*cy + cz*cz) + 2*cx * x - 2*cy * y + 2*cz * z + 1);

   f->p0 = r * (x + cx * r2);
   f->p1 = r * (y + cy * r2);
   f->pz = r * (z + cz * r2);
}

void var131_post_crop (flam3_iter_helper *f, double weight) {
   double x = f->p0, y = f->p1;

   crop_apply(&f->xform->post_crop, &x, &y, f->rc);

   f->p0 = weight * x;
   f->p1 = weight * y;
}

void var132_post_falloff2 (flam3_iter_helper *f, double weight) {
   double v[3] = { f->p0, f->p1, f->pz };

   falloff2_apply(&f->xform->post_falloff2, v, &f->color, f->rc);

   f->p0 = weight * v[0];
   f->p1 = weight * v[1];
   f->pz = weight * v[2];
}

/*
 * 3D variations of the Apophysis 7X plugin pack (src/Plugin/*.c).
 * Integer and ranged parameters are clamped the way the Apophysis
 * plugin interface clamps them when a flame is loaded.
 */

#define PLUGIN_EPS 1.0e-20

static double plugin_clamp(double v, double lo, double hi) {
   return v < lo ? lo : (v > hi ? hi : v);
}

static int plugin_int(double v, int lo, int hi) {
   return (int)plugin_clamp(floor(v + 0.5), lo, hi);
}

static double sgn(double v) {
   return v < 0 ? -1 : (v > 0 ? 1 : 0);
}

void var133_barycentroid (flam3_iter_helper *f, double weight) {
   double a = f->xform->barycentroid_a, b = f->xform->barycentroid_b;
   double c = f->xform->barycentroid_c, d = f->xform->barycentroid_d;
   double dot00 = a*a + b*b;
   double dot01 = a*c + b*d;
   double dot02 = a*f->tx + b*f->ty;
   double dot11 = c*c + d*d;
   double dot12 = c*f->tx + d*f->ty;
   double inv_denom = 1 / (dot00 * dot11 - dot01 * dot01);
   double u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
   double v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

   f->p0 += weight * sqrt(u*u + f->tx*f->tx) * sgn(u);
   f->p1 += weight * sqrt(v*v + f->ty*f->ty) * sgn(v);
   f->pz += weight * f->tz;
}

void var134_dc_bubble (flam3_iter_helper *f, double weight) {
   double scale = f->xform->dc_bubble_scale;
   double bdcs = 1.0 / (scale == 0.0 ? 10E-6 : scale);
   double r4_1 = weight / (f->precalc_sumsq / 4.0 + 1.0);
   double cx, cy;

   /* The plugin adds the accumulated output to itself; kept as is */
   f->p0 += f->p0 + r4_1 * f->tx;
   f->p1 += f->p1 + r4_1 * f->ty;
   f->pz += f->pz + weight * (2.0 / r4_1 - 1.0);

   cx = f->p0 + f->xform->dc_bubble_centerx;
   cy = f->p1 + f->xform->dc_bubble_centery;
   f->color = fmod(fabs(bdcs * (cx*cx + cy*cy)), 1.0);
}

void var135_dc_cube (flam3_iter_helper *f, double weight) {
   double p = 2 * flam3_random_isaac_01(f->rc) - 1;
   double q = 2 * flam3_random_isaac_01(f->rc) - 1;
   int i = (int)(3 * flam3_random_isaac_01(f->rc));
   int j = flam3_random_isaac_bit(f->rc);
   double x, y, z, c;

   switch (i) {
      case 0:
         x = weight * (j ? -1 : 1); y = weight * p; z = weight * q;
         c = j ? f->xform->dc_cube_c1 : f->xform->dc_cube_c2;
         break;
      case 1:
         x = weight * p; y = weight * (j ? -1 : 1); z = weight * q;
         c = j ? f->xform->dc_cube_c3 : f->xform->dc_cube_c4;
         break;
      default:
         x = weight * p; y = weight * q; z = weight * (j ? -1 : 1);
         c = j ? f->xform->dc_cube_c5 : f->xform->dc_cube_c6;
         break;
   }

   f->color = plugin_clamp(c, 0, 1);
   f->p0 += x * f->xform->dc_cube_x;
   f->p1 += y * f->xform->dc_cube_y;
   f->pz += z * f->xform->dc_cube_z;
}

/* Bitmaps for dc_image: like Apophysis, all .bmp files of one directory  */
/* (here the flam3_dc_images environment variable) sorted by name;        */
/* dc_image_index selects one of them.                                    */
#define DC_IMAGE_MAX 1024

typedef struct {
   int width, height, stride, size, bytes_per_pixel;
   unsigned char *pixels;
} dc_bitmap;

static dc_bitmap dc_bitmaps[DC_IMAGE_MAX];
static int dc_bitmap_count = -1;

#ifdef HAVE_LIBPTHREAD
static pthread_mutex_t dc_bitmap_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

static unsigned int le32(unsigned char *b) {
   return b[0] | (b[1] << 8) | (b[2] << 16) | ((unsigned int)b[3] << 24);
}

static int load_bmp(const char *path, dc_bitmap *bm) {
   unsigned char fh[14], ih[40];
   int bits;
   unsigned int offset;
   FILE *file = fopen(path, "rb");

   if (file == NULL)
      return 0;

   if (fread(fh, 1, 14, file) != 14 || fh[0] != 'B' || fh[1] != 'M'
       || fread(ih, 1, 40, file) != 40) {
      fclose(file);
      return 0;
   }

   offset = le32(fh + 10);
   bm->width = (int)le32(ih + 4);
   bm->height = (int)le32(ih + 8);
   bits = ih[14] | (ih[15] << 8);

   if (bits <= 0 || bits % 4 != 0 || bm->width <= 0 || bm->height <= 0) {
      fclose(file);
      return 0;
   }

   bm->stride = ((bm->width * bits + 31) & ~31) >> 3;
   bm->bytes_per_pixel = bits / 8;
   bm->size = bm->stride * bm->height;
   bm->pixels = (unsigned char *)calloc(bm->size, 1);

   if (bm->pixels == NULL || fseek(file, offset, SEEK_SET) != 0) {
      free(bm->pixels);
      fclose(file);
      return 0;
   }

   if (fread(bm->pixels, 1, bm->size, file) == 0) {
      free(bm->pixels);
      fclose(file);
      return 0;
   }

   fclose(file);
   return 1;
}

#ifndef _WIN32
#include <dirent.h>
#include <strings.h>

static int compare_names(const void *a, const void *b) {
   return strcasecmp(*(char * const *)a, *(char * const *)b);
}
#endif

static void load_dc_bitmaps(void) {
#ifndef _WIN32
   char *dir = getenv("flam3_dc_images");
   char **names = NULL;
   int nnames = 0, i;
   DIR *d;
   struct dirent *e;

   dc_bitmap_count = 0;

   if (dir == NULL || (d = opendir(dir)) == NULL)
      return;

   while ((e = readdir(d)) != NULL) {
      size_t len = strlen(e->d_name);
      if (len > 4 && !strcasecmp(e->d_name + len - 4, ".bmp")) {
         names = (char **)realloc(names, (nnames + 1) * sizeof(char *));
         names[nnames++] = strdup(e->d_name);
      }
   }
   closedir(d);

   qsort(names, nnames, sizeof(char *), compare_names);

   for (i = 0; i < nnames; i++) {
      char *path = (char *)malloc(strlen(dir) + strlen(names[i]) + 2);
      sprintf(path, "%s/%s", dir, names[i]);
      if (dc_bitmap_count < DC_IMAGE_MAX && load_bmp(path, &dc_bitmaps[dc_bitmap_count]))
         dc_bitmap_count++;
      else
         fprintf(stderr, "dc_image: could not load bitmap %s\n", path);
      free(path);
      free(names[i]);
   }
   free(names);
#else
   dc_bitmap_count = 0;
#endif
}

static dc_bitmap *get_dc_bitmap(int index) {
#ifdef HAVE_LIBPTHREAD
   pthread_mutex_lock(&dc_bitmap_lock);
#endif
   if (dc_bitmap_count < 0)
      load_dc_bitmaps();
#ifdef HAVE_LIBPTHREAD
   pthread_mutex_unlock(&dc_bitmap_lock);
#endif
   return (index >= 0 && index < dc_bitmap_count) ? &dc_bitmaps[index] : NULL;
}

void var136_dc_image (flam3_iter_helper *f, double weight) {
   flam3_xform *xf = f->xform;
   dc_bitmap *bm = get_dc_bitmap(plugin_int(xf->dc_image_index, -1, DC_IMAGE_MAX-1));
   double cmapmin, cmapmax, cmaprange, a, q, s, c, x, y, tx, ty;
   double r = 0, g = 0, b = 0, result = 0;
   int mode, output;
   long pos;

   if (bm == NULL)
      return;

   cmapmin = plugin_clamp(xf->dc_image_c0, 0, 1);
   cmapmax = plugin_clamp(xf->dc_image_c1, 0, 1);
   if (cmapmax < cmapmin) cmapmax = cmapmin;
   cmaprange = cmapmax - cmapmin;
   a = xf->dc_image_angle * M_PI;
   q = fabs(xf->dc_image_scale) < 0.000001 ? 100000 : 1 / xf->dc_image_scale;
   mode = plugin_int(xf->dc_image_mode, 0, 4);
   output = plugin_int(xf->dc_image_output, 0, 3);

   if (plugin_int(xf->dc_image_input, 0, 1) == 0) {
      x = f->p0; y = f->p1;
   } else {
      x = f->tx; y = f->ty;
   }

   sincos(a, &s, &c);
   tx = 0.5 * (x * q * c - y * q * s) + 0.5;
   ty = 0.5 * (x * q * s + y * q * c) + 0.5;
   tx = fmod(fabs(tx - xf->dc_image_x), 1.0) * bm->width;
   ty = fmod(fabs(ty - xf->dc_image_y), 1.0) * bm->height;

   pos = (long)(bm->height - (long)round(ty) - 1) * bm->stride
         + (long)round(tx) * bm->bytes_per_pixel;
   if (pos < 0 || pos >= bm->size)
      return;

   switch (bm->bytes_per_pixel) {
      case 1:
         r = g = b = bm->pixels[pos];
         break;
      case 2:
         b = bm->pixels[pos] & 0x1F;
         r = (bm->pixels[pos+1] & 0xFC) >> 2;
         g = ((bm->pixels[pos] & 0xE0) >> 5) | ((bm->pixels[pos+1] & 0x03) << 3);
         break;
      case 3:
      case 4:
         r = bm->pixels[pos+2];
         g = bm->pixels[pos+1];
         b = bm->pixels[pos];
         break;
   }

   switch (mode) {
      case 0: result = ((0.2989 * r + 0.5870 * g + 0.1140 * b) / 256.0); break;
      case 1: result = ((16384.0 * r + 256.0 * g + b) / 16777215.0); break;
      case 2: result = r / 256.0; break;
      case 3: result = g / 256.0; break;
      case 4: result = b / 256.0; break;
   }
   result = (result * cmaprange + cmapmin) * xf->dc_image_mul;

   if (plugin_int(xf->dc_image_overwrite, 0, 1) == 0) {
      switch (output) {
         case 0: result += f->color; break;
         case 1: result += f->tx; break;
         case 2: result += f->ty; break;
         case 3: result += f->tz; break;
      }
   }

   switch (output) {
      case 0: f->color = fmod(fabs(result), 1.0); break;
      case 1: f->p0 = result; break;
      case 2: f->p1 = result; break;
      case 3: f->pz = result; break;
   }
}

void var137_dc_linear (flam3_iter_helper *f, double weight) {
   double scale = f->xform->dc_linear_scale;
   double ldcs = 1.0 / (scale == 0.0 ? 10E-6 : scale);
   double s, c;

   f->p0 += weight * f->tx;
   f->p1 += weight * f->ty;
   f->pz += weight * f->tz;

   sincos(f->xform->dc_linear_angle, &s, &c);
   f->color = fmod(fabs(0.5 * (ldcs * (c * f->p0 + s * f->p1 + f->xform->dc_linear_offset) + 1.0)), 1.0);
}

void var138_dc_mandelbrot (flam3_iter_helper *f, double weight) {
   flam3_xform *xf = f->xform;
   double x=0.0, y=0.0, x1=0.0, y1=0.0, x2=0.0, y2=0.0, xtemp=0.0;
   double cx=0.0, cy=0.0;
   int maxiter = plugin_int(xf->dcm_iter, 5, INT_MAX);
   int miniter = plugin_int(xf->dcm_miniter, 0, INT_MAX);
   double xmax = xf->dcm_xmax, xmin = xf->dcm_xmin;
   double ymax = xf->dcm_ymax, ymin = xf->dcm_ymin;
   int max_retries = plugin_int(xf->dcm_retries, 0, INT_MAX);
   int mode = plugin_int(xf->dcm_mode, 0, 5);
   int color_method = plugin_int(xf->dcm_color_method, 0, 7);
   double sc = plugin_clamp(xf->dcm_scatter, -1000.0, 1000.0) / 10.0;
   double zs = weight * xf->dcm_zscale / maxiter;
   double xp = 0.0, yp = 0.0;
   int smooth_iter = 0, max_smooth_iter = plugin_int(xf->dcm_smooth_iter, 0, INT_MAX);
   int inverted, iter = 0, retries = 0;
   int isblur = (sc >= 0);
   double smoothed_iter = 0.0, inv_iter = 1.0, mag2;
   int m_power = plugin_int(xf->dcm_pow, -6, 6);
   int m_power_abs = m_power < 0 ? -m_power : m_power;

   inverted = flam3_random_isaac_01(f->rc) < plugin_clamp(xf->dcm_invert, 0, 1);
   if (!isblur) {
      xf->dcm_x0 = f->tx;
      xf->dcm_y0 = f->ty;
   }

   do {
      if (sc == 0) {
         /* Force selection of point at random */
         xf->dcm_x0 = xf->dcm_y0 = 0;
      }
      if (xf->dcm_x0 == 0 && xf->dcm_y0 == 0) {
         xf->dcm_x0 = (xmax-xmin) * flam3_random_isaac_01(f->rc) + xmin;
         xf->dcm_y0 = (ymax-ymin) * flam3_random_isaac_01(f->rc) + ymin;
      } else {
         /* Choose a point close to previous point */
         xf->dcm_x0 += sc * (flam3_random_isaac_01(f->rc) - 0.5);
         xf->dcm_y0 += sc * (flam3_random_isaac_01(f->rc) - 0.5);
      }

      cx = x1 = x = xp = xf->dcm_x0;
      cy = y1 = y = yp = xf->dcm_y0;
      if (mode == 1) {
         /* Julia set: the constant is the xform's offset */
         cx = xf->c[2][0];
         cy = xf->c[2][1];
      }

      iter = smooth_iter = 0;
      while ( (((x2=x*x) + (y2=y*y) < 4) && (iter < maxiter)) || (smooth_iter++ < max_smooth_iter) ) {
         /* the plugin only guards the first assignment; kept as is */
         if (smooth_iter == 0)
            xp = x;
         yp = y;
         if (mode == 2) y = -y;
         switch (m_power_abs) {
            case 3:
               xtemp = x*(x2 - 3*y2) + cx;
               y = y*(3*x2 - y2) + cy;
               x = xtemp;
               break;
            case 4:
               xtemp = (x2-y2)*(x2-y2) - 4*x2*y2 + cx;
               y = 4*x*y*(x2 - y2) + cy;
               x = xtemp;
               break;
            case 5:
               xtemp = x2*x2*x - 10*x2*x*y2 + 5*x*y2*y2 + cx;
               y = 5*x2*x2*y - 10*x2*y2*y + y2*y2*y + cy;
               x = xtemp;
               break;
            case 6:
               xtemp = x2*x2*x2 - 15*x2*x2*y2 + 15*x2*y2*y2 - y2*y2*y2 + cx;
               y = 6*x2*x2*x*y - 20*x2*x*y2*y + 6*x*y2*y2*y + cy;
               x = xtemp;
               break;
            case 1:
               if (m_power > 0 && iter < maxiter)
                  iter = maxiter - 1;
               break;
            default:
               xtemp = x2 - y2 + cx;
               y = 2*x*y + cy;
               x = xtemp;
               break;
         }
         if (m_power < 0 && (xtemp = x2 + y2) > 0) {
            x = x / xtemp;
            y = -y / xtemp;
         }
         iter++;
      }
      iter -= (smooth_iter - 1);

      if (miniter == 0 || (!inverted && iter >= maxiter)) {
         xf->dcm_x0 = xf->dcm_y0 = 0;
      } else if (iter < miniter || (inverted && iter < maxiter/2)) {
         xf->dcm_x0 = xf->dcm_y0 = 0;
      }
      if (++retries > max_retries)
         break;
   } while ((inverted && iter < maxiter)
            || (!inverted && (iter >= maxiter || (miniter > 0 && iter < miniter))));

   smoothed_iter = iter;
   if (max_smooth_iter > 0) {
      /* Normalized Iteration Count Algorithm for smoothing */
      mag2 = x2 + y2;
      if (mag2 > 1.1)
         smoothed_iter += 1 - log(log(mag2)/2)/M_LN2;
   }
   inv_iter = smoothed_iter > 0 ? 1/smoothed_iter : 1;

   f->p0 += weight * (x1 + xf->dcm_sx * x * inv_iter);
   f->p1 += weight * (y1 + xf->dcm_sy * y * inv_iter);
   if (xf->dcm_zscale != 0)
      f->pz += smoothed_iter * zs;

   if (smoothed_iter < 0) smoothed_iter = 0;
   if (smoothed_iter > maxiter) smoothed_iter = maxiter;

   switch (color_method) {
      case 1:
         xtemp = (y != 0.0) ? atan2(x, y) / (2*M_PI) : 0.0;
         f->color = fmod(fabs(xtemp), 1);
         break;
      case 2:
         xtemp = (yp != 0.0) ? atan2(xp, yp) / (2*M_PI) : 0.0;
         f->color = fmod(fabs(xtemp), 1);
         break;
      case 3:
         xtemp = (y - yp != 0.0) ? atan2(x - xp, y - yp) / (2*M_PI) : 0.0;
         f->color = fmod(fabs(smoothed_iter/maxiter * xtemp), 1);
         break;
      case 4:
         xtemp = (yp != 0.0) ? atan2(xp, yp) / (2*M_PI) : 0.0;
         f->color = fmod(fabs(smoothed_iter/maxiter * xtemp), 1);
         break;
      case 5:
         xtemp = (yp != 0.0) ? (0.5 + atan2(xp, yp) / (2*M_PI)) * (xp*xp + yp*yp) : 0.0;
         f->color = fmod(fabs(smoothed_iter/maxiter * xtemp), 1);
         break;
      case 6:
         xtemp = xp*xp + yp*yp;
         f->color = fmod(fabs(smoothed_iter/maxiter * xtemp), 1);
         break;
      case 7:
         xtemp = sqrt(xp*xp + yp*yp);
         f->color = fmod(fabs(smoothed_iter/maxiter * xtemp), 1);
         break;
      default:
         f->color = fmod(fabs(smoothed_iter/maxiter), 1);
         break;
   }
}

void var139_dc_triangle (flam3_iter_helper *f, double weight) {
   /* The triangle is the xform's own affine transform */
   double xx = f->xform->c[0][0], xy = f->xform->c[0][1];
   double yx = -f->xform->c[1][0], yy = -f->xform->c[1][1];
   double ox = f->xform->c[2][0], oy = f->xform->c[2][1];
   double px = f->tx - ox, py = f->ty - oy;
   double area = plugin_clamp(f->xform->dc_triangle_scatter_area, -1, 1);
   double dot00 = xx*xx + xy*xy;
   double dot01 = xx*yx + xy*yy;
   double dot02 = xx*px + xy*py;
   double dot11 = yx*yx + yy*yy;
   double dot12 = yx*px + yy*py;
   double denom = dot00 * dot11 - dot01 * dot01;
   double u = (dot11 * dot02 - dot01 * dot12) / denom;
   double v = (dot00 * dot12 - dot01 * dot02) / denom;
   int inside = 0, sg = 1;

   if (u + v > 1) {
      /* point escapes edge XY */
      sg = -1;
      if (u > v) { u = u > 1 ? 1 : u; v = 1 - u; }
      else       { v = v > 1 ? 1 : v; u = 1 - v; }
   } else if (u < 0 || v < 0) {
      /* point escapes edge OX or OY */
      u = plugin_clamp(u, 0, 1);
      v = plugin_clamp(v, 0, 1);
   } else
      inside = 1;

   if (plugin_int(f->xform->dc_triangle_zero_edges, 0, 1) && !inside)
      u = v = 0;
   else if (!inside) {
      u = plugin_clamp(u + flam3_random_isaac_01(f->rc) * area * sg, -1, 1);
      v = plugin_clamp(v + flam3_random_isaac_01(f->rc) * area * sg, -1, 1);
      if (u + v > 1 && area > 0) {
         if (u > v) { u = u > 1 ? 1 : u; v = 1 - u; }
         else       { v = v > 1 ? 1 : v; u = 1 - v; }
      }
   }

   f->p0 += weight * (ox + u * xx + v * yx);
   f->p1 += weight * (oy + u * xy + v * yy);
   f->pz += weight * f->tz;
   f->color = fmod(fabs(u + v), 1.0);
}

/* z factor of the dcztransl variations, computed from the color */
static double dcztransl_factor(double color, double x0, double x1, double factor, double clamp) {
   double lo, hi, range, zf;

   x0 = plugin_clamp(x0, 0, 1);
   x1 = plugin_clamp(x1, 0, 1);
   lo = x0 < x1 ? x0 : x1;
   hi = x0 > x1 ? x0 : x1;
   range = hi - lo == 0 ? PLUGIN_EPS : hi - lo;
   zf = factor * (color - lo) / range;

   if (plugin_int(clamp, 0, 1) != 0)
      zf = plugin_clamp(zf, 0, 1);
   return zf;
}

void var140_dc_ztransl (flam3_iter_helper *f, double weight) {
   flam3_xform *xf = f->xform;
   double zf = dcztransl_factor(f->color, xf->dc_ztransl_x0, xf->dc_ztransl_x1,
                                xf->dc_ztransl_factor, xf->dc_ztransl_clamp);

   f->p0 += weight * f->tx;
   f->p1 += weight * f->ty;
   if (plugin_int(xf->dc_ztransl_overwrite, 0, 1) == 0)
      f->pz += weight * f->tz * zf;
   else
      f->pz += weight * zf;
}

void var141_extrude (flam3_iter_helper *f, double weight) {
   /* like the plugin, this replaces the z computed so far */
   if (flam3_random_isaac_01(f->rc) < f->xform->extrude_root_face)
      f->pz = weight < 0 ? 0 : weight;
   else
      f->pz = weight * flam3_random_isaac_01(f->rc);
}

void var142_falloff (flam3_iter_helper *f, double weight) {
   flam3_xform *xf = f->xform;
   int mode = plugin_int(xf->falloff_mode, 0, 2);   /* 0 default, 1 pre, 2 post */
   double x_in = mode == 2 ? f->p0 : f->tx;
   double y_in = mode == 2 ? f->p1 : f->ty;
   double z_in = mode == 2 ? f->pz : f->tz;
   double d = plugin_clamp(xf->falloff_mindist, 0, DBL_MAX);
   double s = 0.04 * plugin_clamp(xf->falloff_scatter, PLUGIN_EPS, DBL_MAX);
   int invert = plugin_int(xf->falloff_invert, 0, 1);
   double rx = plugin_clamp(xf->falloff_mul_x, 0, 1);
   double ry = plugin_clamp(xf->falloff_mul_y, 0, 1);
   double rz = plugin_clamp(xf->falloff_mul_z, 0, 1);
   double ax = flam3_random_isaac_01(f->rc) - 0.5;
   double ay = flam3_random_isaac_01(f->rc) - 0.5;
   double az = flam3_random_isaac_01(f->rc) - 0.5;
   double dx = x_in - xf->falloff_x0, dy = y_in - xf->falloff_y0, dz = z_in - xf->falloff_z0;
   double r = sqrt(dx*dx + dy*dy + dz*dz);
   double rc = ((invert ? (1-r < 0 ? 0 : 1-r) : (r < 0 ? 0 : r)) - d) * s;
   double rs = rc < 0 ? 0 : rc;
   double x_out, y_out, z_out;

   switch (plugin_int(xf->falloff_type, 0, 2)) {
      case 1: { /* radial */
         double sigma = asin(r == 0 ? 0 : z_in / r) + rz * az * rs;
         double phi = atan2(y_in, x_in) + ry * ay * rs;
         double rad = r + rx * ax * rs;
         double ss, sc, ps, pc;
         sincos(sigma, &ss, &sc);
         sincos(phi, &ps, &pc);
         x_out = weight * (rad * sc * pc);
         y_out = weight * (rad * sc * ps);
         z_out = weight * (rad * ss);
         break;
      }
      case 2: { /* box */
         double scale = plugin_clamp(rs, 0, 0.9) + 0.1;
         double denom = 1.0 / scale;
         double bp = plugin_int(xf->falloff_boxpow, 2, 32);
         x_out = weight * (x_in + rx * rs * (floor(x_in * denom) + scale * ax - x_in)) + rx * pow(ax, bp) * rs * denom;
         y_out = weight * (y_in + ry * rs * (floor(y_in * denom) + scale * ay - y_in)) + ry * pow(ay, bp) * rs * denom;
         z_out = weight * (z_in + rz * rs * (floor(z_in * denom) + scale * az - z_in)) + rz * pow(az, bp) * rs * denom;
         break;
      }
      default: /* linear */
         x_out = weight * (x_in + rx * ax * rs);
         y_out = weight * (y_in + ry * ay * rs);
         z_out = weight * (z_in + rz * az * rs);
         break;
   }

   /* the plugin applies its pre/post modes where it is called */
   if (mode == 2) {
      f->p0 = x_out; f->p1 = y_out; f->pz = z_out;
   } else if (mode == 1) {
      f->tx = x_out; f->ty = y_out; f->tz = z_out;
   } else {
      f->p0 += x_out; f->p1 += y_out; f->pz += z_out;
   }
}

static double gdo_fclp(double a) { return a < 0 ? -fmod(fabs(a), 1) : fmod(fabs(a), 1); }
static double gdo_fscl(double a) { return gdo_fclp((a + 1) / 2); }
static double gdo_fosc(double p, double a) { return gdo_fscl(-1 * cos(p * a * 2 * M_PI)); }
static double gdo_flip(double a, double b, double c) { return c * (b - a) + a; }

void var143_gdoffs (flam3_iter_helper *f, double weight) {
   flam3_xform *xf = f->xform;
   double gdodx = plugin_clamp(xf->gdoffs_delta_x, 0, 16) * 0.1;
   double gdody = plugin_clamp(xf->gdoffs_delta_y, 0, 16) * 0.1;
   double gdoax = (fabs(xf->gdoffs_area_x) < 0.1 ? 0.1 : fabs(xf->gdoffs_area_x)) * 2.0;
   double gdoay = (fabs(xf->gdoffs_area_y) < 0.1 ? 0.1 : fabs(xf->gdoffs_area_y)) * 2.0;
   double gdob = plugin_int(xf->gdoffs_gamma, 1, 6) * 2.0 / (gdoax > gdoay ? gdoax : gdoay);
   double osc_x = gdo_fosc(gdodx, 1), osc_y = gdo_fosc(gdody, 1);
   double in_x = f->tx + xf->gdoffs_center_x, in_y = f->ty + xf->gdoffs_center_y;
   double out_x, out_y;

   /* in square mode the plugin uses osc_x for both axes */
   if (plugin_int(xf->gdoffs_square, 0, 1))
      osc_y = osc_x;

   out_x = gdo_flip(gdo_flip(in_x, gdo_fosc(in_x, 4), osc_x), gdo_fosc(gdo_fclp(gdob * in_x), 4), osc_x);
   out_y = gdo_flip(gdo_flip(in_y, gdo_fosc(in_y, 4), osc_y), gdo_fosc(gdo_fclp(gdob * in_y), 4), osc_y);

   /* like the plugin, this replaces the output computed so far */
   f->p0 = weight * out_x;
   f->p1 = weight * out_y;
   f->pz = weight * f->tz;
}

typedef struct { double x, y; } octa_pt;

static int octa_hits_rect(octa_pt tl, octa_pt br, octa_pt p) {
   return p.x >= tl.x && p.y >= tl.y && p.x <= br.x && p.y <= br.y;
}

static int octa_hits_triangle(octa_pt a, octa_pt b, octa_pt c, octa_pt p) {
   double v0x = c.x - a.x, v0y = c.y - a.y;
   double v1x = b.x - a.x, v1y = b.y - a.y;
   double v2x = p.x - a.x, v2y = p.y - a.y;
   double d00 = v0x*v0x + v0y*v0y, d01 = v0x*v1x + v0y*v1y, d02 = v0x*v2x + v0y*v2y;
   double d11 = v1x*v1x + v1y*v1y, d12 = v1x*v2x + v1y*v2y;
   double denom = d00 * d11 - d01 * d01;
   double u = 0, v = 0;

   if (denom != 0) {
      u = (d11 * d02 - d01 * d12) / denom;
      v = (d00 * d12 - d01 * d02) / denom;
   }
   return (u + v < 1.0) && (u > 0) && (v > 0);
}

void var144_octapol (flam3_iter_helper *f, double weight) {
   double s = fabs(f->xform->octapol_s), t = fabs(f->xform->octapol_t);
   double pw = f->xform->octapol_polarweight;
   double a = s * 0.5 + t;
   double rad = 0.707106781 * s * fabs(f->xform->octapol_radius);
   double x = f->tx * 0.15, y = f->ty * 0.15, r;
   octa_pt xy = { x, y };
   octa_pt A = { -0.5*s, 0.5*s + t }, B = { 0.5*s, 0.5*s + t };
   octa_pt C = { t, 0.5*s }, D = { t, -0.5*s };
   octa_pt E = { 0.5*s, -0.5*s - t }, F = { -0.5*s, -0.5*s - t };
   octa_pt G = { -t, -0.5*s }, H = { -t, 0.5*s };
   octa_pt I = { -0.5*s, 0.5*s }, J = { 0.5*s, 0.5*s };
   octa_pt K = { -0.5*s, -0.5*s }, L = { 0.5*s, -0.5*s };

   if (rad > 0 && (r = sqrt(x*x + y*y)) <= rad) {
      double rd = log((r / rad) * (r / rad));
      double phi = atan2(y, x);
      f->p0 += weight * (x + rd * pw * (phi - x));
      f->p1 += weight * (y + rd * pw * (r - y));
   } else if (fabs(x) <= a && fabs(y) <= a) {
      if (octa_hits_rect(H, K, xy) || octa_hits_rect(J, D, xy) ||
          octa_hits_rect(A, J, xy) || octa_hits_rect(K, E, xy) ||
          octa_hits_triangle(I, A, H, xy) || octa_hits_triangle(J, B, C, xy) ||
          octa_hits_triangle(L, D, E, xy) || octa_hits_triangle(K, F, G, xy)) {
         f->p0 += weight * x;
         f->p1 += weight * y;
      } else
         f->p0 = f->p1 = 0;
   } else
      f->p0 = f->p1 = 0;

   f->p0 += weight * x;
   f->p1 += weight * y;
   f->pz += weight * f->tz;
}

void var145_polynomial (flam3_iter_helper *f, double weight) {
   double xp = pow(weight * fabs(f->tx), f->xform->polynomial_powx);
   double yp = pow(weight * fabs(f->ty), f->xform->polynomial_powy);

   f->p0 += xp * sgn(f->tx) + f->xform->polynomial_lcx * f->tx + f->xform->polynomial_scx;
   f->p1 += yp * sgn(f->ty) + f->xform->polynomial_lcy * f->ty + f->xform->polynomial_scy;
   f->pz += weight * f->tz;
}

void var146_post_dcztransl (flam3_iter_helper *f, double weight) {
   flam3_xform *xf = f->xform;
   double zf = dcztransl_factor(f->color, xf->post_dcztransl_x0, xf->post_dcztransl_x1,
                                xf->post_dcztransl_factor, xf->post_dcztransl_clamp);

   f->p0 *= weight;
   f->p1 *= weight;
   if (plugin_int(xf->post_dcztransl_overwrite, 0, 1) == 0)
      f->pz = weight * f->pz * zf;
   else
      f->pz = weight * zf;
}

void var147_post_mirror_z (flam3_iter_helper *f, double weight) {
   f->pz = fabs(f->pz);
   if (flam3_random_isaac_bit(f->rc))
      f->pz = -f->pz;
}

void var148_pre_dcztransl (flam3_iter_helper *f, double weight) {
   flam3_xform *xf = f->xform;
   double zf = dcztransl_factor(f->color, xf->pre_dcztransl_x0, xf->pre_dcztransl_x1,
                                xf->pre_dcztransl_factor, xf->pre_ztransl_clamp);

   f->tx *= weight;
   f->ty *= weight;
   if (plugin_int(xf->pre_dcztransl_overwrite, 0, 1) == 0)
      f->tz = weight * f->tz * zf;
   else
      f->tz = weight * zf;
}

void var149_psphere (flam3_iter_helper *f, double weight) {
   double s0, c0, s1, c1;

   sincos(f->tx * weight * M_PI, &s0, &c0);
   sincos(f->ty * weight * M_PI, &s1, &c1);

   f->p0 += c0 * -s1;
   f->p1 += s0 * c1;
   f->pz += c1 * f->xform->psphere_zscale;
}

void var150_sinusgrid (flam3_iter_helper *f, double weight) {
   double fx = f->xform->sinusgrid_freqx * 2 * M_PI;
   double fy = f->xform->sinusgrid_freqy * 2 * M_PI;
   double sx, sy;

   if (fx == 0.0) fx = PLUGIN_EPS;
   if (fy == 0.0) fy = PLUGIN_EPS;
   sx = -1.0 * cos(f->tx * fx);
   sy = -1.0 * cos(f->ty * fy);

   f->p0 += weight * (f->tx + f->xform->sinusgrid_ampx * (sx - f->tx));
   f->p1 += weight * (f->ty + f->xform->sinusgrid_ampy * (sy - f->ty));
   f->pz += weight * f->tz;
}

void var151_linear2D (flam3_iter_helper *f, double weight) {
   /* "linear" of Apophysis before 7X 15C: drops z (3D linear is "linear3D") */
   f->p0 += weight * f->tx;
   f->p1 += weight * f->ty;
}

/* Plugins added after the pack above, so they are not in Apo's load order */
void var152_circlize (flam3_iter_helper *f, double weight) {
   double var4_PI = weight / M_PI_4;
   double absx = fabs(f->tx);
   double absy = fabs(f->ty);
   double perimeter, side, r, a, sina, cosa;

   if (absx >= absy) {
      if (f->tx >= absy)
         perimeter = absx + f->ty;
      else
         perimeter = 5.0 * absx - f->ty;
      side = absx;
   } else {
      if (f->ty >= absx)
         perimeter = 3.0 * absy - f->tx;
      else
         perimeter = 7.0 * absy + f->tx;
      side = absy;
   }

   /* the plugin does not scale hole by the weight */
   r = var4_PI * side + f->xform->circlize_hole;
   a = M_PI_4 * perimeter / side - M_PI_4;
   sincos(a, &sina, &cosa);

   f->p0 += r * cosa;
   f->p1 += r * sina;
   ZPASS(f, weight);
}

void var153_Spherical3D (flam3_iter_helper *f, double weight) {
   double r = weight / (f->tx * f->tx + f->ty * f->ty + f->tz * f->tz + PLUGIN_EPS);

   f->p0 += f->tx * r;
   f->p1 += f->ty * r;
   f->pz += f->tz * r;
}

void var154_scry_3D (flam3_iter_helper *f, double weight) {
   /* scry_3D by Larry Berlin: like scry, but uses atan2(y,x) for a zero z */
   double inv = 1.0 / (weight + PLUGIN_EPS);
   double t = f->tx * f->tx + f->ty * f->ty + f->tz * f->tz;
   double r = 1.0 / (sqrt(t) * (t + inv) + PLUGIN_EPS);
   double z = f->tz != 0.0 ? f->tz : atan2(f->ty, f->tx);

   f->p0 += f->tx * r;
   f->p1 += f->ty * r;
   f->pz += z * r;
}

/* zeta3D has no published source; this follows the code in zeta3D.dll.  */
/* It is the n=2 term of the zeta series, b^-s with s = x+iy (input z is  */
/* unused): x gets Re(b^-s), y Im(b^-s), z |b^-s|.  The base b of each    */
/* coordinate is that coordinate's output so far (from the variations    */
/* before it) when that is >= 2, otherwise 2.                             */
static double zeta3D_base(double acc) {
   return acc >= 2.0 ? acc : 2.0;
}

void var155_zeta3D (flam3_iter_helper *f, double weight) {
   double bx = zeta3D_base(f->p0);
   double by = zeta3D_base(f->p1);
   double bz = zeta3D_base(f->pz);

   f->p0 += weight * pow(bx, -f->tx) * cos(f->ty * log(bx));
   f->p1 -= weight * pow(by, -f->tx) * sin(f->ty * log(by));
   f->pz += weight * pow(bz, -f->tx);
}

/* Parameters of the variations above (and Apophysis extensions of older */
/* ones), handled generically by the parser, printer and interpolation.  */
#define XP(name,var,field,def) { name, var, offsetof(flam3_xform, field), def }
#define BWRAPS_PARAMS(pfx,var,field) \
   XP(pfx "bwraps_cellsize", var, field.cellsize, 1.0), \
   XP(pfx "bwraps_space", var, field.space, 0.0), \
   XP(pfx "bwraps_gain", var, field.gain, 1.0), \
   XP(pfx "bwraps_inner_twist", var, field.inner_twist, 0.0), \
   XP(pfx "bwraps_outer_twist", var, field.outer_twist, 0.0)
#define CROP_PARAMS(pfx,var,field) \
   XP(pfx "crop_left", var, field.left, -1.0), \
   XP(pfx "crop_top", var, field.top, -1.0), \
   XP(pfx "crop_right", var, field.right, 1.0), \
   XP(pfx "crop_bottom", var, field.bottom, 1.0), \
   XP(pfx "crop_scatter_area", var, field.scatter_area, 0.0), \
   XP(pfx "crop_zero", var, field.zero, 0.0)
#define FALLOFF2_PARAMS(pfx,var,field) \
   XP(pfx "falloff2_scatter", var, field.scatter, 1.0), \
   XP(pfx "falloff2_mindist", var, field.mindist, 0.5), \
   XP(pfx "falloff2_mul_x", var, field.mul_x, 1.0), \
   XP(pfx "falloff2_mul_y", var, field.mul_y, 1.0), \
   XP(pfx "falloff2_mul_z", var, field.mul_z, 0.0), \
   XP(pfx "falloff2_mul_c", var, field.mul_c, 0.0), \
   XP(pfx "falloff2_x0", var, field.x0, 0.0), \
   XP(pfx "falloff2_y0", var, field.y0, 0.0), \
   XP(pfx "falloff2_z0", var, field.z0, 0.0), \
   XP(pfx "falloff2_invert", var, field.invert, 0.0), \
   XP(pfx "falloff2_type", var, field.type, 0.0)

flam3_param_info flam3_extra_params[] = {
   XP("waves2_freqz", VAR_WAVES2, waves2_freqz, 0.0),
   XP("waves2_scalez", VAR_WAVES2, waves2_scalez, 0.0),
   XP("log_base", VAR_LOG, log_base, M_E),
   XP("julia3D_power", VAR_JULIA3D, julia3D_power, 2.0),
   XP("julia3Dz_power", VAR_JULIA3DZ, julia3Dz_power, 2.0),
   XP("curl3D_cx", VAR_CURL3D, curl3D_cx, 0.0),
   XP("curl3D_cy", VAR_CURL3D, curl3D_cy, 0.0),
   XP("curl3D_cz", VAR_CURL3D, curl3D_cz, 0.0),
   XP("post_curl3D_cx", VAR_POST_CURL3D, post_curl3D_cx, 0.0),
   XP("post_curl3D_cy", VAR_POST_CURL3D, post_curl3D_cy, 0.0),
   XP("post_curl3D_cz", VAR_POST_CURL3D, post_curl3D_cz, 0.0),
   XP("post_curl_c1", VAR_POST_CURL, post_curl_c1, 0.0),
   XP("post_curl_c2", VAR_POST_CURL, post_curl_c2, 0.0),
   XP("blur_zoom_length", VAR_BLUR_ZOOM, blur_zoom_length, 0.0),
   XP("blur_zoom_x", VAR_BLUR_ZOOM, blur_zoom_x, 0.0),
   XP("blur_zoom_y", VAR_BLUR_ZOOM, blur_zoom_y, 0.0),
   XP("blur_pixelize_size", VAR_BLUR_PIXELIZE, blur_pixelize_size, 0.1),
   XP("blur_pixelize_scale", VAR_BLUR_PIXELIZE, blur_pixelize_scale, 1.0),
   XP("epispiral_n", VAR_EPISPIRAL, epispiral_n, 6.0),
   XP("epispiral_thickness", VAR_EPISPIRAL, epispiral_thickness, 0.0),
   XP("epispiral_holes", VAR_EPISPIRAL, epispiral_holes, 1.0),
   BWRAPS_PARAMS("", VAR_BWRAPS, bwraps),
   BWRAPS_PARAMS("pre_", VAR_PRE_BWRAPS, pre_bwraps),
   BWRAPS_PARAMS("post_", VAR_POST_BWRAPS, post_bwraps),
   CROP_PARAMS("", VAR_CROP, crop),
   CROP_PARAMS("pre_", VAR_PRE_CROP, pre_crop),
   CROP_PARAMS("post_", VAR_POST_CROP, post_crop),
   FALLOFF2_PARAMS("", VAR_FALLOFF2, falloff2),
   FALLOFF2_PARAMS("pre_", VAR_PRE_FALLOFF2, pre_falloff2),
   FALLOFF2_PARAMS("post_", VAR_POST_FALLOFF2, post_falloff2),
   XP("barycentroid_a", VAR_BARYCENTROID, barycentroid_a, 1.0),
   XP("barycentroid_b", VAR_BARYCENTROID, barycentroid_b, 0.0),
   XP("barycentroid_c", VAR_BARYCENTROID, barycentroid_c, 0.0),
   XP("barycentroid_d", VAR_BARYCENTROID, barycentroid_d, 1.0),
   XP("dc_bubble_centerx", VAR_DC_BUBBLE, dc_bubble_centerx, 0.0),
   XP("dc_bubble_centery", VAR_DC_BUBBLE, dc_bubble_centery, 0.0),
   XP("dc_bubble_scale", VAR_DC_BUBBLE, dc_bubble_scale, 1.0),
   XP("dc_cube_c1", VAR_DC_CUBE, dc_cube_c1, 0.0),
   XP("dc_cube_c2", VAR_DC_CUBE, dc_cube_c2, 0.0),
   XP("dc_cube_c3", VAR_DC_CUBE, dc_cube_c3, 0.0),
   XP("dc_cube_c4", VAR_DC_CUBE, dc_cube_c4, 0.0),
   XP("dc_cube_c5", VAR_DC_CUBE, dc_cube_c5, 0.0),
   XP("dc_cube_c6", VAR_DC_CUBE, dc_cube_c6, 0.0),
   XP("dc_cube_x", VAR_DC_CUBE, dc_cube_x, 1.0),
   XP("dc_cube_y", VAR_DC_CUBE, dc_cube_y, 1.0),
   XP("dc_cube_z", VAR_DC_CUBE, dc_cube_z, 1.0),
   XP("dc_image_index", VAR_DC_IMAGE, dc_image_index, -1.0),
   XP("dc_image_mode", VAR_DC_IMAGE, dc_image_mode, 0.0),
   XP("dc_image_input", VAR_DC_IMAGE, dc_image_input, 0.0),
   XP("dc_image_output", VAR_DC_IMAGE, dc_image_output, 0.0),
   XP("dc_image_overwrite", VAR_DC_IMAGE, dc_image_overwrite, 1.0),
   XP("dc_image_mul", VAR_DC_IMAGE, dc_image_mul, 1.0),
   XP("dc_image_c0", VAR_DC_IMAGE, dc_image_c0, 0.0),
   XP("dc_image_c1", VAR_DC_IMAGE, dc_image_c1, 1.0),
   XP("dc_image_x", VAR_DC_IMAGE, dc_image_x, 0.0),
   XP("dc_image_y", VAR_DC_IMAGE, dc_image_y, 0.0),
   XP("dc_image_angle", VAR_DC_IMAGE, dc_image_angle, 0.0),
   XP("dc_image_scale", VAR_DC_IMAGE, dc_image_scale, 1.0),
   XP("dc_linear_offset", VAR_DC_LINEAR, dc_linear_offset, 0.0),
   XP("dc_linear_angle", VAR_DC_LINEAR, dc_linear_angle, 0.0),
   XP("dc_linear_scale", VAR_DC_LINEAR, dc_linear_scale, 1.0),
   XP("dcm_iter", VAR_DC_MANDELBROT, dcm_iter, 25.0),
   XP("dcm_miniter", VAR_DC_MANDELBROT, dcm_miniter, 1.0),
   XP("dcm_smooth_iter", VAR_DC_MANDELBROT, dcm_smooth_iter, 0.0),
   XP("dcm_retries", VAR_DC_MANDELBROT, dcm_retries, 50.0),
   XP("dcm_mode", VAR_DC_MANDELBROT, dcm_mode, 0.0),
   XP("dcm_pow", VAR_DC_MANDELBROT, dcm_pow, 2.0),
   XP("dcm_color_method", VAR_DC_MANDELBROT, dcm_color_method, 0.0),
   XP("dcm_invert", VAR_DC_MANDELBROT, dcm_invert, 0.0),
   XP("dcm_xmin", VAR_DC_MANDELBROT, dcm_xmin, -2.0),
   XP("dcm_xmax", VAR_DC_MANDELBROT, dcm_xmax, 2.0),
   XP("dcm_ymin", VAR_DC_MANDELBROT, dcm_ymin, -1.5),
   XP("dcm_ymax", VAR_DC_MANDELBROT, dcm_ymax, 1.5),
   XP("dcm_scatter", VAR_DC_MANDELBROT, dcm_scatter, 0.0),
   XP("dcm_sx", VAR_DC_MANDELBROT, dcm_sx, 0.0),
   XP("dcm_sy", VAR_DC_MANDELBROT, dcm_sy, 0.0),
   XP("dcm_zscale", VAR_DC_MANDELBROT, dcm_zscale, 0.0),
   XP("dc_triangle_scatter_area", VAR_DC_TRIANGLE, dc_triangle_scatter_area, 0.0),
   XP("dc_triangle_zero_edges", VAR_DC_TRIANGLE, dc_triangle_zero_edges, 0.0),
   XP("dc_ztransl_x0", VAR_DC_ZTRANSL, dc_ztransl_x0, 0.0),
   XP("dc_ztransl_x1", VAR_DC_ZTRANSL, dc_ztransl_x1, 1.0),
   XP("dc_ztransl_factor", VAR_DC_ZTRANSL, dc_ztransl_factor, 1.0),
   XP("dc_ztransl_overwrite", VAR_DC_ZTRANSL, dc_ztransl_overwrite, 1.0),
   XP("dc_ztransl_clamp", VAR_DC_ZTRANSL, dc_ztransl_clamp, 0.0),
   XP("extrude_root_face", VAR_EXTRUDE, extrude_root_face, 0.5),
   XP("falloff_mode", VAR_FALLOFF, falloff_mode, 0.0),
   XP("falloff_scatter", VAR_FALLOFF, falloff_scatter, 1.0),
   XP("falloff_mindist", VAR_FALLOFF, falloff_mindist, 0.5),
   XP("falloff_mul_x", VAR_FALLOFF, falloff_mul_x, 1.0),
   XP("falloff_mul_y", VAR_FALLOFF, falloff_mul_y, 1.0),
   XP("falloff_mul_z", VAR_FALLOFF, falloff_mul_z, 0.0),
   XP("falloff_x0", VAR_FALLOFF, falloff_x0, 0.0),
   XP("falloff_y0", VAR_FALLOFF, falloff_y0, 0.0),
   XP("falloff_z0", VAR_FALLOFF, falloff_z0, 0.0),
   XP("falloff_invert", VAR_FALLOFF, falloff_invert, 0.0),
   XP("falloff_type", VAR_FALLOFF, falloff_type, 0.0),
   XP("falloff_boxpow", VAR_FALLOFF, falloff_boxpow, 0.0),
   XP("gdoffs_delta_x", VAR_GDOFFS, gdoffs_delta_x, 0.0),
   XP("gdoffs_delta_y", VAR_GDOFFS, gdoffs_delta_y, 0.0),
   XP("gdoffs_area_x", VAR_GDOFFS, gdoffs_area_x, 2.0),
   XP("gdoffs_area_y", VAR_GDOFFS, gdoffs_area_y, 2.0),
   XP("gdoffs_center_x", VAR_GDOFFS, gdoffs_center_x, 0.0),
   XP("gdoffs_center_y", VAR_GDOFFS, gdoffs_center_y, 0.0),
   XP("gdoffs_gamma", VAR_GDOFFS, gdoffs_gamma, 1.0),
   XP("gdoffs_square", VAR_GDOFFS, gdoffs_square, 0.0),
   XP("octapol_polarweight", VAR_OCTAPOL, octapol_polarweight, 0.0),
   XP("octapol_radius", VAR_OCTAPOL, octapol_radius, 1.0),
   XP("octapol_s", VAR_OCTAPOL, octapol_s, 0.5),
   XP("octapol_t", VAR_OCTAPOL, octapol_t, 0.5),
   XP("polynomial_powx", VAR_POLYNOMIAL, polynomial_powx, 1.0),
   XP("polynomial_powy", VAR_POLYNOMIAL, polynomial_powy, 1.0),
   XP("polynomial_lcx", VAR_POLYNOMIAL, polynomial_lcx, 0.0),
   XP("polynomial_lcy", VAR_POLYNOMIAL, polynomial_lcy, 0.0),
   XP("polynomial_scx", VAR_POLYNOMIAL, polynomial_scx, 0.0),
   XP("polynomial_scy", VAR_POLYNOMIAL, polynomial_scy, 0.0),
   XP("post_dcztransl_x0", VAR_POST_DCZTRANSL, post_dcztransl_x0, 0.0),
   XP("post_dcztransl_x1", VAR_POST_DCZTRANSL, post_dcztransl_x1, 1.0),
   XP("post_dcztransl_factor", VAR_POST_DCZTRANSL, post_dcztransl_factor, 1.0),
   XP("post_dcztransl_overwrite", VAR_POST_DCZTRANSL, post_dcztransl_overwrite, 1.0),
   XP("post_dcztransl_clamp", VAR_POST_DCZTRANSL, post_dcztransl_clamp, 0.0),
   XP("pre_dcztransl_x0", VAR_PRE_DCZTRANSL, pre_dcztransl_x0, 0.0),
   XP("pre_dcztransl_x1", VAR_PRE_DCZTRANSL, pre_dcztransl_x1, 1.0),
   XP("pre_dcztransl_factor", VAR_PRE_DCZTRANSL, pre_dcztransl_factor, 1.0),
   XP("pre_dcztransl_overwrite", VAR_PRE_DCZTRANSL, pre_dcztransl_overwrite, 1.0),
   XP("pre_ztransl_clamp", VAR_PRE_DCZTRANSL, pre_ztransl_clamp, 0.0),
   XP("psphere_zscale", VAR_PSPHERE, psphere_zscale, 0.0),
   XP("sinusgrid_ampx", VAR_SINUSGRID, sinusgrid_ampx, 0.5),
   XP("sinusgrid_ampy", VAR_SINUSGRID, sinusgrid_ampy, 0.5),
   XP("sinusgrid_freqx", VAR_SINUSGRID, sinusgrid_freqx, 1.0),
   XP("sinusgrid_freqy", VAR_SINUSGRID, sinusgrid_freqy, 1.0),
   XP("circlize_hole", VAR_CIRCLIZE, circlize_hole, 0.0),
};

int flam3_num_extra_params = vlen(flam3_extra_params);

/* Precalc functions */

void perspective_precalc(flam3_xform *xf) {
   double ang = xf->perspective_angle * M_PI / 2.0;
   xf->persp_vsin = sin(ang);
   xf->persp_vfcos = xf->perspective_dist * cos(ang);
}

void juliaN_precalc(flam3_xform *xf) {
   xf->julian_rN = fabs(xf->julian_power);
   xf->julian_cn = xf->julian_dist / (double)xf->julian_power / 2.0;
}

void wedgeJulia_precalc(flam3_xform *xf) {
   xf->wedgeJulia_cf = 1.0 - xf->wedge_julia_angle * xf->wedge_julia_count * M_1_PI * 0.5;
   xf->wedgeJulia_rN = fabs(xf->wedge_julia_power);
   xf->wedgeJulia_cn = xf->wedge_julia_dist / xf->wedge_julia_power / 2.0;
}

void juliaScope_precalc(flam3_xform *xf) {
   xf->juliascope_rN = fabs(xf->juliascope_power);
   xf->juliascope_cn = xf->juliascope_dist / (double)xf->juliascope_power / 2.0;
}

void radial_blur_precalc(flam3_xform *xf) {
   sincos(xf->radial_blur_angle * M_PI / 2.0,
             &xf->radialBlur_spinvar, &xf->radialBlur_zoomvar);
}

void waves_precalc(flam3_xform *xf) {
   double dx = xf->c[2][0];
   double dy = xf->c[2][1];

   xf->waves_dx2 = 1.0/(dx * dx + EPS);
   xf->waves_dy2 = 1.0/(dy * dy + EPS);
}

void disc2_precalc(flam3_xform *xf) {
   double add = xf->disc2_twist;
   double k;

   xf->disc2_timespi = xf->disc2_rot * M_PI;

   sincos(add,&xf->disc2_sinadd,&xf->disc2_cosadd);
   xf->disc2_cosadd -= 1;

   if (add > 2 * M_PI) {
      k = (1 + add - 2*M_PI);
      xf->disc2_cosadd *= k;
      xf->disc2_sinadd *= k;
   }

   if (add < -2 * M_PI) {
      k = (1 + add + 2*M_PI);
      xf->disc2_cosadd *= k;
      xf->disc2_sinadd *= k;
   }
}

void supershape_precalc(flam3_xform *xf) {
   xf->super_shape_pm_4 = xf->super_shape_m / 4.0;
   xf->super_shape_pneg1_n1 = -1.0 / xf->super_shape_n1;
}

void xform_precalc(flam3_genome *cp, int xi) {

   perspective_precalc(&(cp->xform[xi]));
   juliaN_precalc(&(cp->xform[xi]));
   juliaScope_precalc(&(cp->xform[xi]));
   radial_blur_precalc(&(cp->xform[xi]));
   waves_precalc(&(cp->xform[xi]));
   disc2_precalc(&(cp->xform[xi]));
   supershape_precalc(&(cp->xform[xi]));
   wedgeJulia_precalc(&(cp->xform[xi]));   
}   

/* 0 for pre_ variations, 2 for post_ ones (and flatten), 1 otherwise */
static int var_stage(int v) {
   if (!strncmp(flam3_variation_names[v], "pre_", 4))
      return 0;
   if (!strncmp(flam3_variation_names[v], "post_", 5) || v==VAR_FLATTEN)
      return 2;
   return 1;
}

/* Reorder the active variations like Apophysis does: all pre_ variations */
/* first, then the normal ones and the post_ ones last, each group in     */
/* index order.                                                           */
static void order_active_vars(flam3_xform *xf) {
   int func[flam3_nvariations];
   double weights[flam3_nvariations];
   int stage,i,n=0;

   xf->num_pre_vars = 0;
   xf->num_post_vars = 0;

   for (stage=0; stage<3; stage++) {
      for (i=0; i<xf->num_active_vars; i++) {
         if (var_stage(xf->varFunc[i]) != stage)
            continue;
         func[n] = xf->varFunc[i];
         weights[n] = xf->active_var_weights[i];
         /* linear2D is Apophysis' variation #1: right after linear */
         if (func[n] == VAR_LINEAR2D) {
            int k = n;
            while (k > 0 && var_stage(func[k-1]) == stage && func[k-1] > VAR_LINEAR) {
               func[k] = func[k-1];
               weights[k] = weights[k-1];
               k--;
            }
            func[k] = VAR_LINEAR2D;
            weights[k] = xf->active_var_weights[i];
         }
         n++;
         if (stage==0) xf->num_pre_vars++;
         if (stage==2) xf->num_post_vars++;
      }
   }

   memcpy(xf->varFunc, func, n*sizeof(int));
   memcpy(xf->active_var_weights, weights, n*sizeof(double));
}

int prepare_precalc_flags(flam3_genome *cp) {

   double d;
   int i,j,totnum;

   /* Loop over valid xforms */
   for (i = 0; i < cp->num_xforms; i++) {
      d = cp->xform[i].density;
      if (d < 0.0) {
         fprintf(stderr, "xform %d weight must be non-negative, not %g.\n",i,d);
         return(1);
      }

      if (i != cp->final_xform_index && d == 0.0)
         continue;

      totnum = 0;

      cp->xform[i].vis_adjusted = adjust_percentage(cp->xform[i].opacity);

      cp->xform[i].precalc_angles_flag=0;
      cp->xform[i].precalc_atan_xy_flag=0;
      cp->xform[i].precalc_atan_yx_flag=0;
      cp->xform[i].has_preblur=0;
      cp->xform[i].has_post = !(id_matrix(cp->xform[i].post));


      for (j = 0; j < flam3_nvariations; j++) {

         if (cp->xform[i].var[j]!=0) {

            cp->xform[i].varFunc[totnum] = j;
            cp->xform[i].active_var_weights[totnum] = cp->xform[i].var[j];

            if (j==VAR_POLAR) {
               cp->xform[i].precalc_atan_xy_flag=1;
            } else if (j==VAR_HANDKERCHIEF) {
               cp->xform[i].precalc_atan_xy_flag=1;
            } else if (j==VAR_HEART) {
               cp->xform[i].precalc_atan_xy_flag=1;
            } else if (j==VAR_DISC) {
               cp->xform[i].precalc_atan_xy_flag=1;
            } else if (j==VAR_SPIRAL) {
               cp->xform[i].precalc_angles_flag=1;
            } else if (j==VAR_HYPERBOLIC) {
               cp->xform[i].precalc_angles_flag=1;
            } else if (j==VAR_DIAMOND) {
               cp->xform[i].precalc_angles_flag=1;
            } else if (j==VAR_EX) {
               cp->xform[i].precalc_atan_xy_flag=1;
            } else if (j==VAR_JULIA) {
               cp->xform[i].precalc_atan_xy_flag=1;
            } else if (j==VAR_POWER) {
               cp->xform[i].precalc_angles_flag=1;
            } else if (j==VAR_RINGS) {
               cp->xform[i].precalc_angles_flag=1;
            } else if (j==VAR_FAN) {
               cp->xform[i].precalc_atan_xy_flag=1;
            } else if (j==VAR_BLOB) {
               cp->xform[i].precalc_atan_xy_flag=1;
               cp->xform[i].precalc_angles_flag=1;
            } else if (j==VAR_FAN2) {
               cp->xform[i].precalc_atan_xy_flag=1;
            } else if (j==VAR_RINGS2) {
               cp->xform[i].precalc_angles_flag=1;
            } else if (j==VAR_JULIAN) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_JULIASCOPE) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_RADIAL_BLUR) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_NGON) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_DISC2) {
               cp->xform[i].precalc_atan_xy_flag=1;
            } else if (j==VAR_SUPER_SHAPE) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_FLOWER) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_CONIC) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_CPOW) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_ESCHER) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_POLAR2) {
               cp->xform[i].precalc_atan_xy_flag=1;
            } else if (j==VAR_WEDGE) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_WEDGE_JULIA) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_WEDGE_SPH) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_WHORL) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_LOG) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_JULIA3D) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_JULIA3DZ) {
               cp->xform[i].precalc_atan_yx_flag=1;
            } else if (j==VAR_EPISPIRAL) {
               cp->xform[i].precalc_atan_yx_flag=1;
            }
            
            totnum++;
         }
      }

      cp->xform[i].num_active_vars = totnum;
      order_active_vars(&cp->xform[i]);

   }
   
   return(0);
}


static void apply_var(flam3_iter_helper *f, int var, double weight)
{
      switch (var) {
 
         case (VAR_LINEAR):
            var0_linear(f, weight); break;               
         case (VAR_SINUSOIDAL):
                var1_sinusoidal(f, weight); break;
         case (VAR_SPHERICAL):
                var2_spherical(f, weight); break;
         case (VAR_SWIRL):
                var3_swirl(f, weight); break;
         case (VAR_HORSESHOE):
                var4_horseshoe(f, weight); break;               
         case (VAR_POLAR): 
                var5_polar(f, weight); break;
         case (VAR_HANDKERCHIEF):
                var6_handkerchief(f, weight); break;               
         case (VAR_HEART):
                var7_heart(f, weight); break;               
         case (VAR_DISC):
                var8_disc(f, weight); break;               
         case (VAR_SPIRAL):
                var9_spiral(f, weight); break;               
         case (VAR_HYPERBOLIC):
                var10_hyperbolic(f, weight); break;               
         case (VAR_DIAMOND):
                var11_diamond(f, weight); break;               
         case (VAR_EX):
                var12_ex(f, weight); break;               
         case (VAR_JULIA): 
                var13_julia(f, weight); break;               
         case (VAR_BENT):
                var14_bent(f, weight); break;
         case (VAR_WAVES):
                var15_waves(f, weight); break;
         case (VAR_FISHEYE): 
                var16_fisheye(f, weight); break;
         case (VAR_POPCORN):
                var17_popcorn(f, weight); break;
         case (VAR_EXPONENTIAL):
                var18_exponential(f, weight); break;
         case (VAR_POWER): 
                var19_power(f, weight); break;               
         case (VAR_COSINE):
                var20_cosine(f, weight); break;
         case (VAR_RINGS):
                var21_rings(f, weight); break;
         case (VAR_FAN):
                var22_fan(f, weight); break;
         case (VAR_BLOB):
                var23_blob(f, weight); break;               
         case (VAR_PDJ):
                var24_pdj(f, weight); break;
         case (VAR_FAN2):
                var25_fan2(f, weight); break;
         case (VAR_RINGS2): 
                var26_rings2(f, weight); break;              
         case (VAR_EYEFISH): 
                var27_eyefish(f, weight); break;             
         case (VAR_BUBBLE):
                var28_bubble(f, weight); break;
         case (VAR_CYLINDER):
                var29_cylinder(f, weight); break;
         case (VAR_PERSPECTIVE):
                var30_perspective(f, weight); break;
         case (VAR_NOISE):
                var31_noise(f, weight); break;
         case (VAR_JULIAN): 
                var32_juliaN_generic(f, weight); break;            
         case (VAR_JULIASCOPE):
                var33_juliaScope_generic(f, weight);break;
         case (VAR_BLUR):
                var34_blur(f, weight); break;
         case (VAR_GAUSSIAN_BLUR):
                var35_gaussian(f, weight); break;
         case (VAR_RADIAL_BLUR):
                var36_radial_blur(f, weight); break;
         case (VAR_PIE):
                var37_pie(f, weight); break;
         case (VAR_NGON):
                var38_ngon(f, weight); break;          
         case (VAR_CURL):
                var39_curl(f, weight); break;
         case (VAR_RECTANGLES):
                var40_rectangles(f, weight); break;
         case (VAR_ARCH):
                var41_arch(f, weight); break;
         case (VAR_TANGENT):
                var42_tangent(f, weight); break;
         case (VAR_SQUARE):
                var43_square(f, weight); break;
         case (VAR_RAYS):
                var44_rays(f, weight); break;
         case (VAR_BLADE): 
                var45_blade(f, weight); break;              
         case (VAR_SECANT2): 
                var46_secant2(f, weight); break;               
         case (VAR_TWINTRIAN): 
                var47_twintrian(f, weight); break;               
         case (VAR_CROSS):
                var48_cross(f, weight); break;
         case (VAR_DISC2):
                var49_disc2(f, weight); break;            
         case (VAR_SUPER_SHAPE):
                var50_supershape(f, weight); break;
         case (VAR_FLOWER):
                var51_flower(f, weight); break;            
         case (VAR_CONIC):
                var52_conic(f, weight); break;
         case (VAR_PARABOLA): 
                var53_parabola(f, weight); break;              
         case (VAR_BENT2): 
                var54_bent2(f, weight); break;              
         case (VAR_BIPOLAR): 
                var55_bipolar(f, weight); break;              
         case (VAR_BOARDERS): 
                var56_boarders(f, weight); break;              
         case (VAR_BUTTERFLY): 
                var57_butterfly(f, weight); break;              
         case (VAR_CELL): 
                var58_cell(f, weight); break;              
         case (VAR_CPOW): 
                var59_cpow(f, weight); break;              
         case (VAR_CURVE): 
                var60_curve(f, weight); break;              
         case (VAR_EDISC): 
                var61_edisc(f, weight); break;              
         case (VAR_ELLIPTIC): 
                var62_elliptic(f, weight); break;              
         case (VAR_ESCHER): 
                var63_escher(f, weight); break;              
         case (VAR_FOCI): 
                var64_foci(f, weight); break;              
         case (VAR_LAZYSUSAN): 
                var65_lazysusan(f, weight); break;              
         case (VAR_LOONIE): 
                var66_loonie(f, weight); break;              
         case (VAR_MODULUS): 
                var68_modulus(f, weight); break;              
         case (VAR_OSCILLOSCOPE): 
                var69_oscope(f, weight); break;              
         case (VAR_POLAR2): 
                var70_polar2(f, weight); break;              
         case (VAR_POPCORN2): 
                var71_popcorn2(f, weight); break;              
         case (VAR_SCRY): 
                var72_scry(f, weight); break;              
         case (VAR_SEPARATION): 
                var73_separation(f, weight); break;              
         case (VAR_SPLIT):
                var74_split(f, weight); break;
         case (VAR_SPLITS):
                var75_splits(f, weight); break;
         case (VAR_STRIPES):
                var76_stripes(f, weight); break;
         case (VAR_WEDGE):
                var77_wedge(f, weight); break;
         case (VAR_WEDGE_JULIA):
                var78_wedge_julia(f, weight); break;
         case (VAR_WEDGE_SPH):
                var79_wedge_sph(f, weight); break;
         case (VAR_WHORL):
                var80_whorl(f, weight); break;
         case (VAR_WAVES2):
                var81_waves2(f, weight); break;
         case (VAR_EXP):
                var82_exp(f, weight); break;
         case (VAR_LOG):
                var83_log(f, weight); break;
         case (VAR_SIN):
                var84_sin(f, weight); break;
         case (VAR_COS):
                var85_cos(f, weight); break;
         case (VAR_TAN):
                var86_tan(f, weight); break;
         case (VAR_SEC):
                var87_sec(f, weight); break;
         case (VAR_CSC):
                var88_csc(f, weight); break;
         case (VAR_COT):
                var89_cot(f, weight); break;
         case (VAR_SINH):
                var90_sinh(f, weight); break;
         case (VAR_COSH):
                var91_cosh(f, weight); break;
         case (VAR_TANH):
                var92_tanh(f, weight); break;
         case (VAR_SECH):
                var93_sech(f, weight); break;
         case (VAR_CSCH):
                var94_csch(f, weight); break;
         case (VAR_COTH):
                var95_coth(f, weight); break;
         case (VAR_AUGER):
                var96_auger(f, weight); break;
         case (VAR_FLUX):
                var97_flux(f, weight); break;
         case (VAR_MOBIUS):
                var98_mobius(f, weight); break;
         case (VAR_PRE_BLUR):
                var67_pre_blur(f, weight); break;
         case (VAR_FLATTEN):
                var99_flatten(f, weight); break;
         case (VAR_PRE_ZSCALE):
                var100_pre_zscale(f, weight); break;
         case (VAR_PRE_ZTRANSLATE):
                var101_pre_ztranslate(f, weight); break;
         case (VAR_PRE_ROTATE_X):
                var102_pre_rotate_x(f, weight); break;
         case (VAR_PRE_ROTATE_Y):
                var103_pre_rotate_y(f, weight); break;
         case (VAR_ZSCALE):
                var104_zscale(f, weight); break;
         case (VAR_ZTRANSLATE):
                var105_ztranslate(f, weight); break;
         case (VAR_ZCONE):
                var106_zcone(f, weight); break;
         case (VAR_POST_ROTATE_X):
                var107_post_rotate_x(f, weight); break;
         case (VAR_POST_ROTATE_Y):
                var108_post_rotate_y(f, weight); break;
         case (VAR_ZBLUR):
                var109_zblur(f, weight); break;
         case (VAR_BLUR3D):
                var110_blur3D(f, weight); break;
         case (VAR_HEMISPHERE):
                var111_hemisphere(f, weight); break;
         case (VAR_JULIA3D):
                var112_julia3D(f, weight); break;
         case (VAR_JULIA3DZ):
                var113_julia3Dz(f, weight); break;
         case (VAR_CURL3D):
                var114_curl3D(f, weight); break;
         case (VAR_BLUR_CIRCLE):
                var115_blur_circle(f, weight); break;
         case (VAR_BLUR_ZOOM):
                var116_blur_zoom(f, weight); break;
         case (VAR_BLUR_PIXELIZE):
                var117_blur_pixelize(f, weight); break;
         case (VAR_BWRAPS):
                var118_bwraps(f, weight); break;
         case (VAR_CROP):
                var119_crop(f, weight); break;
         case (VAR_FALLOFF2):
                var120_falloff2(f, weight); break;
         case (VAR_EPISPIRAL):
                var121_epispiral(f, weight); break;
         case (VAR_PRE_SPHERICAL):
                var122_pre_spherical(f, weight); break;
         case (VAR_PRE_SINUSOIDAL):
                var123_pre_sinusoidal(f, weight); break;
         case (VAR_PRE_DISC):
                var124_pre_disc(f, weight); break;
         case (VAR_PRE_BWRAPS):
                var125_pre_bwraps(f, weight); break;
         case (VAR_PRE_CROP):
                var126_pre_crop(f, weight); break;
         case (VAR_PRE_FALLOFF2):
                var127_pre_falloff2(f, weight); break;
         case (VAR_POST_BWRAPS):
                var128_post_bwraps(f, weight); break;
         case (VAR_POST_CURL):
                var129_post_curl(f, weight); break;
         case (VAR_POST_CURL3D):
                var130_post_curl3D(f, weight); break;
         case (VAR_POST_CROP):
                var131_post_crop(f, weight); break;
         case (VAR_POST_FALLOFF2):
                var132_post_falloff2(f, weight); break;
         case (VAR_BARYCENTROID):
                var133_barycentroid(f, weight); break;
         case (VAR_DC_BUBBLE):
                var134_dc_bubble(f, weight); break;
         case (VAR_DC_CUBE):
                var135_dc_cube(f, weight); break;
         case (VAR_DC_IMAGE):
                var136_dc_image(f, weight); break;
         case (VAR_DC_LINEAR):
                var137_dc_linear(f, weight); break;
         case (VAR_DC_MANDELBROT):
                var138_dc_mandelbrot(f, weight); break;
         case (VAR_DC_TRIANGLE):
                var139_dc_triangle(f, weight); break;
         case (VAR_DC_ZTRANSL):
                var140_dc_ztransl(f, weight); break;
         case (VAR_EXTRUDE):
                var141_extrude(f, weight); break;
         case (VAR_FALLOFF):
                var142_falloff(f, weight); break;
         case (VAR_GDOFFS):
                var143_gdoffs(f, weight); break;
         case (VAR_OCTAPOL):
                var144_octapol(f, weight); break;
         case (VAR_POLYNOMIAL):
                var145_polynomial(f, weight); break;
         case (VAR_POST_DCZTRANSL):
                var146_post_dcztransl(f, weight); break;
         case (VAR_POST_MIRROR_Z):
                var147_post_mirror_z(f, weight); break;
         case (VAR_PRE_DCZTRANSL):
                var148_pre_dcztransl(f, weight); break;
         case (VAR_PSPHERE):
                var149_psphere(f, weight); break;
         case (VAR_SINUSGRID):
                var150_sinusgrid(f, weight); break;
         case (VAR_LINEAR2D):
                var151_linear2D(f, weight); break;
         case (VAR_CIRCLIZE):
                var152_circlize(f, weight); break;
         case (VAR_SPHERICAL3D):
                var153_Spherical3D(f, weight); break;
         case (VAR_SCRY_3D):
                var154_scry_3D(f, weight); break;
         case (VAR_ZETA3D):
                var155_zeta3D(f, weight); break;
      }
}

int apply_xform(flam3_genome *cp, int fn, double *p, double *q, randctx *rc)
{
   flam3_iter_helper f;
   flam3_xform *xf = &(cp->xform[fn]);
   int var_n;
   int nvars = xf->num_active_vars;
   int normal_end = nvars - xf->num_post_vars;
   double s1;

   f.rc = rc;
   f.zpass = !cp->apo_pre15c;

   s1 = xf->color_speed;

   q[2] = s1 * xf->color + (1.0-s1) * p[2];
   q[3] = xf->vis_adjusted;

   //fprintf(stderr,"%d : %f %f %f\n",fn,xf->c[0][0],xf->c[1][0],xf->c[2][0]);

   f.tx = xf->c[0][0] * p[0] + xf->c[1][0] * p[1] + xf->c[2][0];
   f.ty = xf->c[0][1] * p[0] + xf->c[1][1] * p[1] + xf->c[2][1];
   /* 3D hack: the affine transform leaves z alone */
   f.tz = p[4];

   f.color = q[2];
   f.xform = xf;

   /* Pre-xforms go here, and modify the f.tx, f.ty and f.tz values */
   for (var_n=0; var_n < xf->num_pre_vars; var_n++)
      apply_var(&f, xf->varFunc[var_n], xf->active_var_weights[var_n]);

   /* Always calculate sumsq and sqrt */
   f.precalc_sumsq = f.tx*f.tx + f.ty*f.ty;
   f.precalc_sqrt = sqrt(f.precalc_sumsq);

   /* Check to see if we can precalculate any parts */
   /* Precalculate atanxy, sin, cos */
   if (xf->precalc_atan_xy_flag > 0) {
      f.precalc_atan = atan2(f.tx,f.ty);
   }
   
   if (xf->precalc_angles_flag > 0) {
      f.precalc_sina = f.tx / f.precalc_sqrt;
      f.precalc_cosa = f.ty / f.precalc_sqrt;
   }

   /* Precalc atanyx */
   if (xf->precalc_atan_yx_flag > 0) {
      f.precalc_atanyx = atan2(f.ty,f.tx);
   }

   f.p0 = 0.0;
   f.p1 = 0.0;
   f.pz = 0.0;

   /* Normal variations, then post-variations modifying f.p0, f.p1, f.pz */
   for (var_n=xf->num_pre_vars; var_n < normal_end; var_n++)
      apply_var(&f, xf->varFunc[var_n], xf->active_var_weights[var_n]);

   for (var_n=normal_end; var_n < nvars; var_n++)
      apply_var(&f, xf->varFunc[var_n], xf->active_var_weights[var_n]);

   /* apply the post transform */
   if (xf->has_post) {
      q[0] = xf->post[0][0] * f.p0 + xf->post[1][0] * f.p1 + xf->post[2][0];
      q[1] = xf->post[0][1] * f.p0 + xf->post[1][1] * f.p1 + xf->post[2][1];
   } else {
      q[0] = f.p0;
      q[1] = f.p1;
   }
   q[4] = f.pz;

   /* Keep var_color of the color change made by the variations */
   q[2] += xf->var_color * (f.color - q[2]);

   /* Check for badvalues and return randoms if bad */
   if (badvalue(q[0]) || badvalue(q[1]) || badvalue(q[4])) {
      q[0] = flam3_random_isaac_11(rc);
      q[1] = flam3_random_isaac_11(rc);
      q[4] = 0.0;
      return(1);
   } else
      return(0);

}

void initialize_xforms(flam3_genome *thiscp, int start_here) {

   int i,j;
   for (i = start_here ; i < thiscp->num_xforms ; i++) {
      thiscp->xform[i].padding = 0;
      thiscp->xform[i].density = 0.0;
      thiscp->xform[i].color_speed = 0.5;
      thiscp->xform[i].animate = 1.0;
      thiscp->xform[i].color = i&1;
      thiscp->xform[i].opacity = 1.0;
      thiscp->xform[i].var[0] = 1.0;
      thiscp->xform[i].motion_freq = 0;
      thiscp->xform[i].motion_func = 0;
      thiscp->xform[i].num_motion = 0;
      thiscp->xform[i].motion = NULL;
      for (j = 1; j < flam3_nvariations; j++)
         thiscp->xform[i].var[j] = 0.0;
      thiscp->xform[i].c[0][0] = 1.0;
      thiscp->xform[i].c[0][1] = 0.0;
      thiscp->xform[i].c[1][0] = 0.0;
      thiscp->xform[i].c[1][1] = 1.0;
      thiscp->xform[i].c[2][0] = 0.0;
      thiscp->xform[i].c[2][1] = 0.0;
      thiscp->xform[i].post[0][0] = 1.0;
      thiscp->xform[i].post[0][1] = 0.0;
      thiscp->xform[i].post[1][0] = 0.0;
      thiscp->xform[i].post[1][1] = 1.0;
      thiscp->xform[i].post[2][0] = 0.0;
      thiscp->xform[i].post[2][1] = 0.0;
      thiscp->xform[i].wind[0] = 0.0;
      thiscp->xform[i].wind[1] = 0.0;
      thiscp->xform[i].blob_low = 0.0;
      thiscp->xform[i].blob_high = 1.0;
      thiscp->xform[i].blob_waves = 1.0;
      thiscp->xform[i].pdj_a = 0.0;
      thiscp->xform[i].pdj_b = 0.0;
      thiscp->xform[i].pdj_c = 0.0;
      thiscp->xform[i].pdj_d = 0.0;
      thiscp->xform[i].fan2_x = 0.0;
      thiscp->xform[i].fan2_y = 0.0;
      thiscp->xform[i].rings2_val = 0.0;
      thiscp->xform[i].perspective_angle = 0.0;
      thiscp->xform[i].perspective_dist = 0.0;
      thiscp->xform[i].persp_vsin = 0.0;
      thiscp->xform[i].persp_vfcos = 0.0;
      thiscp->xform[i].radial_blur_angle = 0.0;
      thiscp->xform[i].disc2_rot = 0.0;
      thiscp->xform[i].disc2_twist = 0.0;
      thiscp->xform[i].disc2_sinadd = 0.0;
      thiscp->xform[i].disc2_cosadd = 0.0;
      thiscp->xform[i].disc2_timespi = 0.0;
      thiscp->xform[i].flower_petals = 0.0;
      thiscp->xform[i].flower_holes = 0.0;
      thiscp->xform[i].parabola_height = 0.0;
      thiscp->xform[i].parabola_width = 0.0;
      thiscp->xform[i].bent2_x = 1.0;
      thiscp->xform[i].bent2_y = 1.0;
      thiscp->xform[i].bipolar_shift = 0.0;
      thiscp->xform[i].cell_size = 1.0;
      thiscp->xform[i].cpow_r = 1.0;
      thiscp->xform[i].cpow_i = 0.0;
      thiscp->xform[i].cpow_power = 1.0;
      thiscp->xform[i].curve_xamp = 0.0;
      thiscp->xform[i].curve_yamp = 0.0;
      thiscp->xform[i].curve_xlength = 1.0;
      thiscp->xform[i].curve_ylength = 1.0;
      thiscp->xform[i].escher_beta = 0.0;
      thiscp->xform[i].lazysusan_space = 0.0;
      thiscp->xform[i].lazysusan_twist = 0.0;
      thiscp->xform[i].lazysusan_spin = 0.0;
      thiscp->xform[i].lazysusan_x = 0.0;
      thiscp->xform[i].lazysusan_y = 0.0;
      thiscp->xform[i].modulus_x = 0.0;
      thiscp->xform[i].modulus_y = 0.0;
      thiscp->xform[i].oscope_separation = 1.0;
      thiscp->xform[i].oscope_frequency = M_PI;
      thiscp->xform[i].oscope_amplitude = 1.0;
      thiscp->xform[i].oscope_damping = 0.0;
      thiscp->xform[i].popcorn2_c = 0.0;
      thiscp->xform[i].popcorn2_x = 0.0;
      thiscp->xform[i].popcorn2_y = 0.0;
      thiscp->xform[i].separation_x = 0.0;
      thiscp->xform[i].separation_xinside = 0.0;
      thiscp->xform[i].separation_y = 0.0;
      thiscp->xform[i].separation_yinside = 0.0;
      thiscp->xform[i].split_xsize = 0.0;
      thiscp->xform[i].split_ysize = 0.0;
      thiscp->xform[i].splits_x = 0.0;
      thiscp->xform[i].splits_y = 0.0;
      thiscp->xform[i].stripes_space = 0.0;
      thiscp->xform[i].stripes_warp = 0.0;
      thiscp->xform[i].wedge_angle = 0.0;
      thiscp->xform[i].wedge_hole = 0.0;
      thiscp->xform[i].wedge_count = 1.0;
      thiscp->xform[i].wedge_swirl = 0.0;
      thiscp->xform[i].wedge_sph_angle = 0.0;
      thiscp->xform[i].wedge_sph_hole = 0.0;
      thiscp->xform[i].wedge_sph_count = 1.0;
      thiscp->xform[i].wedge_sph_swirl = 0.0;

      thiscp->xform[i].wedge_julia_power = 1.0;
      thiscp->xform[i].wedge_julia_dist = 0.0;
      thiscp->xform[i].wedge_julia_count = 1.0;
      thiscp->xform[i].wedge_julia_angle = 0.0;
      thiscp->xform[i].wedgeJulia_cf = 0.0;
      thiscp->xform[i].wedgeJulia_cn = 0.5;
      thiscp->xform[i].wedgeJulia_rN = 1.0;
      thiscp->xform[i].whorl_inside = 0.0;
      thiscp->xform[i].whorl_outside = 0.0;
      
      thiscp->xform[i].waves2_scalex = 0.0;       
      thiscp->xform[i].waves2_scaley = 0.0;       
      thiscp->xform[i].waves2_freqx = 0.0;       
      thiscp->xform[i].waves2_freqy = 0.0;  
      
      thiscp->xform[i].auger_freq = 1.0;
      thiscp->xform[i].auger_weight = 0.5;
      thiscp->xform[i].auger_sym = 0.0;
      thiscp->xform[i].auger_scale = 1.0;     

      thiscp->xform[i].flux_spread = 0.0;
       
      thiscp->xform[i].julian_power = 1.0;
      thiscp->xform[i].julian_dist = 1.0;
      thiscp->xform[i].julian_rN = 1.0;
      thiscp->xform[i].julian_cn = 0.5;
      thiscp->xform[i].juliascope_power = 1.0;
      thiscp->xform[i].juliascope_dist = 1.0;
      thiscp->xform[i].juliascope_rN = 1.0;
      thiscp->xform[i].juliascope_cn = 0.5;
      thiscp->xform[i].radialBlur_spinvar = 0.0;
      thiscp->xform[i].radialBlur_zoomvar = 1.0;
      thiscp->xform[i].pie_slices = 6.0;
      thiscp->xform[i].pie_rotation = 0.0;
      thiscp->xform[i].pie_thickness = 0.5;
      thiscp->xform[i].ngon_sides = 5;
      thiscp->xform[i].ngon_power = 3;
      thiscp->xform[i].ngon_circle = 1;
      thiscp->xform[i].ngon_corners = 2;
      thiscp->xform[i].curl_c1 = 1.0;
      thiscp->xform[i].curl_c2 = 0.0;
      thiscp->xform[i].rectangles_x = 1.0;
      thiscp->xform[i].rectangles_y = 1.0;
      thiscp->xform[i].amw_amp = 1.0;
      thiscp->xform[i].super_shape_rnd = 0.0;
      thiscp->xform[i].super_shape_m = 0.0;
      thiscp->xform[i].super_shape_n1 = 1.0;
      thiscp->xform[i].super_shape_n2 = 1.0;
      thiscp->xform[i].super_shape_n3 = 1.0;
      thiscp->xform[i].super_shape_holes = 0.0;
      thiscp->xform[i].conic_eccentricity = 1.0;
      thiscp->xform[i].conic_holes = 0.0;

      thiscp->xform[i].mobius_re_a = 0.0;
      thiscp->xform[i].mobius_re_b = 0.0;
      thiscp->xform[i].mobius_re_c = 0.0;
      thiscp->xform[i].mobius_re_d = 0.0;
      thiscp->xform[i].mobius_im_a = 0.0;
      thiscp->xform[i].mobius_im_b = 0.0;
      thiscp->xform[i].mobius_im_c = 0.0;
      thiscp->xform[i].mobius_im_d = 0.0;

      thiscp->xform[i].var_color = 1.0;
      thiscp->xform[i].dcm_x0 = 0.0;
      thiscp->xform[i].dcm_y0 = 0.0;
      thiscp->xform[i].num_pre_vars = 0;
      thiscp->xform[i].num_post_vars = 0;

      for (j = 0; j < flam3_num_extra_params; j++)
         flam3_param(&thiscp->xform[i], j) = flam3_extra_params[j].def;
   }
}
