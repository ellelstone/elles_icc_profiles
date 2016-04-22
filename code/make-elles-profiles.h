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

static cmsHPROFILE make_V2_profile (cmsHPROFILE  V4_profile,
                                    cmsCIEXYZ    media_whitepoint,
                                    cmsCIEXYZ    media_blackpoint,
                                    char *          trc,
                                    char *          basename,
                                    char *          id,
                                    char *          extension,
                                    cmsMLU          *copyright,
                                    char *          manufacturer
                                    );

static char* make_file_name (char* basename,
                             char* id,
                             char* profile_version,
                             char* trc,
                             char* extension
                             );

static cmsHPROFILE make_V4_profile (cmsCIExyY       whitepoint,
                                    cmsCIExyYTRIPLE primaries,
                                    char *          trc,
                                    char *          basename,
                                    char *          id,
                                    char *          extension,
                                    cmsMLU          *copyright,
                                    char *          manufacturer
                                    );

static void make_LAB_XYZ_profiles (cmsCIExyY whitepoint, 
                                   cmsMLU *copyright
                                   );

static cmsToneCurve* make_tonecurve (char * trc);

static void make_gray_profile (cmsCIExyY       whitepoint,
                               char *          trc,
                               char *          basename,
                               char *          id,
                               char *          extension,
                               cmsMLU          *copyright,
                               cmsCIEXYZ       media_whitepoint,
                               cmsCIEXYZ       media_blackpoint
                               );

/*
iccFromXml sampleV2srgb.xml sampleV2srgb.icm
iccFromXml sampleV2rec709.xml sampleV2rec709.icm
iccFromXml sampleV2labl.xml sampleV2labl.icm
iccFromXml sampleV2.xml sampleV2.icm

rm r *.icc*
rm make-elles-profiles

*/
