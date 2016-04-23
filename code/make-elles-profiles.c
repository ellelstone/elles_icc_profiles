/* License:
 * 
 * Code for making well-behaved ICC profiles
 * Copyright © 2013, 2014, 2015, 2016 Elle Stone 
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * Contact information:
 * ellestone@ninedegreesbelow.com
 * http://ninedegreesbelow.com
 * 
 * */

/* About the ICC profile header "Platform" tag:
 * 
 * When creating a profile, LCMS checks to see if the platform is 
 * Windows ('MSFT'). If your platform isn't Windows, LCMS defaults 
 * to using the Apple ('APPL') platform tag for the profile header.
 * 
 * There is an unofficial Platform 
 * cmsPlatformSignature cmsSigUnices 0x2A6E6978 '*nix'. There is, 
 * however, no LCMS2 API for changing the platform when making a profile.
 * 
 * So on my own computer, to replace 'APPL' with '*nix' in the header, 
 * I modified the LCMS source file 'cmsio0.c' and recompiled LCMS:
 * #ifdef CMS_IS_WINDOWS_
 * Header.platform= (cmsPlatformSignature) _cmsAdjustEndianess32(cmsSigMicrosoft);
 * #else
 * Header.platform= (cmsPlatformSignature) _cmsAdjustEndianess32(cmsSigUnices);
 * #endif
 * 
 * */

/* Sample command line to compile this code:
 * 
 * gcc -g -O0 -Wall -o make-elles-profiles.exe make-elles-profiles.c -llcms2
 * 
 * 
 * */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lcms2.h>
#include "make-elles-profiles.h"

int main ()
{
printf("D50X, D50Y, D50Z = %1.8f %1.8f %1.8f\n", cmsD50X, cmsD50Y, cmsD50Z);
int i; /* for looping through the various TRCs */

/* *****************Set up V2_profile variables and values *************** */
cmsHPROFILE V4_profile;
cmsCIEXYZ media_whitepoint;
cmsCIEXYZ media_blackpoint = {0.0, 0.0, 0.0};
cmsCIExyY whitepoint;
cmsCIExyYTRIPLE primaries;
//cmsMLU *MfgDesc = cmsMLUalloc(NULL, 1);
cmsMLU *copyright = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(copyright, "en", "US", "Copyright 2016, Elle Stone (http://ninedegreesbelow.com/), CC-BY-SA 3.0 Unported (https://creativecommons.org/licenses/by-sa/3.0/legalcode).");
char *basename="", *trc="";
char *manufacturer="";
char *id="-elle", *extension=".icc";


/* ************************** WHITE POINTS ************************** */

/* D50 WHITE POINTS */
cmsCIExyY d50_romm_spec= {0.3457, 0.3585, 1.0};
cmsCIEXYZ d50_romm_spec_media_whitepoint = {0.964295676, 1.0, 0.825104603};
/* http://photo-lovers.org/pdf/color/romm.pdf */
cmsCIExyY d50_illuminant_specs = {0.345702915, 0.358538597, 1.0};
cmsCIEXYZ d50_illuminant_specs_media_whitepoint = {0.964199999, 1.000000000, 0.824899998};
/* calculated from D50 illuminant XYZ values in ICC specs */

/* D65 WHITE POINTS */
cmsCIExyY d65_srgb_adobe_specs = {0.3127, 0.3290, 1.0};
cmsCIEXYZ d65_media_whitepoint = {0.95045471, 1.0, 1.08905029};
/* calculated media whitepoint:  {0.950455927, 1.0, 1.089057751} */
/* White point from the sRGB.icm and AdobeRGB1998 profile specs:
 * http://www.adobe.com/digitalimag/pdfs/AdobeRGB1998.pdf 
 * 4.2.1 Reference Display White Point
 * The chromaticity coordinates of white displayed on 
 * the reference color monitor shall be x=0.3127, y=0.3290. 
 * . . . [which] correspond to CIE Standard Illuminant D65.
 * 
 * Wikipedia gives this same white point for SMPTE-C.
 * This white point is also given in the sRGB color space specs.
 * It's probably correct for most or all of the standard D65 profiles.
 * 
 * The D65 white point values used in the LCMS virtual sRGB profile
 * is slightly different than the D65 white point values given in the 
 * sRGB color space specs, so the LCMS virtual sRGB profile
 * doesn't match sRGB profiles made using the values given in the 
 * sRGB color space specs.
 * 
 * */
 
/* Various C and E WHITE POINTS */
/* cmsCIExyY c_astm  = {0.310060511, 0.316149551, 1.0}; 
see http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html */
cmsCIExyY e_astm  = {0.333333333, 0.333333333, 1.0};
cmsCIEXYZ e_astm_media_whitepoint = {1.0, 1.0, 1.0};
/* https://en.wikipedia.org/wiki/NTSC#Colorimetry
cmsCIExyY c_cie= {0.310, 0.316};
cmsCIExyY e_cie= {0.333, 0.333}; */
/* see http://en.wikipedia.org/wiki/Standard_illuminant#White_points_of_standard_illuminants
 * also see  http://www.brucelindbloom.com/index.html?Eqn_T_to_xy.html for the equations
cmsCIExyY c_6774_robertson= {0.308548930, 0.324928102, 1.0};
cmsCIExyY e_5454_robertson= {0.333608970, 0.348572909, 1.0}; */

/* ACES white point, taken from
 * Specification S-2014-004
 * ACEScg – A Working Space for CGI Render and Compositing
 */
cmsCIExyY d60_aces= {0.32168, 0.33767, 1.0};
cmsCIEXYZ d60_aces_media_whitepoint = {0.952646075, 1.0, 1.008825184};


/* ********************** MAKE THE PROFILES ************************* */

/* ACES PROFILES */

/* ***** Make profile: ACEScg, D60, gamma=1.00 */
/* ACEScg chromaticities taken from
 * Specification S-2014-004
 * ACEScg – A Working Space for CGI Render and Compositing
 * Version 1.0.1 April 24, 2015
 * http://www.oscars.org/science-technology/aces/aces-documentation
 */
cmsCIExyYTRIPLE aces_cg_primaries = 
{
{0.713, 0.293,  1.0},
{0.165, 0.830,  1.0},
{0.128, 0.044,  1.0}
};
whitepoint = d60_aces;
primaries = aces_cg_primaries;
media_whitepoint = d60_aces_media_whitepoint;
basename = "ACEScg";
manufacturer = "ACEScg chromaticities from S-2014-004 v1.0.1, http://www.oscars.org/science-technology/aces/aces-documentation";
//ModelDesc = "http://www.oscars.org/science-technology/aces/aces-documentation";
for ( i = 0; i < 6; i++ ) {
    if (i==0) trc="-g10";
    else if (i==1) trc="-g18";
    else if (i==2) trc="-g22";
    else if (i==3) trc="-srgbtrc";
    else if (i==4) trc="-labl";
    else if (i==5) trc="-rec709";
    V4_profile = make_V4_profile (whitepoint, primaries, trc, basename, id, 
                              extension, copyright, manufacturer);
    make_V2_profile (V4_profile, media_whitepoint, media_blackpoint, 
                 trc, basename, id, extension, copyright, manufacturer);
    }

/* ***** Make profile: ACES, D60, gamma=1.00 */
/* ACES chromaticities taken from
 * TB-2014-004, Version 1.0.1 April 24, 2015
 * Informative Notes on SMPTE ST 2065-1 – Academy
 * Color Encoding Specification (ACES)
 * http://www.oscars.org/science-technology/aces/aces-documentation
 * 
cmsCIExyYTRIPLE aces_primaries = 
{
{0.73470,  0.26530,  1.0},
{0.00000,  1.00000,  1.0},
{0.00010, -0.07700,  1.0}
}; */
cmsCIExyYTRIPLE aces_primaries_prequantized = 
{
{0.734704192222, 0.265298276252,  1.0},
{-0.000004945077, 0.999992850272,  1.0},
{0.000099889199, -0.077007518685,  1.0}
};
whitepoint = d60_aces;
primaries = aces_primaries_prequantized;
media_whitepoint = d60_aces_media_whitepoint;
basename = "ACES";
manufacturer = "ACES chromaticities from TB-2014-004, http://www.oscars.org/science-technology/aces/aces-documentation";
//ModelDesc = "http://www.oscars.org/science-technology/aces/aces-documentation";
for ( i = 0; i < 6; i++ ) {
    if (i==0) trc="-g10";
    else if (i==1) trc="-g18";
    else if (i==3) trc="-g22";
    else if (i==4) 
      {
        trc="-srgbtrc";
      }
    else if (i==5) 
      {
        trc="-labl";
        }
    else if (i==6) 
      {
        trc="-rec709";
      }
    V4_profile = make_V4_profile (whitepoint, primaries, trc, basename, id, 
                              extension, copyright, manufacturer);
    make_V2_profile (V4_profile, media_whitepoint, media_blackpoint, 
                 trc, basename, id, extension, copyright, manufacturer);
    }


/* ***** Make profile: AllColorsRGB, D50, gamma=1.00 */
/* AllColors.icc has a slightly larger color gamut than the ACES color 
 * space, holding some additional and "just barely visible" colors.
 * It has a D50 white point and a linear gamma TRC.
 * Just like the ACES color space, AllColors also holds a 
 * high percentage of imaginary colors.
 * See http://ninedegreesbelow.com/photography/xyz-rgb.html#xyY 
 * for more information about imaginary colors.
 * AllColors primaries for red and blue from 
 * http://www.ledtuning.nl/en/cie-convertor
 * blue 375nm red 780nm, plus Y intercepts:
 * Color Wavelength (): 375 nm.
 * Spectral Locus coordinates: X:0.17451 Y:0.005182
 * Color Wavelength (): 780 nm.
 * Spectral Locus coordinates: X:0.734690265 Y:0.265309735
 * X1:0.17451 Y1:0.005182
 * X2:0.734690265 Y2:0.265309735
 * X3:0.00Y3:? Solve for Y3:
 * (0.265309735-0.005182)/(0.734690265-0.17451)=0.46436433279205221554=m
 * y=mx+b let x=0; y=b
 * Y1=0.005182=(0.46436433279205221554*0.17451)+b
 * b=0.005182-(0.46436433279205221554*0.17451)=-.07585421971554103213
 *  */
cmsCIExyYTRIPLE allcolors_primaries = 
{
{0.734690265,  0.265309735,  1.0},
{0.000000000,  1.000000000,  1.0},
{0.000000000, -.0758542197,  1.0}
}; 
whitepoint = d50_illuminant_specs;
primaries = allcolors_primaries;
media_whitepoint = d50_illuminant_specs_media_whitepoint;
basename = "AllColorsRGB";
manufacturer = "AllColorsRGB chromaticities from http://ninedegreesbelow.com/photography/lcms-make-icc-profiles.html#AllColorsRGB";
//ModelDesc = "http://ninedegreesbelow.com/photography/lcms-make-icc-profiles.html#AllColorsRGB";
for ( i = 0; i < 6; i++ ) {
    if (i==0) trc="-g10";
    else if (i==1) trc="-g18";
    else if (i==2) trc="-g22";
    else if (i==3) trc="-srgbtrc";
    else if (i==4) trc="-labl";
    else if (i==5) trc="-rec709";
    V4_profile = make_V4_profile (whitepoint, primaries, trc, basename, id, 
                              extension, copyright, manufacturer);
    make_V2_profile (V4_profile, media_whitepoint, media_blackpoint, 
                 trc, basename, id, extension, copyright, manufacturer);
    }


/* ***** Make profile: Identity, D50, gamma=1.00. */
/* These primaries also hold all possible visible colors, 
 * but less efficiently than the ACES or AllColors profile.*/
cmsCIExyYTRIPLE identity_primaries = {/*  */
{1.0, 0.0, 1.0},
{0.0, 1.0, 1.0},
{0.0, 0.0, 1.0}
};
whitepoint = d50_illuminant_specs;
primaries = identity_primaries;
media_whitepoint = d50_illuminant_specs_media_whitepoint;
basename = "IdentityRGB";
manufacturer = "A discussion of the Identity profile primaries can be found here: http://ninedegreesbelow.com/photography/xyz-rgb.html#ICC";
//ModelDesc = "";
for ( i = 0; i < 6; i++ ) {
    if (i==0) trc="-g10";
    else if (i==1) trc="-g18";
    else if (i==2) trc="-g22";
    else if (i==3) trc="-srgbtrc";
    else if (i==4) trc="-labl";
    else if (i==5) trc="-rec709";
    V4_profile = make_V4_profile (whitepoint, primaries, trc, basename, id, 
                              extension, copyright, manufacturer);
    make_V2_profile (V4_profile, media_whitepoint, media_blackpoint, 
                 trc, basename, id, extension, copyright, manufacturer);
    }


/* ***** Make profile: Romm/Prophoto, D50, gamma=1.80 */
/* Reference Input/Output Medium Metric RGB Color Encodings (RIMM/ROMM RGB)
 * Kevin E. Spaulding, Geoffrey J. Woolfe and Edward J. Giorgianni
 * Eastman Kodak Company, Rochester, New York, U.S.A.
 * Above document is available at http://photo-lovers.org/pdf/color/romm.pdf
 * Kodak designed the Romm (ProPhoto) color gamut to include all printable 
 * and most real world colors. It includes some imaginary colors and excludes 
 * some of the real world blues and violet blues that can be captured by 
 * digital cameras. For high bit depth image editing only.
 * 
 * Note: the Kodak specs use values for the profile D50 illuminant that 
 * don't match the profile D50 illuminant given in the ICC specs. LCMS
 * makes profiles using the D50 illuminant values that are given in the 
 * ICC specs.
 */
cmsCIExyYTRIPLE romm_primaries = {
{0.7347, 0.2653, 1.0},
{0.1596, 0.8404, 1.0},
{0.0366, 0.0001, 1.0}
};
primaries = romm_primaries;
whitepoint = d50_romm_spec;
media_whitepoint = d50_romm_spec_media_whitepoint;
basename = "LargeRGB";
manufacturer = "LargeRGB chromaticities from Reference Input/Output Medium Metric RGB Color Encodings (RIMM/ROMM RGB), http://photo-lovers.org/pdf/color/romm.pdf";
//ModelDesc = "";
for ( i = 0; i < 6; i++ ) {
    if (i==0) trc="-g10";
    else if (i==1) trc="-g18";
    else if (i==2) trc="-g22";
    else if (i==3) trc="-srgbtrc";
    else if (i==4) trc="-labl";
    else if (i==5) trc="-rec709";
    V4_profile = make_V4_profile (whitepoint, primaries, trc, basename, id, 
                              extension, copyright, manufacturer);
    make_V2_profile (V4_profile, media_whitepoint, media_blackpoint, 
                 trc, basename, id, extension, copyright, manufacturer);
    }


/* ***** Make profile: WidegamutRGB, D50, gamma=2.19921875 */
/* Pascale's primary values produce a profile that matches 
 * old V2 Widegamut profiles from Adobe and Canon.
 * Danny Pascale: A review of RGB color spaces
 * http://www.babelcolor.com/download/A%20review%20of%20RGB%20color%20spaces.pdf  
 * WideGamutRGB was designed by Adobe to be a wide gamut color space that uses
 * spectral colors as its primaries. For high bit depth image editing only. */
cmsCIExyYTRIPLE widegamut_pascale_primaries = {
{0.7347, 0.2653, 1.0},
{0.1152, 0.8264, 1.0},
{0.1566, 0.0177, 1.0}
};
primaries = widegamut_pascale_primaries;
whitepoint = d50_romm_spec;
media_whitepoint = d50_romm_spec_media_whitepoint;
basename = "WideRGB";
manufacturer = "WideRGB chromaticities from Danny Pascale: A review of RGB color spaces, http://www.babelcolor.com/download/A%20review%20of%20RGB%20color%20spaces.pdf";
//ModelDesc = "";


/* ***** Make profile: ClayRGB (AdobeRGB), D65, gamma=2.19921875 */
/* The Adobe RGB 1998 color gamut covers a higher percentage of 
 * real-world greens than sRGB, but still doesn't include all printable 
 * greens, yellows, and cyans. 
 * When made using the gamma=2.19921875 tone response curve, 
 * this profile can be used for 8-bit image editing 
 * if used with appropriate caution to avoid posterization. 
 * When made with the gamma=2.19921875 tone response curve
 * this profile can be applied to DCF R98 camera-generated jpegs.
 *
cmsCIExyYTRIPLE adobe_primaries = {
{0.6400, 0.3300, 1.0},
{0.2100, 0.7100, 1.0},
{0.1500, 0.0600, 1.0}
}; */
cmsCIExyYTRIPLE adobe_primaries_prequantized = {
{0.639996511, 0.329996864, 1.0},
{0.210005295, 0.710004866, 1.0},
{0.149997606, 0.060003644, 1.0}
};
primaries = adobe_primaries_prequantized;
whitepoint = d65_srgb_adobe_specs;
media_whitepoint = d65_media_whitepoint;
basename = "ClayRGB";
manufacturer = "ClayRGB chromaticities as given in Adobe RGB (1998) Color Image Encoding, Version 2005-05, https://www.adobe.com/digitalimag/pdfs/AdobeRGB1998.pdf";
//ModelDesc = "";
for ( i = 0; i < 6; i++ ) {
    if (i==0) trc="-g10";
    else if (i==1) trc="-g18";
    else if (i==2) trc="-g22";
    else if (i==3) trc="-srgbtrc";
    else if (i==4) trc="-labl";
    else if (i==5) trc="-rec709";
    V4_profile = make_V4_profile (whitepoint, primaries, trc, basename, id, 
                              extension, copyright, manufacturer);
    make_V2_profile (V4_profile, media_whitepoint, media_blackpoint, 
                 trc, basename, id, extension, copyright, manufacturer);
    }


/* ***** Make profile: Rec.2020, D65, Rec709 TRC */
/* https://en.wikipedia.org/wiki/Rec._2020
 * https://www.itu.int/dms_pub/itu-r/opb/rep/R-REP-BT.2246-2-2012-PDF-E.pdf
cmsCIExyYTRIPLE rec2020_primaries = {
{0.7079, 0.2920, 1.0},
{0.1702, 0.7965, 1.0},
{0.1314, 0.0459, 1.0}
};
https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.2020-2-201510-I!!PDF-E.pdf
cmsCIExyYTRIPLE rec2020_primaries = {
{0.708, 0.292, 1.0},
{0.170, 0.797, 1.0},
{0.131, 0.046, 1.0}
};
I used the first set of primaries given above, but after 
hexadecimal quantization the two sets of primaries 
seem to produce the same profile colorants.
*/
cmsCIExyYTRIPLE rec2020_primaries_prequantized = {
{0.708012540607, 0.291993664388, 1.0},
{0.169991652439, 0.797007778423, 1.0},
{0.130997824007, 0.045996550894, 1.0}
};
primaries = rec2020_primaries_prequantized;
whitepoint = d65_srgb_adobe_specs;
media_whitepoint = d65_media_whitepoint;
basename = "Rec2020";
manufacturer = "Rec2020 chromaticities from https://www.itu.int/dms_pub/itu-r/opb/rep/R-REP-BT.2246-2-2012-PDF-E.pdf; https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.2020-2-201510-I!!PDF-E.pdf";
//ModelDesc = "";
for ( i = 0; i < 6; i++ ) {
    if (i==0) trc="-g10";
    else if (i==1) trc="-g18";
    else if (i==2) trc="-g22";
    else if (i==3) trc="-srgbtrc";
    else if (i==4) trc="-labl";
    else if (i==5) trc="-rec709";
    V4_profile = make_V4_profile (whitepoint, primaries, trc, basename, id, 
                              extension, copyright, manufacturer);
    make_V2_profile (V4_profile, media_whitepoint, media_blackpoint, 
                 trc, basename, id, extension, copyright, manufacturer);
    }


/* ***** Make profile: sRGB, D65, sRGB TRC */
/* http://en.wikipedia.org/wiki/Srgb */
/* Hewlett-Packard and Microsoft designed sRGB to match 
 * the color gamut of consumer-grade CRTs from the 1990s
 * and to be the standard color space for the world wide web.
 * When made using the standard sRGB TRC, this sRGB profile 
 * can be applied to DCF R03 camera-generated jpegs and 
 * is an excellent color space for editing 8-bit images.
 * When made using the linear gamma TRC, the resulting profile
 * should only be used for high bit depth image editing.
 *
cmsCIExyYTRIPLE srgb_primaries = {
{0.6400, 0.3300, 1.0},
{0.3000, 0.6000, 1.0},
{0.1500, 0.0600, 1.0}
}; */
cmsCIExyYTRIPLE srgb_primaries_pre_quantized = {
{0.639998686, 0.330010138, 1.0},
{0.300003784, 0.600003357, 1.0},
{0.150002046, 0.059997204, 1.0}
};
primaries = srgb_primaries_pre_quantized;
whitepoint = d65_srgb_adobe_specs;
media_whitepoint = d65_media_whitepoint;
basename = "sRGB";
manufacturer = "sRGB chromaticities from A Standard Default Color Space for the Internet - sRGB, http://www.w3.org/Graphics/Color/sRGB; also see http://www.color.org/specification/ICC1v43_2010-12.pdf";
//ModelDesc = "";
for ( i = 0; i < 6; i++ ) {
    if (i==0) trc="-g10";
    else if (i==1) trc="-g18";
    else if (i==2) trc="-g22";
    else if (i==3) trc="-srgbtrc";
    else if (i==4) trc="-labl"; 
    else if (i==5) {
      trc="-rec709"; 
      basename="Rec709";
      manufacturer="Rec709 chromaticities from Recommendation ITU-R BT.709-6 (06/2015), http://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.709-6-201506-I!!PDF-E.pdf";
      }
    V4_profile = make_V4_profile (whitepoint, primaries, trc, basename, id, 
                              extension, copyright, manufacturer);
    make_V2_profile (V4_profile, media_whitepoint, media_blackpoint, 
                 trc, basename, id, extension, copyright, manufacturer);
    }


/* ***** Make profile: CIE-RGB profile, E white point*/
/* The ASTM E white point is probably the right white point 
 * to use when making the CIE-RGB color space profile.
 * It's not clear to me what the correct CIE-RGB primaries really are.
 * Lindbloom gives one set. The LCMS version 1 tutorial gives a different set.
 * I asked a friend to ask an expert, who said the real primaries should
 * be calculated from the spectral wavelengths. 
 * Two sets of primaries are given below:
 * */
/* This page explains what the CIE color space is:
 * https://en.wikipedia.org/wiki/CIE_1931
 * These pages give the wavelengths:
 * http://hackipedia.org/Color%20space/pdf/CIE%20Color%20Space.pdf 
 * http://infocom.ing.uniroma1.it/~gaetano/texware/Full-How%20the%20CIE%201931%20Color-Matching%20Functions%20Were%20Derived%20from%20Wright-Guild%20Data.pdf
 * This page has resources for calculating xy values given a spectral color wavelength:
 * http://www.cvrl.org/cmfs.htm
 * This page does the calculations for you:
 * http://www.ledtuning.nl/cie.php
 * Plugging the wavelengths into the ledtuning website 
 * gives the following CIE RGB xy primaries:
700.0 nm has Spectral Locus coordinates: x:0.734690023  y:0.265309977
546.1 nm has Spectral Locus coordinates: x:0.2736747378 y:0.7174284409
435.8 nm has Spectral Locus coordinates: x:0.1665361196 y:0.0088826412
*
cmsCIExyYTRIPLE cie_primaries_ledtuning = {
{0.7346900230, 0.2653099770, 1.0},
{0.2736747378, 0.7174284409, 1.0},
{0.1665361196, 0.0088826412, 1.0}
}; */
/* Assuming you want to use the ASTM values for the E white point, 
 * here are the prequantized ledtuning primaries */
cmsCIExyYTRIPLE cie_primaries_ledtuning_prequantized = {
{0.734689082, 0.265296653, 1.0},
{0.273673341, 0.717437354, 1.0},
{0.166531028, 0.008882428, 1.0}
};
primaries = cie_primaries_ledtuning_prequantized;
whitepoint = e_astm;
media_whitepoint = e_astm_media_whitepoint;
basename = "CIERGB";
manufacturer = "A discussion of the CIERGB chromaticities can be found at http://ninedegreesbelow.com/photography/lcms-make-icc-profiles.html#CIERGB";
//ModelDesc = "";
for ( i = 0; i < 6; i++ ) {
    if (i==0) trc="-g10";
    else if (i==1) trc="-g18";
    else if (i==2) trc="-g22";
    else if (i==3) trc="-srgbtrc";
    else if (i==4) trc="-labl";
    else if (i==5) trc="-rec709";
    V4_profile = make_V4_profile (whitepoint, primaries, trc, basename, id, 
                              extension, copyright, manufacturer);
    make_V2_profile (V4_profile, media_whitepoint, media_blackpoint, 
                 trc, basename, id, extension, copyright, manufacturer);
    }


/* **************************** Make Gray ICC profiles ************** */
whitepoint = d50_illuminant_specs;
media_whitepoint = d50_illuminant_specs_media_whitepoint;
basename = "Gray";
for ( i = 0; i < 6; i++ ) 
    {
    if (i==0) trc="-g10";
    else if (i==1) trc="-g18";
    else if (i==2) trc="-g22";
    else if (i==3) trc="-srgbtrc";
    else if (i==4) trc="-labl";
    else if (i==5) trc="-rec709";
    make_gray_profile (whitepoint, trc, basename, id, extension, 
                       copyright, media_whitepoint, media_blackpoint
                       );
    } 


/* *********** Make LCMS built-in LAB and XYZ profiles ************** */
whitepoint = d50_illuminant_specs;
make_LAB_XYZ_profiles (whitepoint, copyright);


/* make gcc happy by returning an integer from main() */
return 0;
}

static void make_gray_profile (cmsCIExyY       whitepoint,
                               char *          trc,
                               char *          basename,
                               char *          id,
                               char *          extension,
                               cmsMLU          *copyright,
                               cmsCIEXYZ       media_whitepoint,
                               cmsCIEXYZ       media_blackpoint
                               )
{
cmsToneCurve *grayTRC, *tonecurve;
tonecurve = make_tonecurve (trc);
grayTRC= tonecurve;

/* Make V4 gray profile */
cmsHPROFILE profile = cmsCreateGrayProfile ( &whitepoint, grayTRC );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
cmsWriteTag (profile, cmsSigMediaWhitePointTag, &media_whitepoint);
char* profile_version="-V4";
char *filename = make_file_name (basename, id, profile_version, trc, extension);
cmsMLU *description;
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", filename);
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

/* Make V2 gray profile */
profile_version="-trueV2";
filename = make_file_name (basename, id, profile_version, trc, extension);
cmsSetProfileVersion (profile, 2.2);
cmsWriteTag (profile, cmsSigMediaBlackPointTag, &media_blackpoint);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", filename);
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
}


static cmsHPROFILE make_V4_profile (cmsCIExyY       whitepoint,
                                    cmsCIExyYTRIPLE primaries,
                                    char *          trc,
                                    char *          basename,
                                    char *          id,
                                    char *          extension,
                                    cmsMLU          *copyright,
                                    char *          manufacturer
                                    )
{
cmsToneCurve *curve[3], *tonecurve;
tonecurve = make_tonecurve (trc);
curve[0] = curve[1] = curve[2] = tonecurve;

/* Make V4 profile */
cmsHPROFILE V4_profile = cmsCreateRGBProfile ( &whitepoint, &primaries, curve);

cmsWriteTag(V4_profile, cmsSigCopyrightTag, copyright);

cmsMLU *MfgDesc;
MfgDesc   = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(MfgDesc, "en", "US", manufacturer);
cmsWriteTag(V4_profile, cmsSigDeviceMfgDescTag, MfgDesc);

char* profile_version="-V4";
char *filename = make_file_name (basename, id, profile_version, trc, extension);
cmsMLU *description;
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", filename);
cmsWriteTag(V4_profile, cmsSigProfileDescriptionTag, description);
cmsSaveProfileToFile(V4_profile, filename);
cmsMLUfree(description);
cmsMLUfree(MfgDesc);
cmsBool cmsMD5computeID(cmsHPROFILE hProfile);
return V4_profile;
}


static cmsToneCurve* make_tonecurve (char * trc)
{ //printf("%s\n", trc);
  cmsToneCurve *tonecurve;
  if (strcmp( trc, "-g10") == 0)
  tonecurve = cmsBuildGamma (NULL, 1.00);
  
  else if (strcmp( trc, "-g18") == 0) 
  tonecurve = cmsBuildGamma (NULL, 1.80078125); 
  
  else if (strcmp( trc, "-g22") == 0)
  tonecurve = cmsBuildGamma (NULL, 2.19921875);

  else if (strcmp( trc, "-srgbtrc") == 0) {
  cmsFloat64Number srgb_parameters[5] = 
  { 2.4, 1.0 / 1.055,  0.055 / 1.055, 1.0 / 12.92, 0.04045 };
  tonecurve =  
  cmsBuildParametricToneCurve(NULL, 4, srgb_parameters);
  }

  else if (strcmp( trc, "-rec709") == 0) { 
  cmsFloat64Number rec709_parameters[5] = 
  { 1.0 / 0.45, 1.0 / 1.099,  0.099 / 1.099,  1.0 / 4.5, 0.081 };  
  tonecurve = 
  cmsBuildParametricToneCurve(NULL, 4, rec709_parameters);
  }

  else if (strcmp( trc, "-labl") == 0) {
  cmsFloat64Number labl_parameters[5] = 
  { 3.0, 1.0 / 1.16,  0.16 / 1.16, 2700.0 / 24389.0, 0.08000 };
  tonecurve = 
  cmsBuildParametricToneCurve(NULL, 4, labl_parameters);
  }

  return tonecurve;
}


static cmsHPROFILE make_V2_profile (cmsHPROFILE  V4_profile,
                                    cmsCIEXYZ    media_whitepoint,
                                    cmsCIEXYZ    media_blackpoint,
                                    char *          trc,
                                    char *          basename,
                                    char *          id,
                                    char *          extension,
                                    cmsMLU          *copyright,
                                    char *          manufacturer
                                    )
{
/* Open sample V2 profile */
char *sample_profile_filename;
sample_profile_filename = "sampleV2.icm";

if (strcmp(trc, "-srgbtrc") == 0 ) sample_profile_filename = "sampleV2srgb.icm";
if (strcmp(trc, "-labl") == 0 )    sample_profile_filename = "sampleV2labl.icm";
if (strcmp(trc, "-rec709") == 0 )  sample_profile_filename = "sampleV2rec709.icm";

cmsHPROFILE sampleV2 = cmsOpenProfileFromFile(sample_profile_filename, "r");

char *profile_version="-trueV2";
char *filename;
cmsHPROFILE V2_profile = sampleV2;
cmsSetProfileVersion (V2_profile, 2.2);
cmsSetDeviceClass (V2_profile, cmsSigDisplayClass);
cmsSetPCS (V2_profile, cmsSigXYZData);
cmsWriteTag (V2_profile, cmsSigMediaWhitePointTag, &media_whitepoint);
cmsWriteTag (V2_profile, cmsSigMediaBlackPointTag, &media_blackpoint);

/* Get and set the colorants */
cmsCIEXYZ  *red = cmsReadTag(V4_profile, cmsSigRedColorantTag); 
cmsCIEXYZ red_tag = *red;
cmsWriteTag (V2_profile, cmsSigRedColorantTag, &red_tag);

cmsCIEXYZ  *green = cmsReadTag(V4_profile, cmsSigGreenColorantTag); 
cmsCIEXYZ green_tag = *green;
cmsWriteTag (V2_profile, cmsSigGreenColorantTag, &green_tag);

cmsCIEXYZ  *blue = cmsReadTag(V4_profile, cmsSigBlueColorantTag); 
cmsCIEXYZ blue_tag = *blue;
cmsWriteTag (V2_profile, cmsSigBlueColorantTag, &blue_tag);

/* Get and set the TRCs */

if ( (strcmp( trc, "-g10") == 0) || 
     (strcmp( trc, "-g18") == 0) || 
     (strcmp( trc, "-g22") == 0) )
{
cmsToneCurve  *red_trc = cmsReadTag(V4_profile, cmsSigRedTRCTag);
cmsWriteTag (V2_profile, cmsSigRedTRCTag, red_trc);

cmsToneCurve  *green_trc = cmsReadTag(V4_profile, cmsSigGreenTRCTag);
cmsWriteTag (V2_profile, cmsSigGreenTRCTag, green_trc);

cmsToneCurve  *blue_trc = cmsReadTag(V4_profile, cmsSigBlueTRCTag); 
cmsWriteTag (V2_profile, cmsSigBlueTRCTag, blue_trc);
}

/* Set copyright, manufacturer, and ModelDesc tags */
cmsWriteTag(V2_profile, cmsSigCopyrightTag, copyright);

cmsMLU *MfgDesc;
MfgDesc   = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(MfgDesc, "en", "US", manufacturer);
cmsWriteTag(V2_profile, cmsSigDeviceMfgDescTag, MfgDesc);

/* Set filename and description */
filename = make_file_name (basename, id, profile_version, trc, extension);

cmsMLU *description;
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", filename);
cmsWriteTag(V2_profile, cmsSigProfileDescriptionTag, description);
cmsSaveProfileToFile(V2_profile, filename);
cmsMLUfree(description);
cmsMLUfree(MfgDesc);
cmsBool cmsMD5computeID(cmsHPROFILE hProfile);
return 0;
}


static char* make_file_name (char* basename, 
                            char* id, 
                            char* profile_version, 
                            char* trc, 
                            char* extension
                            )
{
char* filename="";
char* path="../profiles/";
int i = 0;

if ( strcmp(basename, "-sRGB")==0 && strcmp(trc, "-rec709")==0 ) basename="Rec709";

i =
		strlen(path) +
		strlen(basename) +
		strlen(id) +
		strlen(profile_version) +
		strlen(trc) +
		strlen(extension);
filename = (char*) malloc (i + 1); 


strcpy(filename, path);
strcat(filename, basename);
strcat(filename, id);
strcat(filename, profile_version);
strcat(filename, trc);
strcat(filename, extension);

return filename;
}


static void make_LAB_XYZ_profiles (cmsCIExyY whitepoint, 
                                   cmsMLU *copyright
                                   )
{
/* Based on transicc output, the V4 profiles 
 * can be used in unbounded mode, but the V2 versions cannot. */
cmsHPROFILE profile;
cmsMLU *description;
char *filename;

profile  = cmsCreateLab2Profile(&whitepoint);
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Lab-D50-Identity-elle-V2.icc");
                             filename = "../profiles/Lab-D50-Identity-elle-V2.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

profile  = cmsCreateLab4Profile(&whitepoint);
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Lab-D50-Identity-elle-V4.icc");
                             filename = "../profiles/Lab-D50-Identity-elle-V4.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

profile = cmsCreateXYZProfile();
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "XYZ-D50-Identity-elle-V4.icc");
                             filename = "../profiles/XYZ-D50-Identity-elle-V4.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
}
