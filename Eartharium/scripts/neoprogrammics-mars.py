# Imports
import math
from eartharium import *

# Constants
deg2rad = math.pi / 180.0
rad2deg = 180.0 / math.pi

def deltaT2deltaJD(deltaT):
    return deltaT / 86400


# Reviewing https://neoprogrammics.com/de405-usno-ae98/DE405_Mars.php
#myloc = LLD(deg2rad * 55.6761, deg2rad * 12.5683, 0.0)  # Copenhagen after Neoprogrammics converts it to DMS and back to decimal degrees.
myloc = LLD(deg2rad * 55.676111111, deg2rad * 12.568305556, 0.0)  # Copenhagen after Neoprogrammics converts it to DMS and back to decimal degrees.
mytime = EDateTime(2024,5,19.0, 0.0,0.0,0.0)
deltaTcomp = 0.0
deltaTcomp = deltaT2deltaJD(74.0)

print()
print("Local date time:", mytime.string(), "  JD_TT:", mytime.jd_tt(), "  JD_UTC:", mytime.jd_utc())
print(" (JD TT from JD UTC:", mytime.jd_utc() + deltaTcomp, ")") #  + deltaTcomp
print("DeltaT (IERS table):", EDateTime.DeltaT(mytime.jd_utc()), "DeltaT (NASA poly.):", EDateTime.DeltaT_Polynomial(mytime.jd_utc()))

# Calculate Mars position
details = AElliptical.getPlanetaryDetails(mytime.jd_utc() + deltaTcomp, Planet.A_MARS, Planetary_Ephemeris.EPH_VSOP87_FULL) #+ deltaTcomp

print("Apparent Geocentric Equatorial Spherical:", ACoord.formatDecRA(details.ageqs), " / ", details.tgecs.dst)

agst = AEarth.ApparentGreenwichSiderealTime(mytime.jd_utc() + deltaTcomp)
alst = agst + myloc.lon

topo = AEarth.Equatorial2Topocentric(details.ageqs, myloc, agst)

lha = alst - topo.lon  # East positive
#if (lha > math.pi): lha -= math.pi  # common convension for hour angles is -12h to 12h rather than the 0h to 24h for right ascension

horizontal = Spherical.Equatorial2Horizontal(lha, topo.lat, myloc.lat)

print()
print("Topocentric: ", ACoord.formatDecRA(topo))

print()
print("Local Hour Angle (east positive):", ACoord.angle2uHMSstring(lha), rad2deg * lha)
print("Apparent Local Sidereal Time:    ", ACoord.angle2uHMSstring(alst), rad2deg * alst)

print()
print("Horizontal:", ACoord.formatEleAz(horizontal), rad2deg * horizontal.lat, rad2deg * horizontal.lon)

#Calculate using Tanner's LHA directly, for HMS comparison:
tanner = Spherical.Equatorial2Horizontal(deg2rad * 236.6170907512, topo.lat, myloc.lat)
if (deltaTcomp == 0.0): tanner = Spherical.Equatorial2Horizontal(deg2rad * 236.30851740786, topo.lat, myloc.lat)

print("Horizontal from Tanner: ", ACoord.formatEleAz(tanner))




print("\n\n\nMy calculations:")

print("With 2024-05-19 00:00:00 UTC")
utc_time = EDateTime(2024,5,19.0, 0.0,0.0,0.0)
print(" - Date time:", utc_time.string(), "JD TT:", utc_time.jd_tt(), "JD UTC:", utc_time.jd_utc())
utc_details = AElliptical.getPlanetaryDetails(utc_time.jd_tt(), Planet.A_MARS, Planetary_Ephemeris.EPH_VSOP87_FULL)
print(" - Apparent Geocentric Equatorial Spherical:", ACoord.formatDecRA(utc_details.ageqs), " / ", utc_details.tgecs.dst)
utc_agst = AEarth.ApparentGreenwichSiderealTime(utc_time.jd_utc())
utc_alst = utc_agst + myloc.lon
utc_topo = AEarth.Equatorial2Topocentric(utc_details.ageqs, myloc, utc_agst)
utc_lha = utc_alst - utc_topo.lon
utc_horizontal = Spherical.Equatorial2Horizontal(utc_lha, utc_topo.lat, myloc.lat)
print(" - Topocentric:        ", ACoord.formatDecRA(utc_topo))
print(" - Local Hour Angle:   ", ACoord.angle2uHMSstring(utc_lha), rad2deg * utc_lha)
print(" - Local Sidereal Time:", ACoord.angle2uHMSstring(utc_alst), rad2deg * utc_alst)
print(" - Horizontal:         ", ACoord.formatEleAz(utc_horizontal), rad2deg * utc_horizontal.lat, rad2deg * utc_horizontal.lon)

print("With 2024-05-19 00:00:00 TT")
tt_time = EDateTime(utc_time.jd_utc(), True)
#tt_time.addTime(0,0,0.0, 0.0,0.0,5.0)
print(" - Date time:", tt_time.string(), "JD TT:", tt_time.jd_tt(), "JD UTC:", tt_time.jd_utc())
tt_details = AElliptical.getPlanetaryDetails(tt_time.jd_tt(), Planet.A_MARS, Planetary_Ephemeris.EPH_VSOP87_FULL)
print(" - Apparent Geocentric Equatorial Spherical:", ACoord.formatDecRA(tt_details.ageqs), " / ", tt_details.tgecs.dst)
tt_agst = AEarth.ApparentGreenwichSiderealTime(tt_time.jd_utc())
tt_alst = tt_agst + myloc.lon
tt_topo = AEarth.Equatorial2Topocentric(tt_details.ageqs, myloc, tt_agst)
tt_lha = tt_alst - tt_topo.lon
tt_horizontal = Spherical.Equatorial2Horizontal(tt_lha, tt_topo.lat, myloc.lat)
print(" - Topocentric:        ", ACoord.formatDecRA(tt_topo))
print(" - Local Hour Angle:   ", ACoord.angle2uHMSstring(tt_lha), rad2deg * tt_lha)
print(" - Local Sidereal Time:", ACoord.angle2uHMSstring(tt_alst), rad2deg * tt_alst)
print(" - Horizontal:         ", ACoord.formatEleAz(tt_horizontal), rad2deg * tt_horizontal.lat, rad2deg * tt_horizontal.lon)


print("\n\nPython: Script completed.")
