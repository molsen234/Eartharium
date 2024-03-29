/*
Module : AAPhysicalSun.h
Purpose: Implementation for the algorithms which obtain the physical parameters of the Sun
Created: PJN / 29-12-2003

Copyright (c) 2003 - 2021 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef __AAPHYSICALSUN_H__
#define __AAPHYSICALSUN_H__

#ifndef AAPLUS_EXT_CLASS
#define AAPLUS_EXT_CLASS
#endif //#ifndef AAPLUS_EXT_CLASS


//////////////////////////// Classes //////////////////////////////////////////

class AAPLUS_EXT_CLASS CAAPhysicalSunDetails
{
public:
//Constructors / Destructors
  CAAPhysicalSunDetails() noexcept : P(0),
                                     B0(0),
                                     L0(0)
  {
  };
  CAAPhysicalSunDetails(const CAAPhysicalSunDetails&) = default;
  CAAPhysicalSunDetails(CAAPhysicalSunDetails&&) = default;
  ~CAAPhysicalSunDetails() = default;

//Methods
  CAAPhysicalSunDetails& operator=(const CAAPhysicalSunDetails&) = default;
  CAAPhysicalSunDetails& operator=(CAAPhysicalSunDetails&&) = default;

//Member variables
  double P;
  double B0;
  double L0;
};

class AAPLUS_EXT_CLASS CAAPhysicalSun
{
public:
//Static methods
  static CAAPhysicalSunDetails Calculate(double JD, bool bHighPrecision) noexcept;
  static double TimeOfStartOfRotation(long C) noexcept;
};


#endif //#ifndef __AAPHYSICALSUN_H__
