/*
Module : AAPhysicalMoon.h
Purpose: Implementation for the algorithms which obtain the physical parameters of the Moon
Created: PJN / 17-01-2004

Copyright (c) 2004 - 2021 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////// Macros / Defines //////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif //#if _MSC_VER > 1000

#ifndef __AAPHYSICALMOON_H__
#define __AAPHYSICALMOON_H__

#ifndef AAPLUS_EXT_CLASS
#define AAPLUS_EXT_CLASS
#endif //#ifndef AAPLUS_EXT_CLASS


/////////////////////// Includes //////////////////////////////////////////////

#include "AACoordinateTransformation.h"


/////////////////////// Classes ///////////////////////////////////////////////

class AAPLUS_EXT_CLASS CAAPhysicalMoonDetails
{
public:
//Constructors / Destructors
  CAAPhysicalMoonDetails() noexcept : ldash(0),
                                      bdash(0),
                                      ldash2(0),
                                      bdash2(0),
                                      l(0),
                                      b(0),
                                      P(0)
  {
  };
  CAAPhysicalMoonDetails(const CAAPhysicalMoonDetails&) = default;
  CAAPhysicalMoonDetails(CAAPhysicalMoonDetails&&) = default;
  ~CAAPhysicalMoonDetails() = default;

//Methods
  CAAPhysicalMoonDetails& operator=(const CAAPhysicalMoonDetails&) = default;
  CAAPhysicalMoonDetails& operator=(CAAPhysicalMoonDetails&&) = default;

//Member variables
  double ldash;
  double bdash;
  double ldash2;
  double bdash2;
  double l;
  double b;
  double P;
};

class AAPLUS_EXT_CLASS CAASelenographicMoonDetails
{
public:
//Constructors / Destructors
  CAASelenographicMoonDetails() noexcept : l0(0),
                                           b0(0),
                                           c0(0)
  {
  };
  CAASelenographicMoonDetails(const CAASelenographicMoonDetails&) = default;
  CAASelenographicMoonDetails(CAASelenographicMoonDetails&&) = default;
  ~CAASelenographicMoonDetails() = default;

//Methods
  CAASelenographicMoonDetails& operator=(const CAASelenographicMoonDetails&) = default;
  CAASelenographicMoonDetails& operator=(CAASelenographicMoonDetails&&) = default;

//Member variables
  double l0;
  double b0;
  double c0;
};

class AAPLUS_EXT_CLASS CAAPhysicalMoon
{
public:
//Static methods
  static CAAPhysicalMoonDetails CalculateGeocentric(double JD) noexcept;
  static CAAPhysicalMoonDetails CalculateTopocentric(double JD, double Longitude, double Latitude) noexcept;
  static CAASelenographicMoonDetails CalculateSelenographicPositionOfSun(double JD, bool bHighPrecision) noexcept;
  static double AltitudeOfSun(double JD, double Longitude, double Latitude, bool bHighPrecision) noexcept;
  static double TimeOfSunrise(double JD, double Longitude, double Latitude, bool bHighPrecision) noexcept;
  static double TimeOfSunset(double JD, double Longitude, double Latitude, bool bHighPrecision) noexcept;

protected:
  static double SunriseSunsetHelper(double JD, double Longitude, double Latitude, bool bSunrise, bool bHighPrecision) noexcept;
  static CAAPhysicalMoonDetails CalculateHelper(double JD, double& Lambda, double& Beta, double& epsilon, CAA2DCoordinate& Equatorial) noexcept;
  static void CalculateOpticalLibration(double JD, double Lambda, double Beta, double& ldash, double& bdash, double& ldash2, double& bdash2, double& epsilon, double& omega, double& DeltaU, double& sigma, double& I, double& rho) noexcept;
};


#endif //#ifndef __AAPHYSICALMOON_H__
