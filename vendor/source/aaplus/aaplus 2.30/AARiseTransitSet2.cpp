/*
Module : AARiseTransitSet2.cpp
Purpose: Implementation for the algorithms which obtain the Rise, Transit and Set times (revised version)
Created: PJN / 29-12-2003
History: PJN / 13-07-2019 1. Initial implementation
         PJN / 15-07-2019 1. Refactored the code in various CAARiseTransitSet2 methods and improved the 
                          interpolation code to provide better accuracy of event details.
         PJN / 08-09-2019 1. Added support for EndCivilTwilight, EndNauticalTwilight, EndAstronomicalTwilight,
                          StartAstronomicalTwilight, StartNauticalTwilight & StartCivilTwilight event types

Copyright (c) 2019 - 2021 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


///////////////////////////// Includes ////////////////////////////////////////

#include "stdafx.h"
#include "AARiseTransitSet2.h"
#include "AACoordinateTransformation.h"
#include "AAEarth.h"
#include "AAElliptical.h"
#include "AAParallax.h"
#include "AASidereal.h"
#include "AASun.h"
#include "AAMoon.h"
#include "AANutation.h"
#include <cmath>
#include <cassert>
using namespace std;


///////////////////////////// Implementation //////////////////////////////////

void CAARiseTransitSet2::AddEvents(vector<CAARiseTransitSetDetails2>& events, double LastAltitudeForDetectingRiseSet, double AltitudeForDetectingRiseSet,
                                   double LastAltitudeForInterpolation, double h0, const CAA2DCoordinate& Horizontal, double LastJD, double StepInterval, double LastBearing, 
                                   Object object, double LastAltitudeForDetectingTwilight, double AltitudeForTwilight)
{
  if ((object == Object::SUN) && (LastAltitudeForDetectingTwilight != -90))
  {
    if ((LastAltitudeForDetectingTwilight < -18) && (AltitudeForTwilight >= -18)) //We have just ended astronomical twilight
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::EndAstronomicalTwilight;
      const double fraction = (-18 - LastAltitudeForInterpolation) / (Horizontal.Y - LastAltitudeForInterpolation);
      event.JD = LastJD + (fraction * StepInterval);

      //Ensure the LastBearing and Horizontal.X values are correct for interpolation
      double LastBearing2 = LastBearing;
      double HorizontalX2 = Horizontal.X;
      if (fabs(HorizontalX2 - LastBearing2) > 180)
      {
        if (HorizontalX2 > LastBearing2)
          LastBearing2 += 360;
        else
          HorizontalX2 += 360;
      }

      event.Bearing = CAACoordinateTransformation::MapTo0To360Range(LastBearing2 + (fraction * (HorizontalX2 - LastBearing2)));
      events.push_back(event);
    }
    else if ((LastAltitudeForDetectingTwilight < -12) && (AltitudeForTwilight >= -12)) //We have just ended nautical twilight
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::EndNauticalTwilight;
      const double fraction = (-12 - LastAltitudeForInterpolation) / (Horizontal.Y - LastAltitudeForInterpolation);
      event.JD = LastJD + (fraction * StepInterval);

      //Ensure the LastBearing and Horizontal.X values are correct for interpolation
      double LastBearing2 = LastBearing;
      double HorizontalX2 = Horizontal.X;
      if (fabs(HorizontalX2 - LastBearing2) > 180)
      {
        if (HorizontalX2 > LastBearing2)
          LastBearing2 += 360;
        else
          HorizontalX2 += 360;
      }

      event.Bearing = CAACoordinateTransformation::MapTo0To360Range(LastBearing2 + (fraction * (HorizontalX2 - LastBearing2)));
      events.push_back(event);
    }
    else if ((LastAltitudeForDetectingTwilight < -6) && (AltitudeForTwilight >= -6)) //We have just ended civil twilight
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::EndCivilTwilight;
      const double fraction = (-6 - LastAltitudeForInterpolation) / (Horizontal.Y - LastAltitudeForInterpolation);
      event.JD = LastJD + (fraction * StepInterval);

      //Ensure the LastBearing and Horizontal.X values are correct for interpolation
      double LastBearing2 = LastBearing;
      double HorizontalX2 = Horizontal.X;
      if (fabs(HorizontalX2 - LastBearing2) > 180)
      {
        if (HorizontalX2 > LastBearing2)
          LastBearing2 += 360;
        else
          HorizontalX2 += 360;
      }

      event.Bearing = CAACoordinateTransformation::MapTo0To360Range(LastBearing2 + (fraction * (HorizontalX2 - LastBearing2)));
      events.push_back(event);
    }
    else if ((LastAltitudeForDetectingTwilight > -18) && (AltitudeForTwilight <= -18)) //We have just started astronomical twilight
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::StartAstronomicalTwilight;
      const double fraction = (-18 - LastAltitudeForInterpolation) / (Horizontal.Y - LastAltitudeForInterpolation);
      event.JD = LastJD + (fraction * StepInterval);

      //Ensure the LastBearing and Horizontal.X values are correct for interpolation
      double LastBearing2 = LastBearing;
      double HorizontalX2 = Horizontal.X;
      if (fabs(HorizontalX2 - LastBearing2) > 180)
      {
        if (HorizontalX2 > LastBearing2)
          LastBearing2 += 360;
        else
          HorizontalX2 += 360;
      }

      event.Bearing = CAACoordinateTransformation::MapTo0To360Range(LastBearing2 + (fraction * (HorizontalX2 - LastBearing2)));
      events.push_back(event);
    }
    else if ((LastAltitudeForDetectingTwilight > -12) && (AltitudeForTwilight <= -12)) //We have just started nautical twilight
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::StartNauticalTwilight;
      const double fraction = (-12 - LastAltitudeForInterpolation) / (Horizontal.Y - LastAltitudeForInterpolation);
      event.JD = LastJD + (fraction * StepInterval);

      //Ensure the LastBearing and Horizontal.X values are correct for interpolation
      double LastBearing2 = LastBearing;
      double HorizontalX2 = Horizontal.X;
      if (fabs(HorizontalX2 - LastBearing2) > 180)
      {
        if (HorizontalX2 > LastBearing2)
          LastBearing2 += 360;
        else
          HorizontalX2 += 360;
      }

      event.Bearing = CAACoordinateTransformation::MapTo0To360Range(LastBearing2 + (fraction * (HorizontalX2 - LastBearing2)));
      events.push_back(event);
    }
    else if ((LastAltitudeForDetectingTwilight > -6) && (AltitudeForTwilight <= -6)) //We have just started nautical twilight
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::StartCivilTwilight;
      const double fraction = (-6 - LastAltitudeForInterpolation) / (Horizontal.Y - LastAltitudeForInterpolation);
      event.JD = LastJD + (fraction * StepInterval);

      //Ensure the LastBearing and Horizontal.X values are correct for interpolation
      double LastBearing2 = LastBearing;
      double HorizontalX2 = Horizontal.X;
      if (fabs(HorizontalX2 - LastBearing2) > 180)
      {
        if (HorizontalX2 > LastBearing2)
          LastBearing2 += 360;
        else
          HorizontalX2 += 360;
      }

      event.Bearing = CAACoordinateTransformation::MapTo0To360Range(LastBearing2 + (fraction * (HorizontalX2 - LastBearing2)));
      events.push_back(event);
    }
  }
  if (LastAltitudeForDetectingRiseSet != -90)
  {
    if ((LastAltitudeForDetectingRiseSet < 0) && (AltitudeForDetectingRiseSet >= 0)) //We have just rose above the horizon
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::Rise;
      const double fraction = (0 - LastAltitudeForInterpolation + h0) / (Horizontal.Y - LastAltitudeForInterpolation);
      event.JD = LastJD + (fraction * StepInterval);

      //Ensure the LastBearing and Horizontal.X values are correct for interpolation
      double LastBearing2 = LastBearing;
      double HorizontalX2 = Horizontal.X;
      if (fabs(HorizontalX2 - LastBearing2) > 180)
      {
        if (HorizontalX2 > LastBearing2)
          LastBearing2 += 360;
        else
          HorizontalX2 += 360;
      }

      event.Bearing = CAACoordinateTransformation::MapTo0To360Range(LastBearing2 + (fraction * (HorizontalX2 - LastBearing2)));
      events.push_back(event);
    }
    else if ((LastAltitudeForDetectingRiseSet > 0) && (AltitudeForDetectingRiseSet <= 0)) //We have just set below the horizon
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::Set;
      const double fraction = (0 - LastAltitudeForInterpolation + h0) / (Horizontal.Y - LastAltitudeForInterpolation);
      event.JD = LastJD + (fraction * StepInterval);

      //Ensure the LastBearing and Horizontal.X values are correct for interpolation
      double LastBearing2 = LastBearing;
      double HorizontalX2 = Horizontal.X;
      if (fabs(HorizontalX2 - LastBearing2) > 180)
      {
        if (HorizontalX2 > LastBearing2)
          LastBearing2 += 360;
        else
          HorizontalX2 += 360;
      }

      event.Bearing = CAACoordinateTransformation::MapTo0To360Range(LastBearing2 + (fraction * (HorizontalX2 - LastBearing2)));
      events.push_back(event);
    }
  }
  if (LastBearing != -1)
  {
    if ((LastBearing > 270) && (Horizontal.X >= 0) && (Horizontal.X <= 90)) //We have just crossed the southern meridian from east to west
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::SouthernTransit;
      const double fraction = (360 - LastBearing) / (Horizontal.X + (360 - LastBearing));
      event.JD = LastJD + (fraction * StepInterval);
      event.GeometricAltitude = LastAltitudeForInterpolation + (fraction * (Horizontal.Y - LastAltitudeForInterpolation));
      event.bAboveHorizon = (AltitudeForDetectingRiseSet > 0);
      events.push_back(event);
    }
    else if ((LastBearing < 90) && (Horizontal.X >= 270) && (Horizontal.X <= 360)) //We have just crossed the southern meridian from west to east
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::SouthernTransit;
      const double fraction = LastBearing / (360 - Horizontal.X + LastBearing);
      event.JD = LastJD + (fraction * StepInterval);
      event.GeometricAltitude = LastAltitudeForInterpolation + (fraction * (Horizontal.Y - LastAltitudeForInterpolation));
      event.bAboveHorizon = (AltitudeForDetectingRiseSet > 0);
      events.push_back(event);
    }
    else if ((LastBearing < 180) && (Horizontal.X >= 180)) //We have just crossed the northern meridian from west to east
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::NorthernTransit;
      const double fraction = (180 - LastBearing) / (Horizontal.X - LastBearing);
      event.JD = LastJD + (fraction * StepInterval);
      event.GeometricAltitude = LastAltitudeForInterpolation + (fraction * (Horizontal.Y - LastAltitudeForInterpolation));
      event.bAboveHorizon = (AltitudeForDetectingRiseSet > 0);
      events.push_back(event);
    }
    else if ((LastBearing > 180) && (Horizontal.X <= 180)) //We have just crossed the northern meridian from east to west
    {
      CAARiseTransitSetDetails2 event;
      event.type = CAARiseTransitSetDetails2::Type::NorthernTransit;
      const double fraction = ((LastBearing - 180) / (LastBearing - Horizontal.X));
      event.JD = LastJD + (fraction * StepInterval);
      event.GeometricAltitude = LastAltitudeForInterpolation + (fraction * (Horizontal.Y - LastAltitudeForInterpolation));
      event.bAboveHorizon = (AltitudeForDetectingRiseSet > 0);
      events.push_back(event);
    }
  }
}

vector<CAARiseTransitSetDetails2> CAARiseTransitSet2::Calculate(double StartJD, double EndJD, Object object, double Longitude, double Latitude, double h0, double Height, double StepInterval, bool bHighPrecision)
{
  //What will be the return value
  vector<CAARiseTransitSetDetails2> events;

  const double LongtitudeAsHourAngle = CAACoordinateTransformation::DegreesToHours(Longitude);
  double JD = StartJD;
  double LastJD = 0;
  double LastAltitudeForDetectingRiseSet = -90;
  double LastAltitudeForInterpolation = -90;
  double LastAltitudeForDetectingTwilight = -90;
  double LastBearing = -1;
  while (JD < EndJD)
  {
    CAAEllipticalPlanetaryDetails details;
    CAA2DCoordinate Topo;
    switch (object)
    {
      case Object::SUN:
      {
        const double Long = CAASun::ApparentEclipticLongitude(JD, bHighPrecision);
        const double Lat = CAASun::ApparentEclipticLatitude(JD, bHighPrecision);
        const CAA2DCoordinate Equatorial = CAACoordinateTransformation::Ecliptic2Equatorial(Long, Lat, CAANutation::TrueObliquityOfEcliptic(JD));
        const double SunRad = CAAEarth::RadiusVector(JD, bHighPrecision);
        Topo = CAAParallax::Equatorial2Topocentric(Equatorial.X, Equatorial.Y, SunRad, Longitude, Latitude, Height, JD);
        break;
      }
      case Object::MOON:
      {
        const double Long = CAAMoon::EclipticLongitude(JD);
        const double Lat = CAAMoon::EclipticLatitude(JD);
        const CAA2DCoordinate Equatorial = CAACoordinateTransformation::Ecliptic2Equatorial(Long, Lat, CAANutation::TrueObliquityOfEcliptic(JD));
        const double MoonRad = CAAMoon::RadiusVector(JD) / 149597871; //Convert Kms to AUs
        Topo = CAAParallax::Equatorial2Topocentric(Equatorial.X, Equatorial.Y, MoonRad, Longitude, Latitude, Height, JD);
        break;
      }
      case Object::MERCURY:
      {
        details = CAAElliptical::Calculate(JD, CAAElliptical::EllipticalObject::MERCURY, bHighPrecision);
        Topo = CAAParallax::Equatorial2Topocentric(details.ApparentGeocentricRA, details.ApparentGeocentricDeclination, details.ApparentGeocentricDistance, Longitude, Latitude, Height, JD);
        break;
      }
      case Object::VENUS:
      {
        details = CAAElliptical::Calculate(JD, CAAElliptical::EllipticalObject::VENUS, bHighPrecision);
        Topo = CAAParallax::Equatorial2Topocentric(details.ApparentGeocentricRA, details.ApparentGeocentricDeclination, details.ApparentGeocentricDistance, Longitude, Latitude, Height, JD);
        break;
      }
      case Object::MARS:
      {
        details = CAAElliptical::Calculate(JD, CAAElliptical::EllipticalObject::MARS, bHighPrecision);
        Topo = CAAParallax::Equatorial2Topocentric(details.ApparentGeocentricRA, details.ApparentGeocentricDeclination, details.ApparentGeocentricDistance, Longitude, Latitude, Height, JD);
        break;
      }
      case Object::JUPITER:
      {
        details = CAAElliptical::Calculate(JD, CAAElliptical::EllipticalObject::JUPITER, bHighPrecision);
        Topo = CAAParallax::Equatorial2Topocentric(details.ApparentGeocentricRA, details.ApparentGeocentricDeclination, details.ApparentGeocentricDistance, Longitude, Latitude, Height, JD);
        break;
      }
      case Object::SATURN:
      {
        details = CAAElliptical::Calculate(JD, CAAElliptical::EllipticalObject::SATURN, bHighPrecision);
        Topo = CAAParallax::Equatorial2Topocentric(details.ApparentGeocentricRA, details.ApparentGeocentricDeclination, details.ApparentGeocentricDistance, Longitude, Latitude, Height, JD);
        break;
      }
      case Object::URANUS:
      {
        details = CAAElliptical::Calculate(JD, CAAElliptical::EllipticalObject::URANUS, bHighPrecision);
        Topo = CAAParallax::Equatorial2Topocentric(details.ApparentGeocentricRA, details.ApparentGeocentricDeclination, details.ApparentGeocentricDistance, Longitude, Latitude, Height, JD);
        break;
      }
      case Object::NEPTUNE:
      {
        details = CAAElliptical::Calculate(JD, CAAElliptical::EllipticalObject::NEPTUNE, bHighPrecision);
        Topo = CAAParallax::Equatorial2Topocentric(details.ApparentGeocentricRA, details.ApparentGeocentricDeclination, details.ApparentGeocentricDistance, Longitude, Latitude, Height, JD);
        break;
      }
      default:
      {
        assert(false);
        break;
      }
    }
    const double AST = CAASidereal::ApparentGreenwichSiderealTime(JD);
    const double LocalHourAngle = AST - LongtitudeAsHourAngle - Topo.X;
    const CAA2DCoordinate Horizontal = CAACoordinateTransformation::Equatorial2Horizontal(LocalHourAngle, Topo.Y, Latitude);
    const double AltitudeForDetectingRiseSet = (Horizontal.Y - h0);

    //Call the helper method to add any found events
    AddEvents(events, LastAltitudeForDetectingRiseSet, AltitudeForDetectingRiseSet, LastAltitudeForInterpolation, h0, Horizontal, LastJD, StepInterval, LastBearing, object, LastAltitudeForDetectingTwilight, Horizontal.Y);

    //Prepare for the next loop
    LastAltitudeForDetectingRiseSet = AltitudeForDetectingRiseSet;
    LastAltitudeForInterpolation = Horizontal.Y;
    LastAltitudeForDetectingTwilight = Horizontal.Y;
    LastBearing = Horizontal.X;
    LastJD = JD;
    JD += StepInterval;
  }

  return events;
}

//The higher accuracy version for the moon where the "standard altitude" is not treated as a constant
vector<CAARiseTransitSetDetails2> CAARiseTransitSet2::CalculateMoon(double StartJD, double EndJD, double Longitude, double Latitude, double Height, double StepInterval)
{
  //What will be the return value
  vector<CAARiseTransitSetDetails2> events;

  const double LongtitudeAsHourAngle = CAACoordinateTransformation::DegreesToHours(Longitude);
  double JD = StartJD;
  double LastJD = 0;
  double LastAltitudeForDetectingRiseSet = -90;
  double LastAltitudeForInterpolation = -90;
  double LastBearing = -1;
  while (JD < EndJD)
  {
    const double Long = CAAMoon::EclipticLongitude(JD);
    const double Lat = CAAMoon::EclipticLatitude(JD);
    const CAA2DCoordinate Equatorial = CAACoordinateTransformation::Ecliptic2Equatorial(Long, Lat, CAANutation::TrueObliquityOfEcliptic(JD));
    const double MoonRad = CAAMoon::RadiusVector(JD);
    const CAA2DCoordinate Topo = CAAParallax::Equatorial2Topocentric(Equatorial.X, Equatorial.Y, MoonRad / 149597871, Longitude, Latitude, Height, JD);
    const double AST = CAASidereal::ApparentGreenwichSiderealTime(JD);
    const double LocalHourAngle = AST - LongtitudeAsHourAngle - Topo.X;
    const CAA2DCoordinate Horizontal = CAACoordinateTransformation::Equatorial2Horizontal(LocalHourAngle, Topo.Y, Latitude);
    const double h0 = 0.7275 * CAAMoon::RadiusVectorToHorizontalParallax(MoonRad) - CAACoordinateTransformation::DMSToDegrees(0, 34, 0);
    const double AltitudeForDetectingRiseSet = (Horizontal.Y - h0);

    //Call the helper method to add any found events
    AddEvents(events, LastAltitudeForDetectingRiseSet, AltitudeForDetectingRiseSet, LastAltitudeForInterpolation, h0, Horizontal, LastJD, StepInterval, LastBearing, Object::MOON, 0, 0);

    //Prepare for the next loop
    LastAltitudeForDetectingRiseSet = AltitudeForDetectingRiseSet;
    LastAltitudeForInterpolation = Horizontal.Y;
    LastBearing = Horizontal.X;
    LastJD = JD;
    JD += StepInterval;
  }

  return events;
}


//A version for a stationary object such as a star
vector<CAARiseTransitSetDetails2> CAARiseTransitSet2::CalculateStationary(double StartJD, double EndJD, double Alpha, double Delta, double Longitude, double Latitude, double h0, double StepInterval)
{
  //What will be the return value
  vector<CAARiseTransitSetDetails2> events;

  const double LongtitudeAsHourAngle = CAACoordinateTransformation::DegreesToHours(Longitude);
  double JD = StartJD;
  double LastJD = 0;
  double LastAltitudeForDetectingRiseSet = -90;
  double LastAltitudeForInterpolation = -90;
  double LastBearing = -1;
  while (JD < EndJD)
  {
    const double AST = CAASidereal::ApparentGreenwichSiderealTime(JD);
    const double LocalHourAngle = AST - LongtitudeAsHourAngle - Alpha;
    const CAA2DCoordinate Horizontal = CAACoordinateTransformation::Equatorial2Horizontal(LocalHourAngle, Delta, Latitude);
    const double AltitudeForDetectingRiseSet = (Horizontal.Y - h0);

    //Call the helper method to add any found event
    AddEvents(events, LastAltitudeForDetectingRiseSet, AltitudeForDetectingRiseSet, LastAltitudeForInterpolation, h0, Horizontal, LastJD, StepInterval, LastBearing, Object::STAR, 0, 0);

    //Prepare for the next loop
    LastAltitudeForDetectingRiseSet = AltitudeForDetectingRiseSet;
    LastAltitudeForInterpolation = Horizontal.Y;
    LastBearing = Horizontal.X;
    LastJD = JD;
    JD += StepInterval;
  }

  return events;
}
