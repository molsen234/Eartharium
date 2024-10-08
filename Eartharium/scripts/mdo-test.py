# Access the embedded bridge module
from eartharium import *
#import eartharium

print("Python: Script loaded Eartharium module.")

## NOTE: with 'import eartharium', create objects from eartharium by using the full name like color1 = eartharium.vec4(0.8,0.8,0.8,1.0)
##       Or use: 'from eartharium import *' which makes for terser code like color1 = vec4(0.8,0.8,0.8,1.0)
##       ( some idiot spent a whole day troubleshooting color1 = vec4(0.8,0.8,0.8,1.0) crashes with 'import eartharium' !!! )

# Set up environment
app = getApplication()
print("Got app: ", app)

# Interactive mode works, but you must manually call app.render() after modifying
#  Of course keyboard interactions are also not processed
app.interactive = False  # Should we drop to Python interactive console at the end?

# Scene - where all renerable objects live
scene = app.newScene()
print("Got scene: ", scene)

# Astronomy - Where all astronomical calculations are performed
astro = app.newAstronomy()
print("Got astronomy: ", astro)

# Camera - How to observe the Scene (a scene can have multiple cameras if desired, but always has at least one default camera)
cam = scene.w_camera # Pick up default camera
print("Got camera: ", cam)
cam.camFoV = 12.0
cam.camLat =  0.0
cam.camLon =  0.0
cam.camDst = 10.0
app.currentCam = cam
print("- configured camera")

# RenderLayer3D - A viewported layer where a Scene(Camera) can be rendered
layer = app.newLayer3D(vpx1 = 0.0, vpy1 = 0.0, vpx2 = 1.0, vpy2 = 1.0, scene = scene, astro = astro, cam = cam)
print("Got new Layer3D: ",layer)

# RenderLayerText - viewported text layer that can will overlay any lower layers. Layers are rendered in the order they are defined.
text = app.newLayerText(vpx1 = 0.0, vpy1 = 0.0, vpx2 = 1.0, vpy2 = 1.0) # let lines default to nullptr
text.setFont(app.m_font2) # m_font2 is a large 36 point font, m_font1 is a smaller 16 point font, both monospace to avoid jitter when rapidly updating values
text.setAstronomy(astro)
print("Got new LayerText: ", text)

# RenderLayerGUI - viewported GUI Layer that will display/hide the dynamic GUI when pressing 'g'
gui = app.newLayerGUI(vpx1 = 0.0, vpy1 = 0.0, vpx2 = 1.0, vpy2 = 1.0)
print("Got new LayerGUI: ", gui)
gui.addLayer3D(layer, "3D Scene Name")

# Build the scenario
earth = scene.newEarth("AENS", 360, 180)
app.currentEarth = earth;
earth.param = 1.0

earth.addSubsolarPoint()
earth.flatsunheight = 0.0
earth.addGrid(15.0, 0.002, vec4(0.8, 0.8, 0.8, 1.0), "LALO", False, False, True)
#earth.addGrid(15.0, 0.002, eartharium.vec4(0.8, 0.8, 0.8, 1.0), "LALO", False, False, False)
earth.addEquator()
earth.addTropics()
earth.addEcliptic()
earth.tropicsoverlay = False  # Toggle tropics shading on/off with True/False

print("Initialized Earth.")

#lg = earth.addLocGroup()
#l = earth.locgroups[lg].addLocation(lat = 0.0, lon = 0.0, rad = False)
#l.addLocDot()

#earth.addGreatArc(LLH(-33.888, 18.385, 0.0), LLH(40.450,-73.823, 0.0))

# mydate1 = EDateTime(2459571.173414352)   # JD in UTC
# mydate1.setTime(2021,12,22,16,9,43)
# mydate2 = EDateTime(1681623963)  # Unix timestamp
# print(mydate1.string())
# print(mydate2.string())
# print("Past testing of EDateTime in python")

## How to use static functions:
#print(EDateTime.getDateTime2JD_UTC(2021,12,22,16,9,43))

# Do updates
astro.setTime(2021,12,22,16,9,43)
print(astro.getTimeString())
app.update()
#app.render_out = True;   # render to files
app.render()

#frames = 360
#for f in range(0,frames):    # Forward 1 day at calm pace
#    astro.addTime(0,24/frames,0,0)
#    app.render()
#for f in range(30):      # Pause for a moment (1 second at 30fps)
#    app.render()
#rev_frames = int(frames/3)
#for f in range(0,rev_frames): # Reverse the day at 4x speed
#    astro.addTime(0,-24/rev_frames,0,0)
#    app.render()
#for f in range(0,89): # animate to equinox
#    astro.addTime(0,24,0,0)
#    app.render()
#for f in range(0,frames):    # Forward 1 day at calm pace
#    astro.addTime(0,24/frames,0,0)
#    app.render()
#for f in range(30):      # Pause for a moment
#    app.render()
#for f in range(0,rev_frames): # Reverse the day at 4x speed
#    astro.addTime(0,-24/rev_frames,0,0)
#    app.render()

while (not app.shouldClose()):
    #astro.setTimeNow()
    if app.anim:
        astro.addTime(0.0, 0.0, 10.0, 0.0)

    #app.update()
    app.render()

# Debug - https://www.digitalocean.com/community/tutorials/how-to-debug-python-with-an-interactive-console
# This might be expanded into a full interactive loop including render calls, so main c++ app is not frozen while this runs
# <Ctrl>Z, <Enter> to exit
if (app.interactive == True):
    import code
    code.interact(local=locals())

print("Python: Script completed.")