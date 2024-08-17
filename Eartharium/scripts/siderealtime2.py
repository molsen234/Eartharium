# Libraries
import math
from eartharium import *

# Constants
deg2rad = math.pi / 180.0
rad2deg = 180.0 / math.pi

def deltaT2deltaJD(deltaT):
    return deltaT / 86400

location = LLD(deg2rad * 55.6761, deg2rad * 12.5683, 0.0)  # Copenhagen as defined in Eartharium.cpp
#location = LLD(deg2rad * 55.676111111, deg2rad * 12.568305556, 0.0)  # Copenhagen when Neoprogrammics converts to DMS and then back to fractional D
print()
print("Location:", ACoord.formatLatLon(location))

truetime = EDateTime(2024,5,19.0, 0.0,0.0,0.0)
print()
print("Date time set:", truetime.string())
print(" JD UTC:", truetime.jd_utc(), " JD TT:", truetime.jd_tt())
print(" deltaT (IERS table):", EDateTime.DeltaT(truetime.jd_utc()), "deltaT (NASA poly.):", EDateTime.DeltaT_Polynomial(truetime.jd_utc()))

print()
print("Examining https://neoprogrammics.com/sidereal-time-calculator/")

print()
print("Sidereal time from JD UTC (hms, dms, deg):")
mgst = AEarth.MeanGreenwichSiderealTime(truetime.jd_utc())
agst = AEarth.ApparentGreenwichSiderealTime(truetime.jd_utc())
mlst = mgst + location.lon
alst = agst + location.lon
print(" Mean Local Sidereal Time:        ", ACoord.angle2uHMSstring(mlst), ACoord.angle2DMSstring(mlst), rad2deg * mlst)
print(" Apparent Local Sidereal Time:    ", ACoord.angle2uHMSstring(alst), ACoord.angle2DMSstring(alst), rad2deg * alst)
print(" Mean Greenwich Sidereal Time:    ", ACoord.angle2uHMSstring(mgst), ACoord.angle2DMSstring(mgst), rad2deg * mgst)
print(" Apparent Greenwich Sidereal Time:", ACoord.angle2uHMSstring(agst), ACoord.angle2DMSstring(agst), rad2deg * agst)
print("These are the correct values for the instant specfied in UTC.")
print("Perfect match to J.Tanner with local date time 2024-05-19 00:00:00, deltaT 00:00:00")
# Mean Local Sidereal Time:          016h38m54.219s +249°43'33.292" 249.72591455567553
# Apparent Local Sidereal Time:      016h38m53.912s +249°43'28.676" 249.72463229585486
# Mean Greenwich Sidereal Time:      015h48m37.827s +237°09'27.412" 237.15761455567556
# Apparent Greenwich Sidereal Time:  015h48m37.520s +237°09'22.796" 237.1563322958549

print()
print("Sidereal time from JD TT minus deltaT 69.2046428  (hms, dms, deg):")
mgst = AEarth.MeanGreenwichSiderealTime(truetime.jd_tt() - deltaT2deltaJD(69.2046428))
agst = AEarth.ApparentGreenwichSiderealTime(truetime.jd_tt() - deltaT2deltaJD(69.2046428))
mlst = mgst + location.lon
alst = agst + location.lon
print(" Mean Local Sidereal Time:        ", ACoord.angle2uHMSstring(mlst), ACoord.angle2DMSstring(mlst), rad2deg * mlst)
print(" Apparent Local Sidereal Time:    ", ACoord.angle2uHMSstring(alst), ACoord.angle2DMSstring(alst), rad2deg * alst)
print(" Mean Greenwich Sidereal Time:    ", ACoord.angle2uHMSstring(mgst), ACoord.angle2DMSstring(mgst), rad2deg * mgst)
print(" Apparent Greenwich Sidereal Time:", ACoord.angle2uHMSstring(agst), ACoord.angle2DMSstring(agst), rad2deg * agst)
print("Matches exactly above, so good conversion from JD TT to JD UTC")
# Mean Local Sidereal Time:          016h38m54.219s +249°43'33.292" 249.72591455567553
# Apparent Local Sidereal Time:      016h38m53.912s +249°43'28.676" 249.72463229585486
# Mean Greenwich Sidereal Time:      015h48m37.827s +237°09'27.412" 237.15761455567556
# Apparent Greenwich Sidereal Time:  015h48m37.520s +237°09'22.796" 237.1563322958549

print()
print("Sidereal time from JD TT  (hms, dms, deg):")
mgst = AEarth.MeanGreenwichSiderealTime(truetime.jd_tt())
agst = AEarth.ApparentGreenwichSiderealTime(truetime.jd_tt())
mlst = mgst + location.lon
alst = agst + location.lon
print(" Mean Local Sidereal Time:        ", ACoord.angle2uHMSstring(mlst), ACoord.angle2DMSstring(mlst), rad2deg * mlst)
print(" Apparent Local Sidereal Time:    ", ACoord.angle2uHMSstring(alst), ACoord.angle2DMSstring(alst), rad2deg * alst)
print(" Mean Greenwich Sidereal Time:    ", ACoord.angle2uHMSstring(mgst), ACoord.angle2DMSstring(mgst), rad2deg * mgst)
print(" Apparent Greenwich Sidereal Time:", ACoord.angle2uHMSstring(agst), ACoord.angle2DMSstring(agst), rad2deg * agst)
# Mean Local Sidereal Time:          016h40m03.614s +250°00'54.204" 250.0150567633837
# Apparent Local Sidereal Time:      016h40m03.306s +250°00'49.588" 250.0137744861292
# Mean Greenwich Sidereal Time:      015h49m47.222s +237°26'48.324" 237.44675676338375
# Apparent Greenwich Sidereal Time:  015h49m46.914s +237°26'43.708" 237.4454744861292

print()
print("Sidereal time from JD UTC plus deltaT 69.2046428  (hms, dms, deg):")
mgst = AEarth.MeanGreenwichSiderealTime(truetime.jd_utc() + deltaT2deltaJD(69.2046428))
agst = AEarth.ApparentGreenwichSiderealTime(truetime.jd_utc() + deltaT2deltaJD(69.2046428))
mlst = mgst + location.lon
alst = agst + location.lon
print(" Mean Local Sidereal Time:        ", ACoord.angle2uHMSstring(mlst), ACoord.angle2DMSstring(mlst), rad2deg * mlst)
print(" Apparent Local Sidereal Time:    ", ACoord.angle2uHMSstring(alst), ACoord.angle2DMSstring(alst), rad2deg * alst)
print(" Mean Greenwich Sidereal Time:    ", ACoord.angle2uHMSstring(mgst), ACoord.angle2DMSstring(mgst), rad2deg * mgst)
print(" Apparent Greenwich Sidereal Time:", ACoord.angle2uHMSstring(agst), ACoord.angle2DMSstring(agst), rad2deg * agst)
print("Matches exactly above, so good conversion from JD UTC to JD TT")
# Mean Local Sidereal Time:          016h40m03.614s +250°00'54.204" 250.0150567633837
# Apparent Local Sidereal Time:      016h40m03.306s +250°00'49.588" 250.0137744861292
# Mean Greenwich Sidereal Time:      015h49m47.222s +237°26'48.324" 237.44675676338375
# Apparent Greenwich Sidereal Time:  015h49m46.914s +237°26'43.708" 237.4454744861292

print()
print("Sidereal time from JD UTC plus deltaT 69.0  (hms, dms, deg):")
mgst = AEarth.MeanGreenwichSiderealTime(truetime.jd_utc() + deltaT2deltaJD(69.0))
agst = AEarth.ApparentGreenwichSiderealTime(truetime.jd_utc() + deltaT2deltaJD(69.0))
mlst = mgst + location.lon
alst = agst + location.lon
print(" Mean Local Sidereal Time:        ", ACoord.angle2uHMSstring(mlst), ACoord.angle2DMSstring(mlst), rad2deg * mlst)
print(" Apparent Local Sidereal Time:    ", ACoord.angle2uHMSstring(alst), ACoord.angle2DMSstring(alst), rad2deg * alst)
print(" Mean Greenwich Sidereal Time:    ", ACoord.angle2uHMSstring(mgst), ACoord.angle2DMSstring(mgst), rad2deg * mgst)
print(" Apparent Greenwich Sidereal Time:", ACoord.angle2uHMSstring(agst), ACoord.angle2DMSstring(agst), rad2deg * agst)
print("Perfect match to J.Tanner with local date time 2024-05-19 00:00:00, deltaT 00:01:09")
# Mean Local Sidereal Time:          016h40m03.408s +250°00'51.126" 250.01420165372934
# Apparent Local Sidereal Time:      016h40m03.101s +250°00'46.510" 250.01291937652636
# Mean Greenwich Sidereal Time:      015h49m47.016s +237°26'45.246" 237.44590165372938
# Apparent Greenwich Sidereal Time:  015h49m46.709s +237°26'40.630" 237.4446193765264

print()
print("AGST for JD 2460449.500798611111:", ACoord.angle2uHMSstring(AEarth.ApparentGreenwichSiderealTime(2460449.500798611111)))

print()
print("Conclusion: If entering local date and time in TT, provide the negative of deltaT. If supplying UTC, leave the deltaT option as 00:00:00.")


