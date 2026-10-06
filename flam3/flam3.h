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


#ifndef flam3_included
#define flam3_included

#include <stdio.h>
#include <libxml/parser.h>
#include "isaac.h"

#if defined(_MSC_VER) /* VC++ */
#include <windows.h>
#define EXPORT __declspec (dllexport)
#else
#define EXPORT
#endif

EXPORT char *flam3_version();

#define flam3_palette_random       (-1)
#define flam3_palette_interpolated (-2)

#define flam3_defaults_on          (1)
#define flam3_defaults_off         (0)

#define flam3_name_len    64

#define flam3_print_edits  (1)
#define flam3_dont_print_edits  (0)

//typedef double flam3_palette[256][3];
typedef struct {
	double index;
	double color[4];
} flam3_palette_entry;

typedef flam3_palette_entry flam3_palette[256];

int flam3_get_palette(int palette_index, flam3_palette p, double hue_rotation);

#define flam3_variation_random (-1)
#define flam3_variation_random_fromspecified (-2)

extern char *flam3_variation_names[];

#define flam3_nvariations 156
#define flam3_nxforms     12

#define flam3_parent_fn_len     30

#define flam3_interpolation_linear 0
#define flam3_interpolation_smooth 1

#define flam3_inttype_linear 0
#define flam3_inttype_log    1
#define flam3_inttype_compat 2 /* Linear and old behaviour */
#define flam3_inttype_older  3 /* rotate padded xforms     */

#define flam3_palette_interpolation_hsv   0
#define flam3_palette_interpolation_sweep 1
#define flam3_palette_interpolation_rgb   2
#define flam3_palette_interpolation_hsv_circular  3

#define flam3_max_action_length 10000

#define flam3_palette_mode_step   0
#define flam3_palette_mode_linear 1

#define VAR_LINEAR   0
#define VAR_SINUSOIDAL  1
#define VAR_SPHERICAL  2
#define VAR_SWIRL 3
#define VAR_HORSESHOE  4
#define VAR_POLAR 5
#define VAR_HANDKERCHIEF 6
#define VAR_HEART 7
#define VAR_DISC 8
#define VAR_SPIRAL 9
#define VAR_HYPERBOLIC 10
#define VAR_DIAMOND 11
#define VAR_EX 12
#define VAR_JULIA 13
#define VAR_BENT 14
#define VAR_WAVES 15
#define VAR_FISHEYE 16
#define VAR_POPCORN 17
#define VAR_EXPONENTIAL 18
#define VAR_POWER 19
#define VAR_COSINE 20
#define VAR_RINGS 21
#define VAR_FAN 22
#define VAR_BLOB 23
#define VAR_PDJ 24
#define VAR_FAN2 25
#define VAR_RINGS2 26
#define VAR_EYEFISH 27
#define VAR_BUBBLE 28
#define VAR_CYLINDER 29
#define VAR_PERSPECTIVE 30
#define VAR_NOISE 31
#define VAR_JULIAN 32
#define VAR_JULIASCOPE 33
#define VAR_BLUR 34
#define VAR_GAUSSIAN_BLUR 35
#define VAR_RADIAL_BLUR 36
#define VAR_PIE 37
#define VAR_NGON 38
#define VAR_CURL 39
#define VAR_RECTANGLES 40
#define VAR_ARCH 41
#define VAR_TANGENT 42
#define VAR_SQUARE 43
#define VAR_RAYS 44
#define VAR_BLADE 45
#define VAR_SECANT2 46
#define VAR_TWINTRIAN 47
#define VAR_CROSS 48
#define VAR_DISC2 49
#define VAR_SUPER_SHAPE 50
#define VAR_FLOWER 51
#define VAR_CONIC 52
#define VAR_PARABOLA 53
#define VAR_BENT2 54
#define VAR_BIPOLAR 55
#define VAR_BOARDERS 56
#define VAR_BUTTERFLY 57
#define VAR_CELL 58
#define VAR_CPOW 59
#define VAR_CURVE 60
#define VAR_EDISC 61
#define VAR_ELLIPTIC 62
#define VAR_ESCHER 63
#define VAR_FOCI 64
#define VAR_LAZYSUSAN 65
#define VAR_LOONIE 66
#define VAR_PRE_BLUR 67
#define VAR_MODULUS 68
#define VAR_OSCILLOSCOPE 69
#define VAR_POLAR2 70
#define VAR_POPCORN2 71
#define VAR_SCRY 72
#define VAR_SEPARATION 73
#define VAR_SPLIT 74
#define VAR_SPLITS 75
#define VAR_STRIPES 76
#define VAR_WEDGE 77
#define VAR_WEDGE_JULIA 78
#define VAR_WEDGE_SPH 79
#define VAR_WHORL 80
#define VAR_WAVES2 81
#define VAR_EXP 82
#define VAR_LOG 83
#define VAR_SIN 84
#define VAR_COS 85
#define VAR_TAN 86
#define VAR_SEC 87
#define VAR_CSC 88
#define VAR_COT 89
#define VAR_SINH 90
#define VAR_COSH 91
#define VAR_TANH 92
#define VAR_SECH 93
#define VAR_CSCH 94
#define VAR_COTH 95
#define VAR_AUGER 96
#define VAR_FLUX 97
#define VAR_MOBIUS 98

/* Apophysis 7X "3D hack" variations.  Variations whose name starts with
   "pre_" are applied before all others, those starting with "post_"
   (and flatten) after all others, in increasing index order. */
#define VAR_FLATTEN 99
#define VAR_PRE_ZSCALE 100
#define VAR_PRE_ZTRANSLATE 101
#define VAR_PRE_ROTATE_X 102
#define VAR_PRE_ROTATE_Y 103
#define VAR_ZSCALE 104
#define VAR_ZTRANSLATE 105
#define VAR_ZCONE 106
#define VAR_POST_ROTATE_X 107
#define VAR_POST_ROTATE_Y 108
#define VAR_ZBLUR 109
#define VAR_BLUR3D 110
#define VAR_HEMISPHERE 111
#define VAR_JULIA3D 112
#define VAR_JULIA3DZ 113
#define VAR_CURL3D 114
#define VAR_BLUR_CIRCLE 115
#define VAR_BLUR_ZOOM 116
#define VAR_BLUR_PIXELIZE 117
#define VAR_BWRAPS 118
#define VAR_CROP 119
#define VAR_FALLOFF2 120
#define VAR_EPISPIRAL 121
#define VAR_PRE_SPHERICAL 122
#define VAR_PRE_SINUSOIDAL 123
#define VAR_PRE_DISC 124
#define VAR_PRE_BWRAPS 125
#define VAR_PRE_CROP 126
#define VAR_PRE_FALLOFF2 127
#define VAR_POST_BWRAPS 128
#define VAR_POST_CURL 129
#define VAR_POST_CURL3D 130
#define VAR_POST_CROP 131
#define VAR_POST_FALLOFF2 132

/* 3D variations of the Apophysis 7X plugin pack, in plugin load order */
#define VAR_BARYCENTROID 133
#define VAR_DC_BUBBLE 134
#define VAR_DC_CUBE 135
#define VAR_DC_IMAGE 136
#define VAR_DC_LINEAR 137
#define VAR_DC_MANDELBROT 138
#define VAR_DC_TRIANGLE 139
#define VAR_DC_ZTRANSL 140
#define VAR_EXTRUDE 141
#define VAR_FALLOFF 142
#define VAR_GDOFFS 143
#define VAR_OCTAPOL 144
#define VAR_POLYNOMIAL 145
#define VAR_POST_DCZTRANSL 146
#define VAR_POST_MIRROR_Z 147
#define VAR_PRE_DCZTRANSL 148
#define VAR_PSPHERE 149
#define VAR_SINUSGRID 150
#define VAR_LINEAR2D 151
#define VAR_CIRCLIZE 152
#define VAR_SPHERICAL3D 153
#define VAR_SCRY_3D 154
#define VAR_ZETA3D 155

typedef struct {

   double badvals;
   long int num_iters;
   int render_seconds;
   
} stat_struct;

typedef struct {

   unsigned int width, height;
   int version;
   int id;

   /* There are 256 levels of gray to work with */
   double intensity_weight[256];
   unsigned int bin_size[256];
   unsigned int bin_offset[256];

   /* Pointer to newly allocated memory; we will be allocating */
   /* 2*w*h ushorts for this storage.  The bin offset will     */
   /* provide the starting point for a random selection from   */
   /* (bin size) ordered pairs                                 */
   unsigned short *rowcols;

} flam3_image_store;


/* Parameter blocks shared by the plain, pre_ and post_ versions of the
   Apophysis bwraps, crop and falloff2 variations */
typedef struct {
   double cellsize, space, gain;
   double inner_twist, outer_twist;
} flam3_bwraps_params;

typedef struct {
   double left, top, right, bottom;
   double scatter_area, zero;
} flam3_crop_params;

typedef struct {
   double scatter, mindist;
   double mul_x, mul_y, mul_z, mul_c;
   double x0, y0, z0;
   double invert, type;
} flam3_falloff2_params;

typedef struct xform {
   double var[flam3_nvariations];   /* interp coefs between variations */
   double c[3][2];      /* the coefs to the affine part of the function */
   double post[3][2];   /* the post transform */
   double density;      /* probability that this function is chosen. 0 - 1 */
   double color;     /* color coords for this function. 0 - 1 */
   double color_speed;  /* scaling factor on color added to current iteration */
   double animate;      /* whether or not this xform rotates (in sheep) >0 means stationary */
   double opacity;   /* 0=invisible, 1=totally visible */
   double vis_adjusted; /* adjusted visibility for better transitions */
   
   int padding;/* Set to 1 for padding xforms */
   double wind[2]; /* winding numbers */

   int precalc_angles_flag;
   int precalc_atan_xy_flag;
   int precalc_atan_yx_flag;
   double has_preblur;
   int has_post;

   /* Params for new parameterized variations */
   /* Blob */
   double blob_low;
   double blob_high;
   double blob_waves;

   /* PDJ */
   double pdj_a;
   double pdj_b;
   double pdj_c;
   double pdj_d;

   /* Fan2 */
   double fan2_x;
   double fan2_y;

   /* Rings2 */
   double rings2_val;

   /* Perspective */
   double perspective_angle;
   double perspective_dist;

   /* Julia_N */
   double julian_power;
   double julian_dist;

   /* Julia_Scope */
   double juliascope_power;
   double juliascope_dist;

   /* Radial_Blur */
   double radial_blur_angle;

   /* Pie */
   double pie_slices;
   double pie_rotation;
   double pie_thickness;

   /* Ngon */
   double ngon_sides;
   double ngon_power;
   double ngon_circle;
   double ngon_corners;

   /* Curl */
   double curl_c1;
   double curl_c2;

   /* Rectangles */
   double rectangles_x;
   double rectangles_y;

   /* AMW */
   double amw_amp;

   /* Disc 2 */
   double disc2_rot;
   double disc2_twist;

   /* Supershape */
   double super_shape_rnd;
   double super_shape_m;
   double super_shape_n1;
   double super_shape_n2;
   double super_shape_n3;
   double super_shape_holes;
   
   /* Flower */
   double flower_petals;
   double flower_holes;
   
   /* Conic */
   double conic_eccentricity;
   double conic_holes;
   
   /* Parabola */
   double parabola_height;
   double parabola_width;
   
   /* Bent2 */
   double bent2_x;
   double bent2_y;
   
   /* Bipolar */
   double bipolar_shift;
   
   /* Cell */
   double cell_size;
   
   /* Cpow */
   double cpow_r;
   double cpow_i;
   double cpow_power; /* int in apo */
   
   /* Curve */
   double curve_xamp,curve_yamp;
   double curve_xlength,curve_ylength;
   
   /* Escher */
   double escher_beta;
   
   /* Lazysusan */
   double lazysusan_spin;
   double lazysusan_space;
   double lazysusan_twist;
   double lazysusan_x, lazysusan_y;
   
   /* Modulus */
   double modulus_x, modulus_y;
   
   /* Oscope */
   double oscope_separation;
   double oscope_frequency;
   double oscope_amplitude;
   double oscope_damping;
   
   /* Popcorn2 */
   double popcorn2_x, popcorn2_y, popcorn2_c;
   
   /* Separation */
   double separation_x, separation_xinside;
   double separation_y, separation_yinside;
   
   /* Split */
   double split_xsize;
   double split_ysize;
   
   /* Splits */
   double splits_x,splits_y;
   
   /* Stripes */
   double stripes_space;
   double stripes_warp;
   
   /* Wedge */
   double wedge_angle, wedge_hole;
   double wedge_count, wedge_swirl;
   
   /* Wedge_Julia */
   double wedge_julia_angle;
   double wedge_julia_count;
   double wedge_julia_power;
   double wedge_julia_dist;
   
   /* Wedge_Sph */
   double wedge_sph_angle, wedge_sph_count;
   double wedge_sph_hole, wedge_sph_swirl;
   
   /* Whorl */
   double whorl_inside, whorl_outside;
   
   /* Waves2 */
   double waves2_freqx, waves2_scalex;
   double waves2_freqy, waves2_scaley;
   
   /* Auger */
   double auger_sym, auger_weight;
   double auger_freq, auger_scale;

   /* Flux */
   double flux_spread;

   /* Mobius */
   double mobius_re_a, mobius_im_a;
   double mobius_re_b, mobius_im_b;
   double mobius_re_c, mobius_im_c;
   double mobius_re_d, mobius_im_d;

   /* Waves2 (3D) and Log extensions from Apophysis */
   double waves2_freqz, waves2_scalez;
   double log_base;

   /* Julia3D / Julia3Dz */
   double julia3D_power;
   double julia3Dz_power;

   /* Curl3D / Post_curl3D / Post_curl */
   double curl3D_cx, curl3D_cy, curl3D_cz;
   double post_curl3D_cx, post_curl3D_cy, post_curl3D_cz;
   double post_curl_c1, post_curl_c2;

   /* Blur_zoom / Blur_pixelize */
   double blur_zoom_length, blur_zoom_x, blur_zoom_y;
   double blur_pixelize_size, blur_pixelize_scale;

   /* Epispiral */
   double epispiral_n, epispiral_thickness, epispiral_holes;

   /* Bwraps / Crop / Falloff2 and their pre_ and post_ versions */
   flam3_bwraps_params bwraps, pre_bwraps, post_bwraps;
   flam3_crop_params crop, pre_crop, post_crop;
   flam3_falloff2_params falloff2, pre_falloff2, post_falloff2;

   /* Apophysis plugin pack 3D variations */
   double barycentroid_a, barycentroid_b, barycentroid_c, barycentroid_d;
   double dc_bubble_centerx, dc_bubble_centery, dc_bubble_scale;
   double dc_cube_c1, dc_cube_c2, dc_cube_c3, dc_cube_c4, dc_cube_c5, dc_cube_c6;
   double dc_cube_x, dc_cube_y, dc_cube_z;
   double dc_image_index, dc_image_mode, dc_image_input, dc_image_output;
   double dc_image_overwrite, dc_image_mul, dc_image_c0, dc_image_c1;
   double dc_image_x, dc_image_y, dc_image_angle, dc_image_scale;
   double dc_linear_offset, dc_linear_angle, dc_linear_scale;
   double dcm_iter, dcm_miniter, dcm_smooth_iter, dcm_retries, dcm_mode;
   double dcm_pow, dcm_color_method, dcm_invert, dcm_xmin, dcm_xmax;
   double dcm_ymin, dcm_ymax, dcm_scatter, dcm_sx, dcm_sy, dcm_zscale;
   double dcm_x0, dcm_y0; /* dc_mandelbrot state kept between iterations */
   double dc_triangle_scatter_area, dc_triangle_zero_edges;
   double dc_ztransl_x0, dc_ztransl_x1, dc_ztransl_factor;
   double dc_ztransl_overwrite, dc_ztransl_clamp;
   double pre_dcztransl_x0, pre_dcztransl_x1, pre_dcztransl_factor;
   double pre_dcztransl_overwrite, pre_ztransl_clamp; /* sic, as in Apophysis */
   double post_dcztransl_x0, post_dcztransl_x1, post_dcztransl_factor;
   double post_dcztransl_overwrite, post_dcztransl_clamp;
   double extrude_root_face;
   double falloff_mode, falloff_scatter, falloff_mindist;
   double falloff_mul_x, falloff_mul_y, falloff_mul_z;
   double falloff_x0, falloff_y0, falloff_z0;
   double falloff_invert, falloff_type, falloff_boxpow;
   double gdoffs_delta_x, gdoffs_delta_y, gdoffs_area_x, gdoffs_area_y;
   double gdoffs_center_x, gdoffs_center_y, gdoffs_gamma, gdoffs_square;
   double octapol_polarweight, octapol_radius, octapol_s, octapol_t;
   double polynomial_powx, polynomial_powy, polynomial_lcx, polynomial_lcy;
   double polynomial_scx, polynomial_scy;
   double psphere_zscale;
   double sinusgrid_ampx, sinusgrid_ampy, sinusgrid_freqx, sinusgrid_freqy;
   double circlize_hole;

   /* How much of the color changed by variations is kept (Apo var_color) */
   double var_color;
      
   /* If perspective is used, precalculate these values */
   /* from the _angle and _dist                         */
   double persp_vsin;
   double persp_vfcos;

   /* If Julia_N is used, precalculate these values */
   double julian_rN;
   double julian_cn;

   /* If Julia_Scope is used, precalculate these values */
   double juliascope_rN;
   double juliascope_cn;
   
   /* if Wedge_Julia, precalculate */
   double wedgeJulia_rN;
   double wedgeJulia_cn;
   double wedgeJulia_cf;

   /* If Radial_Blur is used, precalculate these values */
   double radialBlur_spinvar;
   double radialBlur_zoomvar;

   /* Precalculate these values for waves */
   double waves_dx2;
   double waves_dy2;

   /* If disc2 is used, precalculate these values */
   double disc2_sinadd;
   double disc2_cosadd;
   double disc2_timespi;

   /* If supershape is used, precalculate these values */
   double super_shape_pm_4;
   double super_shape_pneg1_n1;

   int num_active_vars;
   int num_pre_vars;  /* active vars [0,num_pre_vars) are pre_ variations */
   int num_post_vars; /* the last num_post_vars active vars are post_ ones */
   double active_var_weights[flam3_nvariations];
   int varFunc[flam3_nvariations];
   
   int motion_freq;
   int motion_func;
   
   struct xform *motion;
   int num_motion;
   

} flam3_xform;

typedef struct {
   char flame_name[flam3_name_len+1]; /* 64 chars plus a null */
   double time;
   int interpolation;
   int interpolation_type;
   int palette_interpolation;
   double hsv_rgb_palette_blend;
   int num_xforms;
   int final_xform_index;
   int final_xform_enable;
   flam3_xform *xform;
   
   /* Xaos implementation */
   double **chaos;
   int chaos_enable;
   
   int genome_index;                   /* index into source file */
   char parent_fname[flam3_parent_fn_len];   /* base filename where parent was located */
   int symmetry;                /* 0 means none */
   flam3_palette palette;
   char *input_image;           /* preview/temporary! */
   int  palette_index;
   double brightness;           /* 1.0 = normal */
   double contrast;             /* 1.0 = normal */
   double gamma;
   double highlight_power;
   int  width, height;          /* of the final image */
   int  spatial_oversample;
   double center[2];             /* of camera */
   double rot_center[2];         /* really the center */
   double rotate;                /* camera */
   double vibrancy;              /* blend between color algs (0=old,1=new) */
   double hue_rotation;          /* applies to cmap, 0-1 */
   double background[3];
   double zoom;                  /* effects ppu, sample density, scale */
   double pixels_per_unit;       /* vertically */
   double spatial_filter_radius; /* radius of spatial filter */
   int spatial_filter_select; /* selected spatial filter */
//   double (*spatial_filter_func)(double); /* spatial filter kernel function */
//   double spatial_filter_support; /* size of standard kernel for specific function */
   double sample_density;        /* samples per pixel (not bucket) */
   /* in order to motion blur more accurately we compute the logs of the
   sample density many times and average the results. */
   /* nbatches is the number of times the buckets are filtered into
   the abucket log accumulator */
   /* ntemporal_samples is the number of time steps per batch.  this many
   interpolated control points are used per batch and accumulated */
   int nbatches;
   int ntemporal_samples;

   /* Density estimation parameters for blurring low density hits */
   double estimator;             /* Filter width for bin with one hit */
   double estimator_curve;              /* Exponent on decay function ( MAX / a^(k-1) ) */
   double estimator_minimum;         /* Minimum filter width used -
                                    forces filter to be used of at least this width on all pts */

   /* XML Edit structure */
   xmlDocPtr edits;

   /* Small-gamma linearization threshold */
   double gam_lin_thresh;

   /* for cmap_interpolated hack */
   int palette_index0;
   double hue_rotation0;
   int palette_index1;
   double hue_rotation1;
   double palette_blend;

   int temporal_filter_type; /* Temporal filters */
   double temporal_filter_width, temporal_filter_exp;
   
   int palette_mode;

   /* 3D camera (Apophysis 3D hack); angles are in radians */
   double cam_pitch;
   double cam_yaw;
   double cam_perspective;
   double cam_zpos;
   double cam_dof;

   /* Apophysis before 7X 15C: the 2D variations do not pass z through */
   int apo_pre15c;

} flam3_genome;

/* Description of a variation parameter, used for the parameters of the
   variations added with the 3D hack so they can be handled generically */
typedef struct {
   char *name;     /* XML attribute name */
   int var;        /* index of the variation the parameter belongs to */
   size_t offset;  /* offsetof() the double in flam3_xform */
   double def;     /* default value */
} flam3_param_info;

extern flam3_param_info flam3_extra_params[];
extern int flam3_num_extra_params;

#define flam3_param(xf,i) (*(double *)((char *)(xf) + flam3_extra_params[i].offset))

typedef struct {
	int from;
	int to;
	double scalar;
} flam3_chaos_entry;

/* xform manipulation */

void flam3_add_motion_element(flam3_xform *xf);
void flam3_add_xforms(flam3_genome *cp, int num_to_add, int interp_padding, int final_flag);
void flam3_delete_xform(flam3_genome *thiscp, int idx_to_delete);
void flam3_copy_xform(flam3_xform *dest, flam3_xform *src);
void flam3_copy(flam3_genome *dest, flam3_genome *src);
void flam3_copyx(flam3_genome *dest, flam3_genome *src, int num_std, int num_final);
void flam3_copy_params(flam3_xform *dest, flam3_xform *src, int varn);
void flam3_delete_motion_elements(flam3_xform *xf);

EXPORT int flam3_xform_preview(flam3_genome *cp, int xi, double range, int numvals, int depth, double *result, randctx *rc);
EXPORT unsigned short* flam3_create_xform_distrib(flam3_genome *cp);
int flam3_create_chaos_distrib(flam3_genome *cp, int xi, unsigned short *xform_distrib);
int flam3_check_unity_chaos(flam3_genome *cp);
void clear_cp(flam3_genome *cp, int def_flag);

/* samples is array nsamples*4 long of x,y,color triples.
   using (samples[0], samples[1]) as starting XY point and
   (samples[2], samples[3]) as starting color coordinate,
   perform fuse iterations and throw them away, then perform
   nsamples iterations and save them in the samples array */
EXPORT int flam3_iterate(flam3_genome *g, int nsamples, int fuse, double *samples,
                     unsigned short *xform_distrib, randctx *rc);

void apply_motion_parameters(flam3_xform *xf, flam3_xform *addto, double blend);

/* genomes is array ngenomes long, with times set and in ascending order.
   interpolate to the requested time and return in result */
EXPORT void flam3_interpolate(flam3_genome *genomes, int ngenomes, double time, double stagger, flam3_genome *result);

/* print genome to given file with extra_attributes if not NULL */
EXPORT void flam3_print(FILE *f, flam3_genome *g, char *extra_attributes, int print_edits);
void flam3_print_xform(FILE *f, flam3_xform *x, int final_flag, int numstd, double *chaos_row, int motion_flag);
EXPORT char *flam3_print_to_string(flam3_genome *cp);

/* ivars is a list of variations to use, or flam3_variation_random     */
/* ivars_n is the number of values in ivars to select from.            */
/* sym is either a symmetry group or 0 meaning random or no symmetry   */
/* spec_xforms specifies the number of xforms to use, setting to 0 makes the number random. */
EXPORT void flam3_random(flam3_genome *g, int *ivars, int ivars_n, int sym, int spec_xforms);

void add_to_action(char *action, char *addtoaction);

EXPORT void flam3_mutate(flam3_genome *cp, int mutate_mode, int *ivars, int ivars_n, int sym, double speed, randctx *rc, char *action);
EXPORT void flam3_cross(flam3_genome *cp0, flam3_genome *cp1, flam3_genome *out, int cross_mode, randctx *rc, char *action);

/* return NULL in case of error */
EXPORT flam3_genome *flam3_parse_xml2(char *s, char *fn, int default_flag, int *ncps);
flam3_genome *flam3_parse_from_file(FILE *f, char *fn, int default_flag, int *ncps);

void flam3_add_symmetry(flam3_genome *g, int sym);

void flam3_improve_colors(flam3_genome *g, int ntries, int change_palette, int color_resolution);
EXPORT int flam3_colorhist(flam3_genome *cp, int num_batches, randctx *rc, double *hist);
EXPORT int flam3_estimate_bounding_box(flam3_genome *g, double eps, int nsamples,
             double *bmin, double *bmax, randctx *rc);
void flam3_rotate(flam3_genome *g, double angle, int interp_type); /* angle in degrees */

double flam3_dimension(flam3_genome *g, int ntries, int clip_to_camera);
double flam3_lyapunov(flam3_genome *g, int ntries);

void flam3_apply_template(flam3_genome *cp, flam3_genome *templ);

EXPORT int flam3_count_nthreads(void);

typedef struct {
//   double         temporal_filter_radius;
   double         pixel_aspect_ratio;    /* width over height of each pixel */
   flam3_genome  *genomes;
   int            ngenomes;
   int            verbose;
   int            bits;
   int            bytes_per_channel;
   int            earlyclip;
   double         time;
   int            (*progress)(void *, double, int, double);
   void          *progress_parameter;
   randctx       rc;
   int           nthreads;
   int           sub_batch_size;
} flam3_frame;


#define flam3_field_both  0
#define flam3_field_even  1
#define flam3_field_odd   2

/* out is pixel array.
   pixels are rgb or rgba if nchan is 3 or 4. */
EXPORT int flam3_render(flam3_frame *f, void *out, int field, int nchan, int transp, stat_struct *stats);

EXPORT double flam3_render_memory_required(flam3_frame *f);
EXPORT int flam3_make_strip(flam3_genome *cp, int nstrips, int stripnum);
void rotate_by(double *p, double *center, double by);


double flam3_random01();
double flam3_random11();
int flam3_random_bit();

/* ISAAC random numbers */
double flam3_random_isaac_01(randctx *);
double flam3_random_isaac_11(randctx *);
int flam3_random_isaac_bit(randctx *);

EXPORT void flam3_init_frame(flam3_frame *f);

/* External memory helpers */
EXPORT void *flam3_malloc(size_t size);
EXPORT void flam3_free(void *ptr);

void flam3_srandom();

flam3_genome *sheep_loop(flam3_genome *cp, double blend);
flam3_genome *sheep_edge(flam3_genome *cp, double blend, int seqflag, double stagger);

/* Motion function indices */
#define MOTION_SIN 1
#define MOTION_TRIANGLE 2
#define MOTION_HILL 3

/* Mutation modes */
#define MUTATE_NOT_SPECIFIED   -1
#define MUTATE_ALL_VARIATIONS  0
#define MUTATE_ONE_XFORM_COEFS 1
#define MUTATE_ADD_SYMMETRY    2
#define MUTATE_POST_XFORMS     3
#define MUTATE_COLOR_PALETTE   4
#define MUTATE_DELETE_XFORM    5
#define MUTATE_ALL_COEFS       6

/* Cross modes */
#define CROSS_NOT_SPECIFIED   -1
#define CROSS_UNION           0
#define CROSS_INTERPOLATE     1  
#define CROSS_ALTERNATE       2

/* Filters */
/* Spatial filter kernels */
#define flam3_gaussian_kernel 0
#define flam3_hermite_kernel 1
#define flam3_box_kernel 2
#define flam3_triangle_kernel 3
#define flam3_bell_kernel 4
#define flam3_b_spline_kernel 5
#define flam3_lanczos3_kernel 6
#define flam3_lanczos2_kernel 7
#define flam3_mitchell_kernel 8
#define flam3_blackman_kernel 9
#define flam3_catrom_kernel 10
#define flam3_hamming_kernel 11
#define flam3_hanning_kernel 12
#define flam3_quadratic_kernel 13

/* Temporal filters */
#define flam3_temporal_box 0
#define flam3_temporal_gaussian 1
#define flam3_temporal_exp 2


#endif
