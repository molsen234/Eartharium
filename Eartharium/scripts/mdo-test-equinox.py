# Access the embedded bridge module
#import eartharium
import math

#symbols_before = dir()
from eartharium import *
#symbols_after = dir()
#imported_symbols = [s for s in symbols_after if not s in symbols_before]
#print(imported_symbols)

print("Python: Script loaded Eartharium module.\n")

myapp = getApplication()
#myapp.interactive = True
#print("Python: Set interactive mode to stay in Eartharium after script completes.")

def jd_diff_to_dhms(diff):
    days = int(diff)
    frac = diff - days
    frac *= 24.0
    hrs = int(frac)
    frac -= hrs
    frac *= 60.0
    mins = int(frac)
    frac -= mins
    frac *= 60.0
    return f"{diff:10.6f} = {days} {hrs:02}:{mins:02}:{frac:06.3f}"

eph = Ephemeris.VSOP87_SHORT   # The difference when using VSOP87_FULL is less than milliseconds
y_begin = 1980
y_end = 2005

eqn = EDateTime();

# Test AEarth::NorthwardEquinox()
prev = AEarth.NorthwardEquinox(y_begin - 1, eph)
print("\nNorthward Equinoxes")
print("===================")
print("\nJD_UTC                Date       Time     Tz   Tropical Year Length")
print("----------------------------------------------------------------------------")
for year in range(y_begin, y_end):
    equinox = AEarth.NorthwardEquinox(year, eph);
    print(f"{equinox:19.12f}", end='  ')
    eqn.setJD_TT(equinox)
    print(eqn.string(), end='  ')
    print(jd_diff_to_dhms(equinox - prev))  # length of tropical year (March equinox to March equinox) in days hrs:mins:secs
    prev = equinox

# Test AEarth::NorthernSolstice()
prev = AEarth.NorthernSolstice(y_begin - 1, eph)
print("\nNorthern Solstices")
print("==================")
print("\nJD_UTC                Date       Time     Tz   Tropical Year Length")
print("----------------------------------------------------------------------------")
for year in range(y_begin, y_end):
    solstice = AEarth.NorthernSolstice(year, eph);
    print(f"{solstice:19.12f}", end='  ')
    eqn.setJD_TT(solstice)
    print(eqn.string(), end='  ')
    print(jd_diff_to_dhms(solstice - prev))  # length of tropical year (N.solstice to N.solstice) in days hrs:mins:secs
    prev = solstice

# Test AEarth::SouthwardEquinox()
prev = AEarth.SouthwardEquinox(y_begin - 1, eph)
print("\nSouthward Equinoxes")
print("===================")
print("\nJD_UTC                Date       Time     Tz   Tropical Year Length")
print("----------------------------------------------------------------------------")
for year in range(y_begin, y_end):
    equinox = AEarth.SouthwardEquinox(year, eph);
    print(f"{equinox:19.12f}", end='  ')
    eqn.setJD_TT(equinox)
    print(eqn.string(), end='  ')
    print(jd_diff_to_dhms(equinox - prev))  # length of tropical year (September equinox to March equinox) in days hrs:mins:secs
    prev = equinox

# Test AEarth::SouthernSolstice()
prev = AEarth.SouthernSolstice(y_begin - 1, eph)
print("\nSouthern Solstices")
print("==================")
print("\nJD_UTC                Date       Time     Tz   Tropical Year Length")
print("----------------------------------------------------------------------------")
for year in range(y_begin, y_end):
    solstice = AEarth.SouthernSolstice(year, eph);
    print(f"{solstice:19.12f}", end='  ')
    eqn.setJD_TT(solstice)
    print(eqn.string(), end='  ')
    print(jd_diff_to_dhms(solstice - prev))  # length of tropical year (N.solstice to N.solstice) in days hrs:mins:secs
    prev = solstice

print("\nNOTE: The given decimal places are not a representation of the accuracy of the calculation.")
print("      Compare with: https://www.timeanddate.com/astronomy/tropicalyearlength.html")

print("\n\nMean Tropical Year Length - McCarthy & Seidelmann")
print("=================================================")
for year in range(y_begin, y_end):
    edt = EDateTime(year, 1, 1, 0, 0, 0)
    print(year, jd_diff_to_dhms(AEarth.TropicalYearLength(edt.jd_tt())))

#print(AEarth.NutationInObliquity(2451545.0))

eratosthenes = EDateTime(-275,1,1,12,0,0)
myjd_tt = eratosthenes.jd_tt()
nutobl = AEarth.NutationInObliquity(eratosthenes.jd_tt())
print("\n\n")
print("Obliquity at the time of Eratosthenes (200BCE):", ACoord.angle2DMSstring(AEarth.PrecessionVondrak_epsilon(eratosthenes.jd_tt())+nutobl))
print("Using the faster equation from Meeus98:        ", ACoord.angle2DMSstring(AEarth.TrueObliquityOfEcliptic(eratosthenes.jd_tt())))
print("Nutation in obliquity:                         ", ACoord.angle2DMSstring(nutobl))
print("Latitude of modern Aswan (Syene) Old Souk area:", ACoord.angle2DMSstring(math.pi * 24.089244 / 180.0))
eratosthenes.setTime(2024,1,1,12,0,0)
nutobl = AEarth.NutationInObliquity(eratosthenes.jd_tt())
print("Obliquity at the time of testing (2024CE):     ", ACoord.angle2DMSstring(AEarth.PrecessionVondrak_epsilon(eratosthenes.jd_tt())+nutobl))
print("Using the faster equation from Meeus98:        ", ACoord.angle2DMSstring(AEarth.TrueObliquityOfEcliptic(eratosthenes.jd_tt())))
print("Nutation in obliquity:                         ", ACoord.angle2DMSstring(nutobl))

# Note: Below finds max obliquity of period, but should be looking for max obliquity on northern solstices during period.
#       The discovered date in February has a much lower Sun transit altitude
maxval = 0.0
for day in range(30000):
    jd = myjd_tt + day
    val = AEarth.PrecessionVondrak_epsilon(jd) + AEarth.NutationInObliquity(jd)
    if (val > maxval):
        maxval = val
        maxjd = jd

eratosthenes.setJD_TT(jd)
print("Max obliquity using Vondrak:                   ", ACoord.angle2DMSstring(maxval), "at: ", eratosthenes.string(), "  (jd_tt:", maxjd, ")")

maxval = 0.0
for day in range(30000):
    jd = myjd_tt + day
    val = AEarth.TrueObliquityOfEcliptic(jd)
    if (val > maxval):
        maxval = val
        maxjd = jd

eratosthenes.setJD_TT(jd)
print("Max obliquity using Meeus98:                   ", ACoord.angle2DMSstring(maxval), "at: ", eratosthenes.string(), "  (jd_utc:", eratosthenes.jd_utc(), ")")

#print(dir(AEarth))

print("\nPython: Script completed.")
