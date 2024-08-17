# Access the embedded bridge module
import eartharium
print("Python: Script loaded Eartharium module.")

# Set up environment
app = eartharium.getApplication()
#print(app)
chain = app.getRenderChain()
#print(chain)
scene = app.newScene()
#print(scene)
astro = app.newAstronomy()
#print(astro)
cam = scene.w_camera
#print(cam)

layer = chain.newLayer3D(vpx1 = 0.0, vpy1 = 0.0, vpx2 = 1.0, vpy2 = 1.0, scene = scene, astro = astro, cam = cam)
#print(layer)

# Build the scenario
earth = scene.newEarth("AENS", 180, 90)
#print(earth)
#lg = earth.addLocGroup()
#print(lg)
#l = earth.locgroups[lg].addLocation(lat = 0.0, lon = 0.0, rad = False)
#print(l)
#l.addLocDot()
#earth.addGreatCircle(LLH(lat = -33.888,18.385,0.0), LLH(40.450,-73.823, 0.0))

# Do updates
astro.setTimeNow()
app.update()
print("App updated")
cam.CamUpdate()
print("Cam updated")

chain.do_render()

# Debug - https://www.digitalocean.com/community/tutorials/how-to-debug-python-with-an-interactive-console
# This might be expanded into a full interactive loop including render calls, so main c++ app is not frozen while this runs
# <Ctrl>Z, <Enter> to exit
if (app.ipython == True):
    import code
    code.interact(local=locals())

print("Python: Script completed.")