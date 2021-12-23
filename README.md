# Eartharium
 3D Modeling of Flat and Globe Earth scenarios

## Setup of the sources
Sadly the file paths are currently hard coded to reside in C:\Coding\Eartharium\
There is no graceful failure mode, or explanatory error messages. Search in the source files and correct as needed if your project lives in a different place. This goes for the textures, shaders, scripts, models and AnimOut folders. Hopefully I get to improve this in the near future, but other things have priority at the moment.

This repository does not contain the textures required, because they are too big. I have made them available on the following google drive:
https://drive.google.com/drive/folders/1WgLgTqcOCudFRZltzbIPZ5RkDoORE4gm?usp=sharing
Simply drop the texture folder in the Eartharium/Eartharium folder, next to the shaders and models folders. The .git_ignore file is set to ignore the textures folder.

You will need a python interpreter set up before you can use the python scripting capabilities. I will document the process once the python interface is a bit more useful. Currently there are too many things it cannot do, and you should focus on using the C++ calls directly. The downside is you have to compile every time you add steps in the animation, or stick to doing really simple stuff.


## Compilation
Both Debug and Release targets should work, but you are probably limited to x64 targets. After compiling, start a cmd line in /x64/release and run Eartharium.exe. If stuff fails, there is not much I can do to help, the project is currently not intended for general use. But you are still welcome to file an issue.

## Basic architecture
There are 5 rough translation units (.cpp files); Eartharium, Earth, Astronomy, Primitives and mdoOpenGL. Eartharium has the main function which is the execution entry point. The Application class (in Primitives) holds most global functions and there should only ever be one instance. It is instantiated in Eartharium globally, and passed to python scripts via a getter, or to test area functions via main.

There are a few different types of things that can be displayed during rendering. Most important is the various 3D objects, which are held in a Scene object. Via the Scene, objects such as a Solar System or an Earth a StarBox background, a Shadow caster, a Camera etc. can be created. The rendering of those objects is handled by a RenderLayer3D layer, which is created and managed by the RenderChain. The Scene can contain multiple Camera objects. An Astronomy object takes care of astronomical calculations and progression of astronomical time. Time is currently always in UTC everywhere (mostly represented in Julian days for calculations). A RenderLayer3D takes an Scene, a Camera (which must be in the Scene) and an Astronomy object. It will then render the Scene as seen from the selected Camera, at the Astronomical time specified.

This allows you to set up multiple cameras, and create multiple RenderLayer3D layers with different views of the same Scene. The layer also takes a region on the screen where it will render to. So using one scene with two cameras, you can have a split screen view of the two cameras, while only performing calculations for one Scene and one Astronomy object. Alternatively, two scenes can be used, one with a SolarSystem and one with Earth in details, and they can share the same Astronomy time.

It is also handy to be able to display text on the screen. For this there is a RenderLayerText layer that takes a collection of strings to display. If it is given an Astronomy object, it will display the current UTC time top left of the screen. When the strings are updated, they will be refreshed on screen.

For graphing how different parameters evolve over time or relate to each other, there is a RenderLayerPlot which internally uses ImPlot to draw zoomable graphs. Like the other layers, this too can be placed anywhere on the screen. This layer is new, and like the text layer, it lacks a lot of functionality.

Finally there is a GUI layer. It is not a good idea to create more than one of those, and it should be the last layer so it appears on top. If a RenderLayerGUI layer is defined, then it can be toggled on / off by pressing G in interactive mode (so, not while python scripts are running, unless app.interactive has been set). The GUI will need to be told about the available RenderLayer3D layers manually. These will then appear as Scene tabs at the top of the GUI. RenderLayerGUI then checks to see which objects are present in the attached Scene, and displays basic GUI settings for those objects. It can be used to interactively change things, but the changes are not recorded.

Camera controls are WASD to orbit around the origin, Q/E to increase/decrease Field of View, and Z/C to increase/decrease Camera distance. The RenderLayer3D layer selected in the Scene tabs (yes, they should be called layer tabs, but hey) determines which Camera is being manipulated by the keyboard.

Since it is handy to place observers on Earth and compare what they see, there is a Location object available. This allows to set up arrows towards the Sun, Moon, planets etc. or draw small spheres of how the local sky looks for example. You can create a Location directly and tell it which Earth it belongs to, or you can create a location group in Earth which can then hold multiple Locations and iterate over them.

Earth itself can show insolation in various forms, it can have grid lines for latitude, it can have pointers towards celestial pbjects, etc. When any of these features are enabled (same goes for arrows and paths etc in Locations) those features will update automatically when time is changed in the Astronomy object. To facilitate planetary paths Astronomy can also generate time series that can be shared amongst multiple locations. Thus, the calculations are done once, but 10 or 30 locations can all access the same data. This dramatically increases render speed to practically real time, depending on how many different time seriese you use.

There are also some helper functions like a Lerper that can make animations a bit less tedious, but there is still a lot missing before this is easy to use.

I hope the above helps navigate the admittedly messy and chaotic code.

## Using Eartharium
This project is currently just a hodgepodge of classes that can be chained together for marginally useful animations. Eventually I hope to have a full interface, but for now things are programmed by hand. I will give a few examples of some basic animations in the following.

### Basic Render setup
Begin by creating a function in Eartharium.cpp:

void TestEarth(Application& myapp) {
}

Add a call to it in main(), replacing any TestArea() call (current sources have TestArea5(app); or AngleArcsDev(app)) Your TestEarth() function will now be called when you compile and run Eartharium.

A minimal setup requires a Scene, an Astronomy, a Camera, an Earth and a RenderLayer3D to render it. Add the following to TestEarth:

    RenderChain* chain = myapp.getRenderChain();
    Astronomy* astro = myapp.newAstronomy();
    astro->setTimeNow();
    Scene* scene = myapp.newScene();
    Camera* cam = scene->w_camera; // Default Camera - the Scene must always have at least one
    cam->camFoV = 22.8f;
    cam->camLat = 90.0f;
    cam->camLon =  0.0f;
    cam->camDst = 10.0f;
    cam->CamUpdate();
    myapp.currentCam = cam;  // Manually set which Camera receives keyboard updates
    RenderLayer3D* layer = chain->newRenderLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    Earth* earth = scene->getEarthOb("AENS", 180, 90);
    earth->flatsunheight = 0.0f; // Used for both Subsolar and Sublunar points
    earth->addSubsolarPoint();
    earth->w_sinsol = true;
    earth->addSublunarPoint();
    earth->w_linsol = true;
    earth->addTerminatorTrueSun();
    earth->w_twilight = false;
    earth->addTerminatorTrueMoon();
    chain->do_render(); // Draw the layers, thus rendering the scene.

The above sets up all the required items, and configures Earth to be a flat AE map. It sets up the Camera to look straight down above the north pole. Earth is configured to show the Subsolar and Sublunar points, and show the illuminated areas (day and night), as well as where the Moon can be seen (greyscale vs color). But you will not be able to see this because the function returns as soon as it is done.

Add a loop to wait until you press ESC before exiting, and poll the keyboard to update the various objects:

    myapp.anim = false;
    while (!glfwWindowShouldClose(myapp.window)) {
        myapp.update();
        if (myapp.anim) astro->addTime(0.0, 0.0, 5.0, 0.0);
        scene->w_camera->CamUpdate();
        chain->do_render();
    }

The myapp.update() will process keyboard events and resizing when the window size changes. Use WASD to change the camera view angle. Use Q/E to zoom in/out (Field of View). Hit the space bar to toggle myapp.amin on / off, this will start / stop the time progression. addTime arguments are days, hours, minutes, seconds; all are double floating points and all can be larger than their natural values (e.g. 32.5 hours and 73.8 minutes works fine).

Change the camera view to see the AE map at an oblique angle, then use the N and M keys to morph smoothly between AE map and Normal Sphere (this is what AENS refers to in the call creating the Earth). Press F to toggle btween windowed and full screen mode. Hit ESC or close the window to end the program.

Now add the following lines before the loop, to create a location with arrows indicating the observed (true) directions to the Sun and Moon:

    unsigned int locgroup = earth->addLocGroup();
    earth->locgroups[locgroup]->addLocation(l_cph.lat, l_cph.lon, false, 0.2f);
    earth->locgroups[locgroup]->addLocation(l_nyc.lat, l_nyc.lon, false, 0.2f);
    earth->locgroups[locgroup]->addLocation(l_tok.lat, l_tok.lon, false, 0.2f);
    for (auto& loc : earth->locgroups[longrp]->locations) {
        loc->addLocDot();
        loc->addArrow3DTrueSun();
        loc->addArrow3DTrueMoon();
    }

Now when you compile and run the program, you will see 3 locations marked with little red dots. Each will have a yellow arrow pointing in the direction where an oberver would see the Sun, and a grey one for the Moon. Press space, the arrows will follow the celestial objects they are connected to automatically. Try using N and M to morph the Earth, the arrows will automaticall change according to the surface of the Earth. The cities are defined at the top of Eartharium.cpp, you can add your own. Or you can just speficy the latitude and longitude directly in the addLocation call of course. The 'false' indicates whether the angles are in radians (they are not, hence false). The 0.2f is the radius of the local sky sphere. Add the following inside the location loop right after addARrow3DTrueMoon():

    loc->addLocSky();
    loc->addPath3DTrueSun();
    loc->addPath3DTrueMoon();
    loc->addArrow3DTruePlanet(MERCURY);
    loc->addPlanetTruePath24(MERCURY);

Now compile and run again. You will see a semitransparent ball around each location, this is the local sky. Additionally the 24 hour path of the Moon and Sun have been added. Mercury is also displayed now. The calculations for the paths are time consuming, so the multiple locations will share the data instead of duplicating calculations. Only the last part, which is transforming from geocentric to topographic coordinates is performed for each location.

You might want to see the time and date (in UTC, which is Greenwich time without daylight savings) displayed clearly. For that use the RenderLayerText layer by adding the following lines somewhere after creating the RenderLayer3D layer, but before entering the loop:

    RenderLayerText* text = chain->newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(myapp.m_font2);
    text->setCelestialMech(astro);

That is all, the nullptr means that we are not asking to draw any additional text lines. As soon as RenderLayerText has an Astronomy object, it will show the time. I have yet to implement a function to remove the Astronomy object, but you can do that yourself if you need to switch the time display off.

Take a look at the function declarations for Earth and Location in Earth.h to see which other things you can add. Earth supports latitude/longitude grids, various ground paths and many other objects. They should all seamlessly change according to time and morphing. If you would rather morph between a cylindrical Earth and a rectangular plate carre, then use ERRC (ER = Equirectangular, RC = Right Cylinder). There is even a Williams pear shape named WP, and WGS-84 ellipsoid called E8. So morph between WGS84 and the pear would be called E8WP or WPE8 depending on the direction you need. You cannot do WPE8AE to get more targets though. One is OK, if you don't need morphing at all. If you wanted to animate ER to RC to NS to AE, you would first do ERRC with Earth::param running from 0.0f to 1.0f. Then use Earth::updateType("RCNS") and again run param from 0.0f to 1.0f, then finally NSAE.

Also take a look at how the different functions in Earth and Location are implemented, then it should be easy to add more on your own.
