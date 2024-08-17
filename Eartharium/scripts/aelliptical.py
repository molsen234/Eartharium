# Access the embedded bridge module

import math

from eartharium import *
#symbols_before = dir()
#from eartharium import *
#symbols_after = dir()
#imported_symbols = [s for s in symbols_after if not s in symbols_before]
#print(imported_symbols)

print("Python: Script loaded Eartharium module.\n")

deg2rad = math.pi / 180.0
rad2deg = 180.0 / math.pi

#myapp = getApplication()
#myapp.interactive = True
#print("Python: Set interactive mode to stay in Eartharium after script completes.")


myloc = LLD(deg2rad * 55.6761, deg2rad * 12.5683, 0.0)  # Copenhagen
#myloc = LLD(deg2rad * 55.676111111, deg2rad * 12.568305556, 0.0)  # Copenhagen
mytime = EDateTime(2024,5,19, 0,0, 0.0)
#mytime.setJD_TT(2460449.5)

details = AElliptical.getPlanetaryDetails(mytime.jd_tt(), Planet.A_MARS, Planetary_Ephemeris.EPH_VSOP87_FULL)
print("Date time:", mytime.string(), "  JD_TT:",mytime.jd_tt(),"  JD_UTC:",mytime.jd_utc())
print("DeltaT (IERS):", EDateTime.DeltaT(mytime.jd_utc()), "DeltaT (NASA poly):", EDateTime.DeltaT_Polynomial(mytime.jd_utc()),"\n")
print("\nMars")
print("----")
print("True Heliocentric Ecliptic Spherical:    ", ACoord.formatLatLon(details.thecs))
print("True Geocentric Ecliptic Spherical:      ", ACoord.formatLatLon(details.tgecs))
# glm::dvec3 tgecr
print("True Geocentric Equatorial Spherical:    ", ACoord.formatDecRA(details.tgeqs))
print("True Light-Time (in minutes):            ", details.tlt * 24 * 60)
print("True distance:                           ", details.tgecs.dst)
print("Apparent Geocentric Ecliptic Spherical:  ", ACoord.formatLatLon(details.agecs))
print("Apparent Geocentric Equatorial Spherical:", ACoord.formatDecRA(details.ageqs))
#Apparent Geocentric Equatorial Spherical: +019°43'07.911" /  003h57m11.586s (datetime 2024 7 22 16 50 00)
# Exactly (to 10th of second) equal to Carte du Ciel when Topocentric is turned off
# CdC used DE430 JPL ephemerides for the calculation
print("Apparent Light-Time (in minutes):        ", details.alt * 24 * 60)
print("Apparent distance:                       ", details.agecs.dst)

# Tested Apparent Geocentric Equatorial Spherical coordinates for Mars on 19th May 2024 00:00:00
# Output values match exactly to Fred Espernak's results based on DE405 here: https://astropixels.com/ephemeris/planets/mars2024.html
#                         and to Jay Tanner's (also using DE405) within 0.02 arcseconds: https://neoprogrammics.com/de405-usno-ae98/DE405_Mars.php
#                         Stellarium v24.2 matches RA of date exactly and Dec is off by 0.1 arcsecond (when Topocentric coordinates are turned off in settings)
# NOTE: On J.Tanner's site, the deltaT is given as 74 seconds, where I and F.Espernak use 69.2 seconds.
#       I suspect Tanner displays the adjusted DE405 ephemeris time, whereas Espernak shows the time before adjustment
#       (DE405 uses barycentric dynamical time which is adjusted for general relativistic effects, but 5 seconds sounds like a lot)
#       https://asd.gsfc.nasa.gov/Craig.Markwardt/bary/ has some details, read it when convenient.
# UPD: Tanner uses the NASA deltaT polynomials which currently evaluate to 74.2.
# Topocentric RA/Dec also matches J.Tanner precisely although he provides deviating LHA, investigating further:

agst = AEarth.ApparentGreenwichSiderealTime(mytime.jd_utc())
print("AGST:", agst, rad2deg*agst, ACoord.angle2uHMSstring(agst))
alst = agst + myloc.lon  # apparent local sidereal time
print("ALST:", alst, rad2deg*alst, ACoord.angle2uHMSstring(alst))

topo = AEarth.Equatorial2Topocentric(details.ageqs, myloc, agst)
print("Topocentric: ", ACoord.formatDecRA(topo))

lha = alst - topo.lon  # East positive
lha2 = agst - myloc.lon - topo.lon  # West positive
#if (lha > math.pi): lha -= math.pi  # common convension for hour angles is -12h to 12h rather than the 0h to 24h for right ascension
#if (lha2 > math.pi): lha2 -= math.pi
print("Local Hour Angle (east positive):", lha, ACoord.angle2uHMSstring(lha), rad2deg * lha)
print("Local Hour Angle (west positive):", lha2, ACoord.angle2uHMSstring(lha2), rad2deg * lha2)

horizontal = Spherical.Equatorial2Horizontal(lha, topo.lat, myloc.lat)
# convert to clockwise from North
horizontal.lon -= math.pi
print("Horizontal:", ACoord.formatEleAz(horizontal), rad2deg * horizontal.lat, rad2deg * horizontal.lon)
#Above verified in Stellarium to an accuracy of less than 1 second in time, which is the granularity Stellarium provides. Note that this is several arc seconds.

#Calculate using Tanner's LHA:
tanner = Spherical.Equatorial2Horizontal(deg2rad * 236.6170907512, topo.lat, myloc.lat)
print("Horizontal from Tanner: ", ACoord.formatEleAz(tanner)) # Perfect match, so Spherical.Equatorial2Horizontal() works.
# Tanner's horizontal coordinates are off by quite a bit (many arc minutes) from mine above (variable horizontal), I suspect he has a bug in his code.
# UPD: Tanner adds the deltaT value (again?) when calculating the ALST and thus the LHA. This is mimmicked by: https://aa.usno.navy.mil/data/siderealtime
#      and Tanner's other page: https://neoprogrammics.com/sidereal-time-calculator/


print("\nPython: Script completed.")
