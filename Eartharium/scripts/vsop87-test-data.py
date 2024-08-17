# Run through VSOP87 test data
# Test data obtained from neoprogrammics.com
# J.Tanner informs that the data was taken from the original FORTRAN VSOP87 source code files
# File is in Eartharium/astronomical_data/VSOP87 test data/vsop87 test data.txt

from eartharium import * 

print("Python VSOP87 test script started")

#VSOP87A EARTH     Date=2000 Jan 01 G   Time=12:00:00 TDB   JD=2451545
# X = -0.1771354586
# Y = +0.9672416237
# Z = -0.0000039000
pos_x = AEarth.VSOP87A_X(2451545.0)
pos_y = AEarth.VSOP87A_Y(2451545.0)
pos_z = AEarth.VSOP87A_Z(2451545.0)
print("Eartharium: ", pos_x, pos_y, pos_z)
print("VSOP87 file:", -0.1771354586, +0.9672416237, -0.0000039000)

with open("C:\\Coding\Eartharium\\astronomical_data\\VSOP87 test data\\vsop87 test data.txt") as testfile:
    testlines = testfile.readlines()

for line in testlines:
    if len(line) < 10: continue
    if line[1:3] == "==": print(line)

