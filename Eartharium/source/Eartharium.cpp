
#include <chrono>  // For comparing execution times of different functions.

// Protos
void GLClearError();
void GLPrintError();

//#include "OpenGL.h"
#include "Astronomy.h"
#include "Renderer.h"
#include "Primitives.h"
//#include "SceneObjects.h"
#include "Earth.h"
#include "OldObjects.h"
//#include "Utilities.h"
//  #include "astronomy/acoordinates.h"
//  #include "astronomy/aearth.h"
//#include "FigureGeometry.h"

#include "Geometry.h"

#include "../../EAstronomy/all.h"

#include "pythoninterface.h"

// -----------
//  Locations
// -----------
// Just a few handy locations. Eventually a location manager should load from a file and offer interactive maintenance !!!
LLD l_gnw{ 51.477811, 0.001475, 0.0 };      // Greenwich Royal Observatory, UK
LLD l_qqt{ 69.243574, -53.540529, 0.0 };    // Quequertarsuaq, Greenland
LLD l_cph{ 55.6761, 12.5683, 0.0 };         // Copenhagen, Denmark 55.6761° N, 12.5683° E
LLD l_nyc{ 40.7182, -74.0060, 0.0 };        // New York, USA 40.7128° N, 74.0060° W
LLD l_tok{ 35.668009, 139.773577, 0.0 };    // Tokyo, Japan 35.668009, 139.773577
LLD l_sye{ 24.0889, 32.8998, 0.0 };         // Syene, Egypt 24.0889° N, 32.8998° E
LLD l_alx{ 31.2001, 29.9187, 0.0 };         // Alexandria, Egypt 31.2001° N, 29.9187° E
LLD l_clm{ 6.9271, 79.8612, 0.0 };          // Colombo, Sri Lanka 6.9271° N, 79.8612° E
LLD l_rig{ -53.822025, -67.651686, 0.0 };   // Rio Grande, Argentina - 53.822025, -67.651686
LLD l_ams{ 52.3676, 4.9041, 0.0 };          // Amsterdam, The Netherlands 52.3676° N, 4.9041° E
LLD l_perth{ -31.942045, 115.963123, 0.0 }; // Perth airport(PER) Australia
LLD l_sydney{ -33.8688, 151.2093, 66.0 };      // Sydney Australia according to google maps (incl elevation)
LLD l_cptwn{ -33.974195, 18.602728, 0.0 };  // Cape Town airport South Africa
LLD l_puare{ -53.002324, -70.854556, 0.0 }; // Punta Arenas airport(PUQ) Chile
LLD l_stiag{ -33.3898, -70.7944, 0.0 };     // Santiago airport(SCL) Chile
LLD l_sydny{ -33.9399, 151.1753, 0.0 };     // Sydney airport(SYD) Australia
LLD l_buair{ -34.818463, -58.534176, 0.0 }; // Buenos Aires airport(EZE) Argentina
LLD l_frafu{ 50.037967, 8.561057, 0.0 };    // Frankfurt airport(FRA) Germany
LLD l_seoul{ 37.468028, 126.442011, 0.0 };  // Seoul airport(ICN) South Korea
LLD l_kabul{ 34.543447, 69.177123, 1817.522 };  // Kabul FlatEarthIntel.com measurepoint
LLD l_jamaica{ 18.1096, -77.2975, 0.0 };    // Jamaica according to google. For Columbus Eclipse
LLD l_ugc{ -79.766666666667, -82.866666666667, 0.0 };           // Union Glacier Camp - for The Final Experiment
LLD l_pua{ -53.159331, -70.888583, 0.0 };   // Union Glacier Camp - for The Final Experiment

// Places to check webcams for sunrise/sunset
LLD l_phuket{ 7.891579, 98.295930, 0.0 };   // Phuket west facing (sunset) webcam: https://www.webcamtaxi.com/en/thailand/phuket/patong-beach-resort.html


// ---------------
//  Solar Eclipse
// ---------------
// Currently unused obviously. Here as a reminder to implement such a class
class SolarEclipse {
    // This is the stuff I'd love to make: https://www.youtube.com/watch?v=IaBt5Zau3ng&ab_channel=NASAScientificVisualizationStudio
    // Along with showing where the Sun and Moon are in relation to Earth, with shadow cones for umbra, penumbra and antumbra
public:
};




void TimeGetDetails(Application& app) {
    Astronomy* astro = app.newAstronomy();
    astro->setTime(-200, 9, 23.0, 0.0, 0.0, 0.0);
    //astro->setTimeNow();

    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    CelestialDetail old;
    double count = 1000; // 100'000;
    double i = 0;
    start = std::chrono::high_resolution_clock::now();
    //old = astro->getDetails(astro->getJD_TT(), URANUS, ECGEO);
    //return;
    for (i = 0; i < count; i++) {
    //    old = astro->getDetails(astro->getJD_TT(), URANUS, ECGEO);
    }
    stop = std::chrono::high_resolution_clock::now();
    std::cout << " Time in us:  " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << '\n';
    std::cout << "RA/Dec: " << rad2hrs * old.geq.lon << ", " << rad2deg * old.geq.lat << "\n";
}


//  #include "astronomy/amoon.h"
//#include "AAplus/AAMoon.h"
//#include "AAplus/AAELP2000.h"
//#include "AAplus/AAELPMPP02.h"

void testLunarPosition(Application& app) {
    // Moon - 2024-04-08 12:00 UTC

    // My library (converted from AA+ to always use radians)
    Astronomy* astro = app.newAstronomy();
    //astro->setTime(2024, 4, 8.0, 12.0, 0.0, 0.0);
    //astro->setTime(1992, 4, 12.0, 0.0, 0.0, 0.0);
    astro->setJD_UTC(2448724.5); // same as astro->setTime(1992, 4, 12.0, 0.0, 0.0, 0.0);
    double jd_tt = astro->getJD_TT();
    jd_tt = 2448724.5;

    // True Geocentric Ecliptic Spherical coordinates
    LLD meeusa1 = AMoon::EclipticCoordinates(jd_tt, MEEUS_SHORT);
    LLD meeusb1 = astro->MoonApparentEcliptic(jd_tt, MEEUS_SHORT);
    LLD elp2ka1 = AMoon::EclipticCoordinates(jd_tt, ELP2000_82);
    LLD elp2kb1 = astro->MoonTrueEcliptic(jd_tt, ELP2000_82);
    LLD elp2kc1 = AELP2000::EclipticCoordinates(jd_tt);
    LLD elp2kard = elp2kc1;
    elp2kard += AEarth::EclipticAberration(elp2kard.lon, elp2kard.lat, jd_tt, EPH_VSOP87_SHORT);
    elp2kard += FK5::CorrectionInLonLat(elp2kard, jd_tt);
    elp2kard.lon += AEarth::NutationInLongitude(jd_tt);
    const double epsilon = AEarth::TrueObliquityOfEcliptic(jd_tt); // Mean Obliquity + Nutation in Obliquity
    //const double epsilon = AEarth::TrueObliquityOfEcliptic(JD_2000); // Mean Obliquity + Nutation in Obliquity
    elp2kard = Spherical::Ecliptic2Equatorial(elp2kard, epsilon);
    elp2kard = AEarth::PrecessEquatorialJ2000(elp2kard, jd_tt);

    LLD mpp02a1 = AMoon::EclipticCoordinates(jd_tt, ELP_MPP02);
    LLD mpp02b1 = astro->MoonTrueEcliptic(jd_tt, ELP_MPP02);
    glm::dvec3 mrect = AELP2000::EclipticRectangularCoordinates(jd_tt);
    glm::dvec3 mrfk5 = AELP2000::EquatorialRectangularCoordinatesFK5(jd_tt);  // In equinox of J2000
    LLD mfk5 = Spherical::Rectangular2Spherical(mrfk5);
    LLD mdate = AEarth::PrecessEquatorialJ2000(mfk5, jd_tt);             // In equinox of date

    std::cout << "\n\nMOON " << astro->timestr << " -> jd_tt: " << jd_tt << " - MDO Library" << std::endl;
    std::cout << "Ephemeris coordinates: " << std::endl;
    std::cout << " MDO Meeus Short: " << meeusa1.str_EC() << " - " << ACoord::angle2DMSstring(meeusa1.lat) << "," << ACoord::angle2uDMSstring(meeusa1.lon) << std::endl;
    std::cout << " MDO ELP2000-82:  " << elp2ka1.str_EC() << " - " << ACoord::angle2DMSstring(elp2ka1.lat) << "," << ACoord::angle2uDMSstring(elp2ka1.lon) << std::endl;
    std::cout << " LLD ELP2000-82:  " << elp2kc1.str_EC() << " - " << ACoord::angle2DMSstring(elp2kc1.lat) << "," << ACoord::angle2uDMSstring(elp2kc1.lon) << std::endl;
    std::cout << " LLD elp2kard:    " << elp2kard.str_EC() << " - " << ACoord::angle2DMSstring(elp2kard.lat) << "," << ACoord::angle2uHMSstring(elp2kard.lon) << std::endl;
    std::cout << " MDO ELP2000-82 Rectangular:  " << mrect.x << "," << mrect.y << "," << mrect.z << std::endl;
    std::cout << " MDO ELP2000-82 EQ FK5@J2000: " << mrfk5.x << "," << mrfk5.y << "," << mrfk5.z << std::endl;
    std::cout << " MDO ELP2000-82 FK5@J2000: " << mfk5.str_EC() << " - " << ACoord::angle2DMSstring(mfk5.lat) << "," << ACoord::angle2uHMSstring(mfk5.lon) << std::endl;
    std::cout << " MDO ELP2000-82 FK5@date: " << mdate.str_EC() << " - " << ACoord::angle2DMSstring(mdate.lat) << "," << ACoord::angle2uHMSstring(mdate.lon) << std::endl;
    std::cout << " MDO ELPMPP02:    " << mpp02a1.str_EC() << " - " << ACoord::angle2DMSstring(mpp02a1.lat) << "," << ACoord::angle2uDMSstring(mpp02a1.lon) << std::endl;
    std::cout << std::endl;


    // AA+ - P.J.Naughter
    std::cout << "MOON - AA+ v2.49 - P.J.Naughter" << std::endl;
    std::cout << "Ephemeris coordinates: " << std::endl;
    //std::cout << " AA+ Meeus Short: "
    //    << CAAMoon::EclipticLatitude(jd_tt) << ","
    //    << CAAMoon::EclipticLongitude(jd_tt) << ","
    //    << CAAMoon::RadiusVector(jd_tt) << std::endl;
    //std::cout << " AA+ ELP2000-82:  "
    //    << CAAELP2000::EclipticLatitude(jd_tt) << ","
    //    << CAAELP2000::EclipticLongitude(jd_tt) << ","
    //    << CAAELP2000::RadiusVector(jd_tt) << std::endl;
    //CAA3DCoordinate rect{ CAAELP2000::EclipticRectangularCoordinates(jd_tt) };
    //std::cout << " AA+ ELP2000-82 Rectangular: " << rect.X << "," << rect.Y << "," << rect.Z << std::endl;
    //CAA3DCoordinate rfk5{ CAAELP2000::EquatorialRectangularCoordinatesFK5(jd_tt) };
    //std::cout << " AA+ ELP2000-82 EQ Rect FK5: " << rfk5.X << "," << rfk5.Y << "," << rfk5.Z << std::endl;
    //LLD sfk5 = Spherical::Rectangular2Spherical({ rfk5.X, rfk5.Y, rfk5.Z });
    //std::cout << " LLD ELP2000-82 FK5: " << sfk5.str_EC() << " - " << astro->angle2DMSstring(sfk5.lat, true) << "," << astro->angle2uHMSstring(sfk5.lon, true) << std::endl;
    //std::cout << " AA+ ELPMPP02:    "
    //    << CAAELPMPP02::EclipticLatitude(jd_tt) << ","
    //    << CAAELPMPP02::EclipticLongitude(jd_tt) << ","
    //    << CAAELPMPP02::RadiusVector(jd_tt) << std::endl;

    // NOTE: When comparing the above to Stellarium, the following facts are important:
    //       1) Stellarium displays and takes JD in UTC!
    //       2) Switch off topocentric coordinates!
    //       3) Switch off atmosphere!
    //       4) Switch off aberration.
    //       5) Meeus Short returns Ecliptic of date directly.
    //       6) ELP2000_82 returns FK5 @J2000 when using EquatorialRectangularCoordinatesFK5()
    //astro->setJD_UTC(JD_2000);
    //std::cout << "\n\n J2000.0 UTC: " << astro->getJD_UTC() << " TT: " << astro->getJD_TT() << std::endl;

    // Stellar Apparent Position by Name
    //astro->setTime(2024, 5, 19.0, 12.0, 0.0, 0.0);
    //LLD decra = astro->ApparentStellarPositionbyName("Sirius", NO_DOUBLE);
    //std::cout << "\n\nSirius Apparent Position (Dec,RA): " << astro->formatDecRA(decra, true) << std::endl;
    // Example from Meeus and forum post
    astro->setTime(2028, 11 , 13.0, 3.0, 33.0, 35.0);
    LLD decra = astro->ApparentStellarPositionbyName("* tet Per", NO_DOUBLE);
    std::cout << "\n\nTheta Persei Apparent Position (Dec,RA): " << ACoord::formatDecRA(decra) << "\n\n" << std::endl;

    astro->explainApparentStellarPositionbyName("* tet Per");
    astro->explainApparentStellarPositionbyName("Sirius");
    astro->explainApparentStellarPositionbyName("Polaris");

}

void testMoonPhases(Application& app) {
    EDateTime datestrK{ 1977, 2, 1.0, 0.0, 0.0, 0.0 };
    EDateTime datestr{ 1977, 2, 1.0, 0.0, 0.0, 0.0 };
    //EDateTime datestr{ 2000, 1, 1.0, 0.0, 0.0, 0.0 };
    double moonjd = datestr.jd_tt();
    std::cout << "moonjd: " << moonjd << "  K: " << AMoon::Phase_K((moonjd - JD_0000) / JD_YEAR) << std::endl;

    datestrK.setJD_TT(AMoon::TruePhaseK(-283.0));
    std::cout << "A First New Moon February 1977: " << datestrK.string() << std::endl;    // MEEUS98 example 49.a
    datestr.setJD_TT(AMoon::TruePhase(moonjd, NEW_MOON));
    std::cout << "M First New Moon February 1977: " << datestr.string() << std::endl;     // MEEUS98 example 49.a

    datestrK.setJD_TT(AMoon::TruePhaseK(-282.75));
    std::cout << "A First Qua Moon February 1977: " << datestrK.string() << std::endl;
    datestr.setJD_TT(AMoon::TruePhase(moonjd, FIRST_QUARTER_MOON));
    std::cout << "M First Qua Moon February 1977: " << datestr.string() << std::endl;

    datestrK.setJD_TT(AMoon::TruePhaseK(-283.50));
    std::cout << "A First Ful Moon February 1977: " << datestrK.string() << std::endl;
    datestr.setJD_TT(AMoon::TruePhase(moonjd, FULL_MOON));
    std::cout << "M First Ful Moon February 1977: " << datestr.string() << std::endl;

    datestrK.setJD_TT(AMoon::TruePhaseK(-283.25));
    std::cout << "A Third Qua Moon February 1977: " << datestrK.string() << std::endl;
    datestr.setJD_TT(AMoon::TruePhase(moonjd, THIRD_QUARTER_MOON));
    std::cout << "M Third Qua Moon February 1977: " << datestr.string() << std::endl;

    std::cout << std::endl;
    EDateTime ex49b{ 2044, 1, 1.0, 0.0, 0.0, 0.0 };
    double ex49b_jd = ex49b.jd_tt();
    std::cout << "First Last Quarter Moon 2044: " << AMoon::TruePhaseK(544.75) << std::endl;                // MEEUS98 example 49.b
    std::cout << "First Last Quarter Moon 2044: " << AMoon::TruePhase(ex49b_jd, THIRD_QUARTER_MOON) << std::endl;  // MEEUS98 example 49.b
    std::cout << "First New Moon 2044: " << AMoon::TruePhaseK(545.0) << std::endl;
    std::cout << "First New Moon 2044: " << AMoon::TruePhase(ex49b_jd, NEW_MOON) << std::endl;
    std::cout << "First First Quarter Moon 2044: " << AMoon::TruePhaseK(544.25) << std::endl;
    std::cout << "First First Quarter Moon 2044: " << AMoon::TruePhase(ex49b_jd, FIRST_QUARTER_MOON) << std::endl;
    std::cout << "First Full Moon 2044: " << AMoon::TruePhaseK(544.50) << std::endl;
    std::cout << "First Full Moon 2044: " << AMoon::TruePhase(ex49b_jd, FULL_MOON) << std::endl;

    EDateTime jd_zero{ JD_0000, true };
    std::cout << "DateTime: " << jd_zero.stringms() << " jd_tt: " << jd_zero.jd_tt() << " jd_utc: " << jd_zero.jd_utc() << std::endl;
}

void testLunarNodes(Application& app) {
    EDateTime datestrK{ 1987, 5, 1.0, 0.0, 0.0, 0.0 };
    EDateTime datestr{ 1987, 5, 1.0, 0.0, 0.0, 0.0 };
    double moonjd = datestr.jd_tt();
    std::cout << "moonjd: " << moonjd << "  K: " << AMoon::Node_K((moonjd - JD_0000) / JD_YEAR) << std::endl;

    datestrK.setJD_TT(AMoon::PassageThroNodeK(-170.0));
    std::cout << "A Ascending Node May 1987:  " << datestrK.string() << std::endl;    // MEEUS98 example 51.a
    datestr.setJD_TT(AMoon::PassageThroNode(moonjd, ASCENDING));
    std::cout << "M Ascending Node May 1987:  " << datestr.string() << std::endl;     // MEEUS98 example 51.a

    datestrK.setJD_TT(AMoon::PassageThroNodeK(-170.5));
    std::cout << "A Descending Node May 1987: " << datestrK.string() << std::endl;
    datestr.setJD_TT(AMoon::PassageThroNode(moonjd, DESCENDING));
    std::cout << "M Descending Node May 1987: " << datestr.string() << std::endl;
}


//#include "AAplus/AAElliptical.h"
//  #include "astronomy/aelliptical.h"
//  #include "astronomy/amars.h"
void testAElliptical(Application& app) {
    EDateTime marsdate{ 2024, 5, 19.0, 12.0, 0.0, 0.0 };

    // MDO implementation
    APlanetaryDetails marsdetm = AElliptical::getPlanetaryDetails(marsdate.jd_tt(), A_MARS, EPH_VSOP87_FULL);
    std::cout << "MDO Mars: " << ACoord::formatDecRA(marsdetm.ageqs) << std::endl;
    // PJN implementation
    //CAAEllipticalPlanetaryDetails marsdeta = CAAElliptical::Calculate(marsdate.jd_tt(), CAAElliptical::Object::MARS, true);
    //std::cout << "AA+ Mars: " << Astronomy::formatDecRA({ marsdeta.ApparentGeocentricDeclination, marsdeta.ApparentGeocentricRA * 15, marsdeta.ApparentGeocentricDistance }, false) << std::endl;
    // Output:
    // MDO Mars: 004°33'35.789" / 000h55m04.054s
    // AA+ Mars: 004°33'35.789" / 000h55m04.054s
    // Matches Stellarium 23.1 exactly to the accuracy of Stellarium output (0.1") with Atmosphere and Topocentric turned off

    // Meeus98 example 33.a
    // Venus at 1992 Dec 20 0:00:00 tt, jde 2448976.5
    //EDateTime venusdate{ 1992, 12, 20.0, 0.0, 0.0, 0.0 };   // No, because Meeus gives the date in dynamical time (TT)
    EDateTime venusdate{ 2448976.5 , true }; // Set JD_TT
    APlanetaryDetails venusdetm = AElliptical::getPlanetaryDetails(venusdate.jd_tt(), A_VENUS, EPH_VSOP87_SHORT);
    std::cout << "MDO Venus (short VSOP87): " << ACoord::formatDecRA(venusdetm.ageqs) << std::endl;
    // Output: MDO Venus (short VSOP87): -18°53'16.909" / 021h04m41.488s
    // MEEUS98:                          -18°53'16.8"   / 021h04m41.50s

    //Physical Mars
    EDateTime pmarsdate{ 1992, 11, 9.0, 0.0, 0.0, 0.0 };
    APhysicalMarsDetails physmars{ AMars::calcPhysicalMarsDetails(pmarsdate.jd_tt(), EPH_VSOP87_FULL) };
    physmars.print();
}


void testDetailedEarth(Application& app) {
    const float radius_gp = 0.01f;
    Astronomy* astro = app.newAstronomy();
    //astro->setTime(2024, 9, 18.0, 2.0, 44.0, 14.1); // https://www.eclipsewise.com/lunar/LEorth/2001-2100/LE2024Sep18P.gif
    //astro->setTime(2024, 9, 18.0, 2.0, 44.0, 14.0);
    //astro->setTime(1992, 4, 12.0, 0.0, 0.0, 0.0); // Given in TT, so convert
    //astro->setJD_TT(astro->getJD_UTC()); // Ex 47.a
    astro->setTime(2024, 4, 23.0, 8.0, 26.0, 0.0);

    std::cout << astro->getTimeString() << std::endl;
    Scene* scene = app.newScene();
    scene->astro = astro;
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "EarthView");

    DetailedEarth* erf = new DetailedEarth(scene, nullptr, "NSAE", 180, 90, 1.0f);
    app.currentEarth2 = erf;
    erf->inlunation = false;
    //Earth* erf = scene->newEarth("NSAE", 180, 90);
    //app.currentEarth = erf;
    //erf->w_linsol = false;
    erf->w_twilight = true;
    //erf->addEquator();
    //erf->addPrimeMeridian();
    erf->addArcticCircles();
    //erf->removeArcticCircles();
    erf->addTropicCircles();
    erf->addSunGP();
    erf->sungp->setRadius(radius_gp);
    //erf->position = { 0.0f, 0.5f, 0.0f };
    //erf->addGrid(15.0);
    //erf->addCelestialSphere();
    //erf->celestialgrid->setSpacing(24);
    //erf->gridOb->setColor(LIGHT_GREY);
    erf->addEcliptic();
    //erf->addMoonGP();
    //erf->moongp->setRadius(radius_gp);

    double jd_tt = astro->getJD_TT();
    const double epsilon = AEarth::TrueObliquityOfEcliptic(jd_tt);
    const double agst = AEarth::ApparentGreenwichSiderealTime(astro->getJD_UTC());
    //LLD obsloc{ deg2rad * 55.6761, deg2rad * 12.5683, 0.0 };
    LLD obsloc{ deg2rad * 47.567, deg2rad * -52.717, 0.0 };

    // Moon position using Meeus short ELP 2000
    LLD meeuspos = astro->MoonApparentEcliptic(jd_tt, MEEUS_SHORT);
    //meeuspos.lon += AEarth::NutationInLongitude(jd_tt);  // Already included in AMoon::Ecliptic_Meeus_Short()
    meeuspos.dst *= km2au;
    std::cout << meeuspos.dst << "\n";
    std::cout << ACoord::formatLatLon(meeuspos) << "\n";
    meeuspos = Spherical::Ecliptic2Equatorial(meeuspos, epsilon);
    meeuspos = AEarth::Equatorial2Topocentric(meeuspos, obsloc, agst);
    std::cout << ACoord::formatDecRA(meeuspos) << "\n";

    // Using full theory
    LLD moonpos{};
    moonpos = AELP2000::EclipticCoordinates(jd_tt);
    std::cout << "ELP2000-82 ecliptic position:     " << ACoord::formatLatLon(moonpos) << "\n";
    //moonpos = AELPMPP02::EclipticCoordinates(jd_tt);
    //std::cout << "ELPMPP02 ecliptic position:     " << ACoord::formatLatLon(moonpos) << "\n";
    moonpos.dst *= km2au;
    double moondst = moonpos.dst;
    LLD fk5 = FK5::CorrectionInLonLat(moonpos, jd_tt);
    LLD aberration = AEarth::EclipticAberration(moonpos.lon, moonpos.lat, jd_tt, EPH_VSOP87_FULL);
    moonpos += fk5;
    moonpos += aberration;
    std::cout << "Astrometric ecliptic position:    " << ACoord::formatLatLon(moonpos) << "\n";
    moonpos.lon += AEarth::NutationInLongitude(jd_tt);
    std::cout << "Nutated ecliptic position:        " << ACoord::formatLatLon(moonpos) << "\n";

    // Precession
    moonpos = AEarth::PrecessEcliptic(moonpos.lon, moonpos.lat, EDateTime::getJDUTC2TT(JD_2000), jd_tt);
    moonpos.dst = moondst;
    std::cout << "Precessed ecliptic position:      " << ACoord::formatLatLon(moonpos) << "\n";
    moonpos = Spherical::Ecliptic2Equatorial(moonpos, epsilon);
    //moonpos = AEarth::PrecessEquatorial(moonpos, EDateTime::getJDUTC2TT(JD_2000), jd_tt);
    std::cout << "Precessed equatorial position:    " << ACoord::formatDecRA(moonpos) << "\n";
    moonpos = AEarth::Equatorial2Topocentric(moonpos, obsloc, agst);

    // Topocentric
    std::cout << "Topocentric equatorial position:  " << ACoord::formatDecRA(moonpos) << "\n";
    
    std::cout << "Geocentric Distance (km / AU):    " << moonpos.dst << " / " << moonpos.dst * km2au << "\n";
    std::cout << "Obliquity of Ecliptic:            " << ACoord::angle2DMSstring(epsilon) << " (" << epsilon << ")" << "\n";
    std::cout << "Apparent Greenwich Sidereal Time: " << ACoord::angle2uHMSstring(agst) << " (" << agst << ")" << "\n";

    std::cout << "Observer Location:                " << ACoord::formatLatLon(obsloc) << "\n";
    std::cout << "Apparent Local Sidereal Time:     " << ACoord::angle2uHMSstring(agst + obsloc.lon) << " (" << agst + obsloc.lon << ")" << "\n";

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        if (app.anim) {
            //astro->setTimeNow();
            astro->addTime(0.0, 0.0, 4.0, 0.0);
            //astro->addTime(0.0, 0.0, 0.0, 31558149.504); // Sidereal year in seconds
        }

        //app.anim = false; // Nice for single step action. <space> will set app.anim in app.render, and we get back here one frame later.
        app.render();
    }

}



void testDetailedSky(Application& app) {
    Astronomy* astro = app.newAstronomy();
    astro->setTime(2024, 4, 8.0, 18.0, 40.0, 0.0);
    //std::cout << astro.getTimeString() << std::endl;
    Scene* scene = app.newScene();
    scene->astro = astro;
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "SkyView");

    DetailedSky* sky = new DetailedSky(scene, nullptr, "NSAE", 180, 90, 1.0f);
    app.currentEarth2 = sky;

    sky->addStars(5.0);
    sky->addGridEquatorial();
    sky->addPrecessionPath();

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        if (app.anim) {
            //astro->setTimeNow();
            astro->addTime(0.0, 0.0, 4.0, 0.0);
            //astro->addTime(0.0, 0.0, 0.0, 31558149.504); // Sidereal year in seconds
        }

        //app.anim = false; // Nice for single step action. <space> will set app.anim in app.render, and we get back here one frame later.
        app.render();
    }

}




void LunarData(Application& app) {
    Astronomy* astro = app.newAstronomy();

    astro->setJD_UTC(JD_2000);
    std::cout << JD_2000 << " " << astro->getJD_TT() << "\n";

    // Trying to match J.Meus 1992 Astronomical Algorithms SE, example 47.a (page 342)
    astro->setTime(1992, 4, 12.0, 0.0, 0.0, 0.0);  // Meus example 47.a

    double currentJD = astro->getJD_UTC();         // Should probably be JD_TT. But JD_UTC gives the exact result of Meus example 47.a
    double elon = AMoon::EclipticLongitude(currentJD);  // lambda
    double elat = AMoon::EclipticLatitude(currentJD);   // beta
    double Epsilon = AEarth::TrueObliquityOfEcliptic(currentJD);
    LLD equa = Spherical::Ecliptic2Equatorial2(elon, elat, Epsilon);
    double moonDist = AMoon::RadiusVector(currentJD); // RadiusVector() returns in km
    double moonRA = hrs2rad * equa.lon;
    double moonDec = deg2rad * equa.lat;
    // precessing the epoch is needed below when matching NASAs 2023 lunar table (see below), not with Meus' example.
    // CAA2DCoordinate j2k = CAAPrecession::PrecessEquatorial(equa.X, equa.Y, astro->getJD_UTC(), JD_2000);

    //std::cout << astro->getTimeString() << " " << astro->radecFormat(astro->rangezero2tau(moonRA), astro->rangepi2pi(moonDec), true) << "\n";
    std::cout << astro->getTimeString() << " " << equa.lon << " / " << equa.lat << "\n";
    std::cout << astro->getJD_UTC() - astro->getJD_TT() << "  " << elon << "," << elat << " | " << moonDist << ", True Obliq: " << Epsilon << "\n\n\n";
    // Above is completely accurate as per Meus example 47.a

    // Trying to match https://svs.gsfc.nasa.gov/vis/a000000/a005000/a005048/mooninfo_2023.txt
    // Which is a data dump of this: https://svs.gsfc.nasa.gov/5048
    // (Moon phases and libration 2023)
    LLD j2k{};
    for (double i = 0; i < 30; i++) {
        astro->setTime(2023, 1, 1.0, i, 0.0, 0.0);
        currentJD = astro->getJD_TT();
        elon = AMoon::EclipticLongitude(currentJD);  // lambda
        elat = AMoon::EclipticLatitude(currentJD);   // beta
        Epsilon = AEarth::TrueObliquityOfEcliptic(currentJD);
        equa = Spherical::Ecliptic2Equatorial2(elon, elat, Epsilon);
        moonDist = AMoon::RadiusVector(currentJD); // RadiusVector() returns in km - seems consistently 40km low for Jan 2023.
        //moonRA = hrs2rad * equa.X;
        //moonDec = deg2rad * equa.Y;
        // Above NASA datadump appears to list RA/Dec in J2000.0 epoch, we calculated to epoch of date.
        j2k = AEarth::PrecessEquatorial2(equa.lon, equa.lat, astro->getJD_TT(), 2451545.00074287); // latter is JD2000, but in TT);
        //std::cout << astro->getTimeString() << " " << astro->radecFormat(astro->rangezero2tau(moonRA), astro->rangepi2pi(moonDec), true) << "\n";
        //std::cout << astro->getTimeString() << " " << equa.X << " / " << equa.Y << "\n";

        // Selenographic position of the Sun (use for rendering terminator)
        ASelenographicMoonDetails selsun = AMoon::CalculateSelenographicPositionOfSun(astro->getJD_TT(), ELP2000_82);

        // Geocentric libration and position angle of lunar axis
        APhysicalMoonDetails libration = AMoon::CalculateGeocentric(astro->getJD_TT());

        std::cout << "J2000: " << astro->getTimeString() << " " << moonDist << " " << j2k.lon << " " << j2k.lat;
        std::cout << " " << selsun.l0 << " " << selsun.b0 << "\n";
        std::cout << "       " << libration.l << " " << libration.b << " " << ACoord::rangezero2threesixty(libration.P) << "\n";
    }


    // Test new DetailedMoon object
    //astro->setTimeNow();
    //astro->setTime(2020, 1, 27.0, 16.0, 0.0, 0.0);
    astro->setTime(2023, 7, 1.0, 0.0, 0.0, 0.0);
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "LunarView");

    //RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    //text->setFont(app.m_font2);
    //text->setAstronomy(astro);
    //RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    //gui->addLayer3D(layer, "GlobalView");


    // FoV to Moon's greatest angular diameter (0.56833 degrees)
    // Distance: Moon's real diameter is 3474.8km, internal rep is 2 units. Moon's closest distance ~360000km. So 2*360000/3474.8 fills screen
    //           Dist is dynamically updated in DetailedMoon::update() to simulate Earth Moon distance
    cam->setLatLonFovDist(0.0f, 0.0f, 0.56833f, 210.0f);
    // With such as small FoV and high distance, the Near Clipping Plane has to be around 5.0f or more to avoid rendering issues.
    cam->camNear = 5.0f;
    //cam->setCamLightPos()

    DetailedMoon* moon = new DetailedMoon(scene, nullptr, "NSER", 180, 90, 1.0f);
    app.currentEarth2 = moon;
    moon->addSunGP();
    moon->addEarthGP();
    moon->addLibrationTrail();
    //moon->setTopocentric(l_ams.lat, l_ams.lon);
    // NOTE: These can all expose their various confguration settings directly via the object link.
    moon->addEquator();
    moon->equator->setColor(LIGHT_RED);
    moon->equator->setWidth(0.003f);
    moon->addPrimeMeridian();
    moon->primem->setColor(LIGHT_RED);
    moon->primem->setWidth(0.003f);
    //moon->addGrid(10.0);

    //DetailedEarth* erf = scene->newDetailedEarth("NS", 180, 90, 1.0f);
    //erf->addEquator();
    //erf->addPrimeMeridian();
    //app.currentEarth2 = erf;
    //DetailedSky* sky = scene->newDetailedSky("NS", 90, 45, 1.2f);
    //sky->setTexture(true);

    //scene->scenetree->printSceneTree();

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        if (app.anim) {
            astro->addTime(0.0, 0.0, 30.0, 0.0);
            //astro->setTimeNow();
            //app.anim = false; // Nice for single step action. <space> will set app.anim in app.render, and we get back here one frame later.
        }
        app.render();

    }
}


void stellarTest(Application& app) {

    Astronomy* astro = app.newAstronomy();

    astro->setTime(2024, 5, 19.0, 12.0, 0.0, 0.0);
    LLD sirius = astro->ApparentStellarPositionbyName("Sirius");
    astro->explainApparentStellarPositionbyName("Sirius");
    std::cout << ACoord::formatDecRA(sirius) << "\n";

}


void planetaryTest(Application& app) {
    
    Astronomy astro{ };

    //CelestialDetail celdet1 = astro.getDetails(JD_UNIX, MARS, ECGEO);
    CelestialDetail celdet2 = astro.getDetails(JD_UNIX, A_MARS, EPH_VSOP87_SHORT, ECGEO);
    //celdet1.print();
    celdet2.print();

    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    CelestialDetail old;
    double count = 1000; // 100'000;
    double i = 0;

    //old = astro.getDetails(JD_UNIX, MARS, ECGEO);
    //start = std::chrono::high_resolution_clock::now();
    //for (i = 0; i < count; i++) {
    //    old = astro.getDetails(JD_UNIX, MARS, ECGEO);
    //}
    //stop = std::chrono::high_resolution_clock::now();
    //std::cout << "getDetails() Time in us:  " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << '\n';
    //std::cout << "getDetails() RA/Dec: " << rad2hrs * old.geq.lon << ", " << rad2deg * old.geq.lat << "\n";
 
    old = astro.getDetails(JD_UNIX, A_MARS, EPH_VSOP87_SHORT, ECGEO);
    start = std::chrono::high_resolution_clock::now();
    for (i = 0; i < count; i++) {
        old = astro.getDetails(JD_UNIX, A_MARS, EPH_VSOP87_SHORT, ECGEO);
    }
    stop = std::chrono::high_resolution_clock::now();
    std::cout << "getDetails() Time in us:  " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << '\n';
    std::cout << "getDetails() RA/Dec: " << rad2hrs * old.geq.lon << ", " << rad2deg * old.geq.lat << "\n";

    //double equinox = AEarth::NorthwardEquinox(2024, VSOP87_FULL);  // JD_UTC of vernal equinox 2024
    EDateTime eqn{ 2463871.5 };
    std::cout << eqn.string() << "\n";

}


//  #include "astronomy/asun.h"
//  #include "astronomy/aellipsoids.h"

void UnionGlacierSun(Application& app) {
    LLD observerloc = { deg2rad * ACoord::dms2deg(-79.0,46.0,0.0), deg2rad * ACoord::dms2deg(-82.0,52.0,0.0), 0.0 };
    //LLD observerloc = { deg2rad * ACoord::dms2deg(55,43,0.1), deg2rad * ACoord::dms2deg(12,27,0), 0.0 };  // Copenhagen 12d27m00.0sE, 55d43m00.1sN 
    EDateTime utctime{ 2000, 12, 15.0, 0.0, 0.0, 0.0 };
    for (int i = 0; i < 24; i++) {
        std::cout << utctime.string() << "  ";
        // Ecliptic coordinates
        APlanetaryDetails details = AElliptical::getPlanetaryDetails(utctime.jd_tt(), A_SUN, EPH_VSOP87_FULL);
        //details.agecs.lat = ASun::ApparentEclipticLatitude(utctime.jd_tt());
        //details.agecs.lon = ASun::ApparentEclipticLongitude(utctime.jd_tt());
        // std::cout << i << ": ";
        //std::cout << i << ": sunlat=" << Astronomy::angle2DMSstring(details.agecs.lat) << " sunlon=" << Astronomy::angle2DMSstring(details.agecs.lon) << "\n";
        //std::cout << i << ": sunlat=" << (details.agecs.lat) << " sunlon=" << (details.agecs.lon) << "\n";
        double obliquity = AEarth::TrueObliquityOfEcliptic(utctime.jd_tt());
        //std::cout << "obliquity=" << Astronomy::angle2DMSstring(obliquity) << " ";
        //std::cout << "jd_tt=" << utctime.jd_tt() << " jd_utc=" << utctime.jd_utc() << "\t";
        LLD equatorial = Spherical::Ecliptic2Equatorial(details.agecs, obliquity);  // already calculated in details.ageqs
        double agst = AEarth::ApparentGreenwichSiderealTime(utctime.jd_utc());
        //std::cout << "agst=" << Astronomy::angle2uHMSstring(agst) << " ";
        //std::cout << "mlst=" << Astronomy::angle2uHMSstring(AEarth::MeanGreenwichSiderealTime(utctime.jd_utc()) + observerloc.lon) << " ";
        //std::cout << "alst=" << Astronomy::angle2uHMSstring(agst + observerloc.lon) << " ";
        equatorial = AEarth::Equatorial2Topocentric(equatorial, observerloc, agst);
        std::cout << ACoord::angle2uHMSstring(equatorial.lon) << "," << ACoord::angle2DMSstring(equatorial.lat) << "  ";
        std::cout << ACoord::angle2uHMSstring(details.ageqs.lon) << "," << ACoord::angle2DMSstring(details.ageqs.lat) << "  ";
        //std::cout << "Dec/RA=" << Astronomy::formatDecRA(equatorial) << "\n";
        //equatorial.print();
        double lha = ACoord::rangezero2tau(agst - equatorial.lon + observerloc.lon);
        //std::cout << " lha=" << Astronomy::angle2uHMSstring(lha) << " ";
        LLD horizontal = Spherical::Equatorial2Horizontal(lha, equatorial.lat, observerloc.lat);
        //horizontal.print();
        horizontal.lon = ACoord::rangezero2tau(horizontal.lon + pi); // convert southern azimuth to northern
        //std::cout << Astronomy::angle2DMSstring(horizontal.lon) << ", " << Astronomy::angle2DMSstring(horizontal.lat) << "\n";
        std::cout << (rad2deg * horizontal.lon) << ", " << (rad2deg * horizontal.lat) << "\n";
        // !!! FIX: These are within 0.01 degrees (36 arc seconds!!) of JPL ehpemeris, maybe it can be improved a bit
        utctime.addTime(0, 0, 0.0, 1.0, 0.0, 0.0); // add 1 hour
    }
    std::cout << "\n\n earthlon=" << AEarth::EclipticLongitudeJ2000(utctime.jd_utc()) << " ,earthlat=" << AEarth::EclipticLatitudeJ2000(utctime.jd_utc()) << "\n";

    Ellipsoid_2axis my_ellipsoid { 1.0 / 298.25642, 6378.1366, 6378.1366 * (1 - 1.0 / 298.25642), sqrt(2 * (1.0 / 298.25642) - (1.0 / 298.25642) * (1.0 / 298.25642)) };
//	// Something indicating the orientation of planetocentric and planetographic coordinates
//};

    LLD iau76_loc{};
    iau76_loc.lat = deg2rad * 45.0;
    double iau76_lat = AEllipsoid::Planetocentric2Planetographic(iau76_loc, my_ellipsoid).lat;
    iau76_loc.lat = iau76_lat;
    double cen_lat = AEllipsoid::Planetographic2Planetocentric(iau76_loc, my_ellipsoid).lat;
    std::cout << "IAU76: " << rad2deg * iau76_lat << " CEN: " << rad2deg * cen_lat;
}

void ellipsoidDetailedEarth(Application& app) {
    const float radius_gp = 0.03f;
    Astronomy* astro = app.newAstronomy();
    //astro->setTime(2024, 9, 18.0, 2.0, 44.0, 14.1); // https://www.eclipsewise.com/lunar/LEorth/2001-2100/LE2024Sep18P.gif
    //astro->setTime(2024, 9, 18.0, 2.0, 44.0, 14.0);
    //astro->setTime(1992, 4, 12.0, 0.0, 0.0, 0.0); // Given in TT, so convert
    //astro->setJD_TT(astro->getJD_UTC()); // Ex 47.a
    astro->setTime(2024, 12, 12.0, 0.0, 0.0, 0.0);
    astro->setTime(2024, 12, 15.0, 9.0, 50.0, 0.0);
    astro->setTime(2025, 1, 19.0, 21.0, 45.0, 0.0);

    //std::cout << astro->getTimeString() << std::endl;
    Scene* scene = app.newScene();
    scene->astro = astro;
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "EarthView");

    Ellipsoid_2axis my_ellipsoid;
    my_ellipsoid.fromSemimajorEccentricity(6378.140, 0.95);
    //my_ellipsoid.fromSemimajorSemiminor(6378.140, 8500.0);  // test prolate spheroid

    DetailedEarth* erf = new DetailedEarth(scene, nullptr, "NSER", 180, 90, 1.0f);
    erf->setEllipoid(my_ellipsoid);  //  Default is WGS84

    app.currentEarth2 = erf;
    erf->inlunation = false;
    erf->w_twilight = false;
    
    //erf->addMoonTerminator();

    float locdotsize = 0.005f;
    LocationGroup* locgroup = erf->addLocationGroup("LocGroup01");
    LocationSO* ams_loc = locgroup->addLocation({ l_ams * deg2rad }, "Amsterdam");
    LocationSO* cph_loc = locgroup->addLocation({ l_cph * deg2rad }, "Copenhagen");
    LocationSO* ugc_loc = locgroup->addLocation({ l_ugc * deg2rad }, "Union Glacier Camp");
    LocationSO* pua_loc = locgroup->addLocation({ l_pua * deg2rad }, "Puentas Areas");
    for (auto loc : locgroup->locations) {
        loc->addLocDot(locdotsize);
        //loc->addTrueSundir();
        //loc->addZenithArrow();
        //loc->addHorizonPlane({0.0f, 1.0f, 0.0f, 0.5f});
    }
    
    //Earth* erf = scene->newEarth("NSAE", 180, 90);
    //app.currentEarth = erf;
    erf->addEquator();
    erf->addPrimeMeridian();
    erf->addGrid();
    erf->addArcticCircles();
    //erf->removeArcticCircles();
    erf->addTropicCircles();
    erf->addSunGP();
    erf->sungp->setRadius(radius_gp);
    //erf->position = { 0.0f, 0.5f, 0.0f };
    //erf->addGrid(15.0);
    //erf->addCelestialSphere();
    //erf->celestialgrid->setSpacing(24);
    //erf->gridOb->setColor(LIGHT_GREY);
    erf->addEcliptic();
    //erf->addMoonGP();
    //erf->moongp->setRadius(radius_gp);

    DetailedSky* sky = new DetailedSky(scene, nullptr, "NS", 180, 90, 3.0f);
    sky->addGridEcliptic();
    sky->addSun();
    
    //for (double lat{ 0.0 }; lat < pi2; lat += deg2rad * 1.0) {
    //    std::cout << "Lat: " << lat * rad2deg << "   " << AEllipsoid::RadiusOfParallelOfLatitude(lat, my_ellipsoid) / my_ellipsoid.semimajor_axis << "  " << AEllipsoid::RhoCosPhiPrime(lat, 0.0, my_ellipsoid) << "\n";
    //}

    std::cout << AEllipsoid::Earth_IAU76.eccentricity << "\n";

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        if (app.anim) {
            //astro->setTimeNow();
            astro->addTime(0.0, 0.0, 4.0, 0.0);
            //astro->addTime(0.0, 0.0, 0.0, 31558149.504); // Sidereal year in seconds
        }

        //app.anim = false; // Nice for single step action. <space> will set app.anim in app.render, and we get back here one frame later.
        app.render();
    }
}


// -----------
//  BlinkTest
// -----------

std::vector<BlinkTester::Blink_Entry> SDO_List{
    { 2460693.500800231  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_000000_1024_HMIIF.jpg" },
    { 2460693.5112168975 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_001500_1024_HMIIF.jpg" },
    { 2460693.5216335645 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_003000_1024_HMIIF.jpg" },
    { 2460693.532050231  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_004500_1024_HMIIF.jpg" },
    { 2460693.5424668975 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_010000_1024_HMIIF.jpg" },
    { 2460693.5528835645 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_011500_1024_HMIIF.jpg" },
    { 2460693.563300231  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_013000_1024_HMIIF.jpg" },
    { 2460693.5737168975 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_014500_1024_HMIIF.jpg" },
    { 2460693.5841335645 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_020000_1024_HMIIF.jpg" },
    { 2460693.5945502315 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_021500_1024_HMIIF.jpg" },
    { 2460693.604966898  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_023000_1024_HMIIF.jpg" },
    { 2460693.615383565  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_024500_1024_HMIIF.jpg" },
    { 2460693.6258002315 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_030000_1024_HMIIF.jpg" },
    { 2460693.636216898  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_031500_1024_HMIIF.jpg" },
    { 2460693.646633565  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_033000_1024_HMIIF.jpg" },
    { 2460693.6570502315 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_034500_1024_HMIIF.jpg" },
    { 2460693.667466898  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_040000_1024_HMIIF.jpg" },
    { 2460693.677883565  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_041500_1024_HMIIF.jpg" },
    { 2460693.6883002315 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_043000_1024_HMIIF.jpg" },
    { 2460693.698716898  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_044500_1024_HMIIF.jpg" },
    { 2460693.709133565  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_050000_1024_HMIIF.jpg" },
    { 2460693.7195502315 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_051500_1024_HMIIF.jpg" },
    { 2460693.7299668984 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_053000_1024_HMIIF.jpg" },
    { 2460693.7403835654 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_054500_1024_HMIIF.jpg" },
    { 2460693.750800232  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_060000_1024_HMIIF.jpg" },
    { 2460693.7612168984 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_061500_1024_HMIIF.jpg" },
    { 2460693.7716335654 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_063000_1024_HMIIF.jpg" },
    { 2460693.782050232  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_064500_1024_HMIIF.jpg" },
    { 2460693.8549668984 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_083000_1024_HMIIF.jpg" },
    { 2460693.865383566  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_084500_1024_HMIIF.jpg" },
    { 2460693.886216899  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_091500_1024_HMIIF.jpg" },
    { 2460693.896633566  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_093000_1024_HMIIF.jpg" },
    { 2460693.917466899  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_100000_1024_HMIIF.jpg" },
    { 2460693.927883566  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_101500_1024_HMIIF.jpg" },
    { 2460693.9383002324 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_103000_1024_HMIIF.jpg" },
    { 2460693.948716899  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_104500_1024_HMIIF.jpg" },
    { 2460693.959133566  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_110000_1024_HMIIF.jpg" },
    { 2460693.9695502324 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_111500_1024_HMIIF.jpg" },
    { 2460693.979966899  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_113000_1024_HMIIF.jpg" },
    { 2460693.990383566  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_114500_1024_HMIIF.jpg" },
    { 2460694.000800233  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_120000_1024_HMIIF.jpg" },
    { 2460694.0112168994 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_121500_1024_HMIIF.jpg" },
    { 2460694.0216335664 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_123000_1024_HMIIF.jpg" },
    { 2460694.032050233  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_124500_1024_HMIIF.jpg" },
    { 2460694.0424668994 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_130000_1024_HMIIF.jpg" },
    { 2460694.0528835664 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_131500_1024_HMIIF.jpg" },
    { 2460694.063300233  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_133000_1024_HMIIF.jpg" },
    { 2460694.0737168994 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_134500_1024_HMIIF.jpg" },
    { 2460694.0841335664 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_140000_1024_HMIIF.jpg" },
    { 2460694.094550233  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_141500_1024_HMIIF.jpg" },
    { 2460694.1049668994 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_143000_1024_HMIIF.jpg" },
    { 2460694.1153835664 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_144500_1024_HMIIF.jpg" },
    { 2460694.125800233  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_150000_1024_HMIIF.jpg" },
    { 2460694.1362169    , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_151500_1024_HMIIF.jpg" },
    { 2460694.146633567  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_153000_1024_HMIIF.jpg" },
    { 2460694.1570502333 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_154500_1024_HMIIF.jpg" },
    { 2460694.1674669    , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_160000_1024_HMIIF.jpg" },
    { 2460694.177883567  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_161500_1024_HMIIF.jpg" },
    { 2460694.1883002333 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_163000_1024_HMIIF.jpg" },
    { 2460694.1987169    , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_164500_1024_HMIIF.jpg" },
    { 2460694.209133567  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_170000_1024_HMIIF.jpg" },
    { 2460694.2195502333 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_171500_1024_HMIIF.jpg" },
    { 2460694.2299669    , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_173000_1024_HMIIF.jpg" },
    { 2460694.240383567  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_174500_1024_HMIIF.jpg" },
    { 2460694.2508002333 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_180000_1024_HMIIF.jpg" },
    { 2460694.2612169    , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_181500_1024_HMIIF.jpg" },
    { 2460694.2716335673 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_183000_1024_HMIIF.jpg" },
    { 2460694.282050234  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_184500_1024_HMIIF.jpg" },
    { 2460694.2924669003 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_190000_1024_HMIIF.jpg" },
    { 2460694.3028835673 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_191500_1024_HMIIF.jpg" },
    { 2460694.313300234  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_193000_1024_HMIIF.jpg" },
    { 2460694.3237169003 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_194500_1024_HMIIF.jpg" },
    { 2460694.3341335673 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_200000_1024_HMIIF.jpg" },
    { 2460694.344550234  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_201500_1024_HMIIF.jpg" },
    { 2460694.3549669003 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_203000_1024_HMIIF.jpg" },
    { 2460694.3653835673 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_204500_1024_HMIIF.jpg" },
    { 2460694.375800234  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_210000_1024_HMIIF.jpg" },
    { 2460694.3862169003 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_211500_1024_HMIIF.jpg" },
    { 2460694.3966335673 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_213000_1024_HMIIF.jpg" },
    { 2460694.4070502343 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_214500_1024_HMIIF.jpg" },
    { 2460694.417466901  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_220000_1024_HMIIF.jpg" },
    { 2460694.4278835678 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_221500_1024_HMIIF.jpg" },
    { 2460694.4383002343 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_223000_1024_HMIIF.jpg" },
    { 2460694.448716901  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_224500_1024_HMIIF.jpg" },
    { 2460694.4591335678 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_230000_1024_HMIIF.jpg" },
    { 2460694.4695502343 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_231500_1024_HMIIF.jpg" },
    { 2460694.479966901  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_233000_1024_HMIIF.jpg" },
    { 2460694.4903835678 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250118_234500_1024_HMIIF.jpg" },
    { 2460694.5008002343 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_000000_1024_HMIIF.jpg" },
    { 2460694.511216901  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_001500_1024_HMIIF.jpg" },
    { 2460694.5216335678 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_003000_1024_HMIIF.jpg" },
    { 2460694.5320502343 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_004500_1024_HMIIF.jpg" },
    { 2460694.5424669012 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_010000_1024_HMIIF.jpg" },
    { 2460694.552883568  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_011500_1024_HMIIF.jpg" },
    { 2460694.5633002347 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_013000_1024_HMIIF.jpg" },
    { 2460694.5737169012 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_014500_1024_HMIIF.jpg" },
    { 2460694.584133568  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_020000_1024_HMIIF.jpg" },
    { 2460694.5945502347 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_021500_1024_HMIIF.jpg" },
    { 2460694.6049669012 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_023000_1024_HMIIF.jpg" },
    { 2460694.615383568  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_024500_1024_HMIIF.jpg" },
    { 2460694.6258002347 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_030000_1024_HMIIF.jpg" },
    { 2460694.6362169012 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_031500_1024_HMIIF.jpg" },
    { 2460694.646633568  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_033000_1024_HMIIF.jpg" },
    { 2460694.6570502347 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_034500_1024_HMIIF.jpg" },
    { 2460694.6674669012 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_040000_1024_HMIIF.jpg" },
    { 2460694.6778835687 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_041500_1024_HMIIF.jpg" },
    { 2460694.688300235  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_043000_1024_HMIIF.jpg" },
    { 2460694.6987169017 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_044500_1024_HMIIF.jpg" },
    { 2460694.7091335687 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_050000_1024_HMIIF.jpg" },
    { 2460694.719550235  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_051500_1024_HMIIF.jpg" },
    { 2460694.7299669017 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_053000_1024_HMIIF.jpg" },
    { 2460694.7403835687 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_054500_1024_HMIIF.jpg" },
    { 2460694.750800235  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_060000_1024_HMIIF.jpg" },
    { 2460694.7612169017 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_061500_1024_HMIIF.jpg" },
    { 2460694.7716335687 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_063000_1024_HMIIF.jpg" },
    { 2460694.782050235  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_064500_1024_HMIIF.jpg" },
    { 2460694.854966902  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_083000_1024_HMIIF.jpg" },
    { 2460694.865383569  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_084500_1024_HMIIF.jpg" },
    { 2460694.886216902  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_091500_1024_HMIIF.jpg" },
    { 2460694.896633569  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_093000_1024_HMIIF.jpg" },
    { 2460694.917466902  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_100000_1024_HMIIF.jpg" },
    { 2460694.927883569  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_101500_1024_HMIIF.jpg" },
    { 2460694.9383002357 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_103000_1024_HMIIF.jpg" },
    { 2460694.9487169026 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_104500_1024_HMIIF.jpg" },
    { 2460694.9591335696 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_110000_1024_HMIIF.jpg" },
    { 2460694.969550236  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_111500_1024_HMIIF.jpg" },
    { 2460694.9799669026 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_113000_1024_HMIIF.jpg" },
    { 2460694.9903835696 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_114500_1024_HMIIF.jpg" },
    { 2460695.000800236  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_120000_1024_HMIIF.jpg" },
    { 2460695.0112169026 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_121500_1024_HMIIF.jpg" },
    { 2460695.0216335696 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_123000_1024_HMIIF.jpg" },
    { 2460695.032050236  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_124500_1024_HMIIF.jpg" },
    { 2460695.0424669026 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_130000_1024_HMIIF.jpg" },
    { 2460695.0528835696 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_131500_1024_HMIIF.jpg" },
    { 2460695.063300236  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_133000_1024_HMIIF.jpg" },
    { 2460695.0737169026 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_134500_1024_HMIIF.jpg" },
    { 2460695.08413357   , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_140000_1024_HMIIF.jpg" },
    { 2460695.0945502366 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_141500_1024_HMIIF.jpg" },
    { 2460695.104966903  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_143000_1024_HMIIF.jpg" },
    { 2460695.11538357   , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_144500_1024_HMIIF.jpg" },
    { 2460695.1258002366 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_150000_1024_HMIIF.jpg" },
    { 2460695.136216903  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_151500_1024_HMIIF.jpg" },
    { 2460695.14663357   , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_153000_1024_HMIIF.jpg" },
    { 2460695.1570502366 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_154500_1024_HMIIF.jpg" },
    { 2460695.167466903  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_160000_1024_HMIIF.jpg" },
    { 2460695.17788357   , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_161500_1024_HMIIF.jpg" },
    { 2460695.1883002366 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_163000_1024_HMIIF.jpg" },
    { 2460695.198716903  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_164500_1024_HMIIF.jpg" },
    { 2460695.20913357   , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_170000_1024_HMIIF.jpg" },
    { 2460695.219550237  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_171500_1024_HMIIF.jpg" },
    { 2460695.2299669036 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_173000_1024_HMIIF.jpg" },
    { 2460695.2403835705 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_174500_1024_HMIIF.jpg" },
    { 2460695.250800237  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_180000_1024_HMIIF.jpg" },
    { 2460695.2612169036 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_181500_1024_HMIIF.jpg" },
    { 2460695.2716335705 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_183000_1024_HMIIF.jpg" },
    { 2460695.282050237  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_184500_1024_HMIIF.jpg" },
    { 2460695.2924669036 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_190000_1024_HMIIF.jpg" },
    { 2460695.3028835705 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_191500_1024_HMIIF.jpg" },
    { 2460695.313300237  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_193000_1024_HMIIF.jpg" },
    { 2460695.3237169036 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_194500_1024_HMIIF.jpg" },
    { 2460695.3341335705 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_200000_1024_HMIIF.jpg" },
    { 2460695.344550237  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_201500_1024_HMIIF.jpg" },
    { 2460695.354966904  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_203000_1024_HMIIF.jpg" },
    { 2460695.365383571  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_204500_1024_HMIIF.jpg" },
    { 2460695.3758002375 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_210000_1024_HMIIF.jpg" },
    { 2460695.386216904  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_211500_1024_HMIIF.jpg" },
    { 2460695.396633571  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_213000_1024_HMIIF.jpg" },
    { 2460695.4070502375 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_214500_1024_HMIIF.jpg" },
    { 2460695.417466904  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_220000_1024_HMIIF.jpg" },
    { 2460695.427883571  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_221500_1024_HMIIF.jpg" },
    { 2460695.4383002375 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_223000_1024_HMIIF.jpg" },
    { 2460695.448716904  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_224500_1024_HMIIF.jpg" },
    { 2460695.459133571  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_230000_1024_HMIIF.jpg" },
    { 2460695.4695502375 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_231500_1024_HMIIF.jpg" },
    { 2460695.479966904  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_233000_1024_HMIIF.jpg" },
    { 2460695.4903835715 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250119_234500_1024_HMIIF.jpg" },
    { 2460695.500800238  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_000000_1024_HMIIF.jpg" },
    { 2460695.5112169045 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_001500_1024_HMIIF.jpg" },
    { 2460695.5216335715 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_003000_1024_HMIIF.jpg" },
    { 2460695.532050238  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_004500_1024_HMIIF.jpg" },
    { 2460695.5424669045 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_010000_1024_HMIIF.jpg" },
    { 2460695.5528835715 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_011500_1024_HMIIF.jpg" },
    { 2460695.563300238  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_013000_1024_HMIIF.jpg" },
    { 2460695.5737169045 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_014500_1024_HMIIF.jpg" },
    { 2460695.5841335715 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_020000_1024_HMIIF.jpg" },
    { 2460695.594550238  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_021500_1024_HMIIF.jpg" },
    { 2460695.6049669045 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_023000_1024_HMIIF.jpg" },
    { 2460695.6153835715 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_024500_1024_HMIIF.jpg" },
    { 2460695.625800238  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_030000_1024_HMIIF.jpg" },
    { 2460695.6362169045 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_031500_1024_HMIIF.jpg" },
    { 2460695.646633572  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_033000_1024_HMIIF.jpg" },
    { 2460695.6570502385 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_034500_1024_HMIIF.jpg" },
    { 2460695.667466905  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_040000_1024_HMIIF.jpg" },
    { 2460695.677883572  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_041500_1024_HMIIF.jpg" },
    { 2460695.6883002385 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_043000_1024_HMIIF.jpg" },
    { 2460695.698716905  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_044500_1024_HMIIF.jpg" },
    { 2460695.709133572  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_050000_1024_HMIIF.jpg" },
    { 2460695.7195502385 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_051500_1024_HMIIF.jpg" },
    { 2460695.729966905  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_053000_1024_HMIIF.jpg" },
    { 2460695.740383572  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_054500_1024_HMIIF.jpg" },
    { 2460695.7508002385 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_060000_1024_HMIIF.jpg" },
    { 2460695.761216905  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_061500_1024_HMIIF.jpg" },
    { 2460695.771633572  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_063000_1024_HMIIF.jpg" },
    { 2460695.7820502385 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_064500_1024_HMIIF.jpg" },
    { 2460695.8549669054 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_083000_1024_HMIIF.jpg" },
    { 2460695.8653835724 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_084500_1024_HMIIF.jpg" },
    { 2460695.8862169054 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_091500_1024_HMIIF.jpg" },
    { 2460695.8966335724 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_093000_1024_HMIIF.jpg" },
    { 2460695.9174669054 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_100000_1024_HMIIF.jpg" },
    { 2460695.9278835724 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_101500_1024_HMIIF.jpg" },
    { 2460695.938300239  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_103000_1024_HMIIF.jpg" },
    { 2460695.9487169054 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_104500_1024_HMIIF.jpg" },
    { 2460695.9591335724 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_110000_1024_HMIIF.jpg" },
    { 2460695.969550239  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_111500_1024_HMIIF.jpg" },
    { 2460695.9799669054 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_113000_1024_HMIIF.jpg" },
    { 2460695.990383573  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_114500_1024_HMIIF.jpg" },
    { 2460696.0008002394 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_120000_1024_HMIIF.jpg" },
    { 2460696.011216906  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_121500_1024_HMIIF.jpg" },
    { 2460696.021633573  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_123000_1024_HMIIF.jpg" },
    { 2460696.0320502394 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_124500_1024_HMIIF.jpg" },
    { 2460696.042466906  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_130000_1024_HMIIF.jpg" },
    { 2460696.052883573  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_131500_1024_HMIIF.jpg" },
    { 2460696.0633002394 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_133000_1024_HMIIF.jpg" },
    { 2460696.073716906  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_134500_1024_HMIIF.jpg" },
    { 2460696.084133573  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_140000_1024_HMIIF.jpg" },
    { 2460696.0945502394 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_141500_1024_HMIIF.jpg" },
    { 2460696.104966906  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_143000_1024_HMIIF.jpg" },
    { 2460696.115383573  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_144500_1024_HMIIF.jpg" },
    { 2460696.1258002394 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_150000_1024_HMIIF.jpg" },
    { 2460696.136216906  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_151500_1024_HMIIF.jpg" },
    { 2460696.146633573  , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_153000_1024_HMIIF.jpg" },
    { 2460696.15705024   , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_154500_1024_HMIIF.jpg" },
    { 2460696.1674669064 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_160000_1024_HMIIF.jpg" },
    { 2460696.1778835733 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_161500_1024_HMIIF.jpg" },
    { 2460696.18830024   , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_163000_1024_HMIIF.jpg" },
    { 2460696.1987169064 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_164500_1024_HMIIF.jpg" },
    { 2460696.2091335733 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_170000_1024_HMIIF.jpg" },
    { 2460696.21955024   , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_171500_1024_HMIIF.jpg" },
    { 2460696.2299669064 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_173000_1024_HMIIF.jpg" },
    { 2460696.2403835733 , "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\SDO HMI data\\20250120_174500_1024_HMIIF.jpg" }
};

std::vector<BlinkTester::Blink_Entry> Photo_List{
    {NO_DOUBLE, "C:\\Users\\micha\\Desktop\\FEM Challenge 2025-02\\cropped circular3.png"}
};

void blinkTest(Application& app) {
    const float radius_gp = 0.03f;
    Astronomy* astro = app.newAstronomy();
    astro->setTime(2025, 1, 19.0, 21.0, 45.0, 0.0);
    //std::cout << astro->getTimeString() << std::endl;

    Scene* scene = app.newScene();
    scene->astro = astro;
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "EarthView");

    DetailedEarth* erf = new DetailedEarth(scene, nullptr, "NSER", 180, 90, 1.0f);
    app.currentEarth2 = erf;   // Bind Earth to keyboard updates
    erf->inlunation = false;
    erf->w_twilight = false;
    erf->w_mrefract = true;    // Initially show refracted terminator
    //erf->addLatitude();
    Latitude* mylat = new Latitude(scene, erf, erf, 0.0);
    mylat->setWidth(0.001f);

    BlinkTester* blink = new BlinkTester(
        scene,
        nullptr,
        &SDO_List,
        &Photo_List
    );
    blink->blink2->alpha = 0.0f;        // start out just showing the SDO sequence
    blink->blink2->alpha_on = true;

    double pa{ 0 };  // For Position Angle of Sun
    double full_angle{ 0 };  // pa + q
    double lat{ 0 };
    double decl{ 0 };
    double ang_size{ 0 };
    LLD sunpos{ 0.0, 0.0, 0.0 };
    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        // Position angle of Sun
        pa = ASun::CalculatePhysicalSun(astro->getJD_TT(), Planetary_Ephemeris::EPH_VSOP87_FULL).P;
        blink->pos_ang = (float)pa * rad2degf;
        //std::cout << "Sun Position Angle: " << ACoord::angle2DMSstring(pa) << "\n";
        // blink2 is the top image which is being rotated
        full_angle = (double)(blink->blink2->rotation * deg2radf) + pa;
        //std::cout << "Sun Parallactic Angle: " << ACoord::angle2DMSstring(full_angle) << "\n";
        blink->par_ang = (float)full_angle * rad2degf;
        // Calculate Ecliptic Sun position
        sunpos.lat = ASun::GeometricEclipticLatitude(astro->getJD_TT());
        sunpos.lon = ASun::GeometricEclipticLongitude(astro->getJD_TT());
        sunpos += AEarth::EclipticAberration(sunpos.lon, sunpos.lat, astro->getJD_TT(), Planetary_Ephemeris::EPH_VSOP87_FULL);
        sunpos.lon += AEarth::NutationInLongitude(astro->getJD_TT());
        // Convert to Equatorial Sun position
        double obliq = AEarth::TrueObliquityOfEcliptic(astro->getJD_TT());
        sunpos = Spherical::Ecliptic2Equatorial(sunpos, obliq);
        
        // Adjust the latitude band
        lat = asin(cos(full_angle) * cos(sunpos.lat));  // Meeus p99  cos(parallactic angle) = sin(latitude) / cos(declination)
        //std::cout << rad2deg * lat << " (" << ACoord::angle2DMSstring(lat) << ")\n";
        mylat->setLatitude(lat);
        blink->lat_ang = (float)lat * rad2degf;

        ang_size = ASun::SunSemidiameterA(AEarth::EclipticDistance(astro->getJD_TT(), EPH_VSOP87_FULL));
        
        if (app.anim) {  // Spacebar gives output
            std::cout << "\n" << astro->m_datetime.stringms() << " = JD_UTC: " << astro->getJD_UTC() << " = JD_TT: " << astro->getJD_TT() << "\n";
            std::cout << "Position Angle: " << pa * rad2deg << "(" << ACoord::angle2DMSstring(pa) << ")\n";
            std::cout << "Parallactic Angle: " << full_angle * rad2deg << "(" << ACoord::angle2DMSstring(full_angle) << ")\n";
            std::cout << "LATITUDE: " << lat * rad2deg << "(" << ACoord::angle2DMSstring(lat) << ")\n";
            std::cout << "Sun Angular Size: " << ACoord::angle2DMSstring(2.0 * ang_size) << "\n";

            app.anim = false;
        }


        //app.anim = false; // Nice for single step action. <space> will set app.anim in app.render, and we get back here one frame later.
        app.render();
    }
}


// -------------------
//  Test EarthMoonSun
// -------------------
void testEarthMoonSun(Application& app) {
    // Set up
    Astronomy* astro = app.newAstronomy();
    //astro->setTime(2024, 9, 18.0, 2.0, 44.0, 14.1); // https://www.eclipsewise.com/lunar/LEorth/2001-2100/LE2024Sep18P.gif
    astro->setTimeNow();
    Scene* scene = app.newScene();
    scene->astro = astro;
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "EclipseView");

    // Test object
    EarthMoonSun* ems = new EarthMoonSun(scene, nullptr);



    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        if (app.anim) {
            //astro->setTimeNow();
            astro->addTime(0.0, 1.0, 0.0, 0.0);
        }
        //app.anim = false; // Nice for single step action. <space> will set app.anim in app.render, and we get back here one frame later.
        app.render();
    }
}

// -------------------
//  Test EarthSun
// -------------------
void testEarthSun(Application& app) {
    // Set up
    Astronomy* astro = app.newAstronomy();
    astro->setTime(2025, 1, 19.0, 11.0, 43.0, 0.0);  // Time of interest in FEM challenge
    Scene* scene = app.newScene();
    scene->astro = astro;
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Earth Sun Axes");

    // Test object
    EarthSun* es = new EarthSun(scene, nullptr);
    //es->addEarthSunLine();

    // Equatorial Coordinate sphere
    EquatorialCoordinates* eq = new EquatorialCoordinates(scene, es->earth, 24);
    double eq_radius = 0.5;
    eq->scale = { eq_radius, eq_radius, eq_radius };
    eq->setWidthColor(0.002f, LIGHT_GREY);

    EclipticCoordinates* ec = new EclipticCoordinates(scene, es->sun, 24);
    ec->scale = { eq_radius, eq_radius, eq_radius };
    ec->setWidthColor(0.002f, LIGHT_GREY);

    // Line from center of Earth to center of Sun
    SightLine* es_line = new SightLine(scene, es->earth, es->earthpos, es->sunpos, 0.003f, LIGHT_YELLOW);
    // Line from center of Earth to north tip of Sun axis
    SightLine* ee_line = new SightLine(scene, es->earth, es->earthpos, es->sunaxis1, 0.003f, LIGHT_YELLOW);

    LLD p1 = { 0.0, 0.0, 1.0 };
    LLD p2 = { 45.0, 45.0, 1.0 };
    p1 *= deg2rad;
    p2 *= deg2rad;
    LLD ncp = { pi2, 0.0, 1.0 };
    SimpleArc* sunaxis_arc = new SimpleArc(scene, eq, p1, p2);
    SimpleArc* sunpos_arc = new SimpleArc(scene, eq, ncp, p1);

    //scene->scenetree->printSceneTree();

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        if (app.anim) {
            //astro->setTimeNow();
            astro->addTime(0.0, 2.0, 0.0, 0.0);
        }
        //cam->setPosLLH(es->earthloc * rad2deg);
        //app.anim = false; // Nice for single step action. <space> will set app.anim in app.render, and we get back here one frame later.
        
        es_line->setStartEnd(es->earthpos, es->sunpos);
        ee_line->setStartEnd(es->earthpos, es->sunaxis1);
        
        // Arc between sun center and north axis endpoint, projected onto EQ sphere
        // Sun center projection
        glm::dvec3 sunpoint = es->sunpos - es->earthpos;
        sunpoint = glm::normalize(sunpoint) * eq_radius;
        LLD sunpoint_eq = Spherical::Rectangular2Spherical(sunpoint);
        // Sun axis endpoint projection
        glm::dvec3 ee = es->sunaxis1 - es->earthpos;
        ee = glm::normalize(ee) * eq_radius;
        LLD ee_eq = Spherical::Rectangular2Spherical(ee);
        // Adjust arc to be suncenter to sun axis north end
        sunaxis_arc->setStartEnd(sunpoint_eq, ee_eq);

        // Arc between Sun center and North Celestial Pole
        sunpos_arc->setStartEnd(sunpoint_eq, ncp);
        app.render();
    }
}


// -----------------
//  Test Geometry.h
// -----------------
void testGeometry(Application& app) {
    // Set up
    Astronomy* astro = app.newAstronomy();
    //astro->setTime(2024, 9, 18.0, 2.0, 44.0, 14.1); // https://www.eclipsewise.com/lunar/LEorth/2001-2100/LE2024Sep18P.gif
    astro->setTimeNow();
    Scene* scene = app.newScene();
    scene->astro = astro;
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Geometry");

    // Test object
    Ellipse* ellipse = new Ellipse(scene, nullptr);


    
    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        if (app.anim) {
            //astro->setTimeNow();
            astro->addTime(0.0, 1.0, 0.0, 0.0);
        }
        //app.anim = false; // Nice for single step action. <space> will set app.anim in app.render, and we get back here one frame later.
        app.render();
    }
}




// ----------------------
//  Idle Area for python
// ----------------------
void IdleArea(Application& myapp) {
    // An Idle function to drop into after a python script has completed, so the scene can be interacted with for planning the next animation steps.
    // Scans for the various elements created by the python script and takes over by adding GUI / keyboard interaction
    myapp.anim = false;
    RenderLayerGUI* gui = myapp.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    unsigned int lnum = 1;
    char lname[20];
    for (auto& l : myapp.m_layers) {
        if (l->type == LAYER3D) {
            sprintf_s(lname, "Scene %02d", lnum);
            gui->addLayer3D((RenderLayer3D*)l, lname);
            lnum++;
        }
    }
    while (!myapp.shouldClose()) {
        myapp.currentCam->update();
        myapp.update();
        //if (myapp.anim) astro->addTime(0.0, 0.0, 5.0, 0.0);
        //scene->w_camera->CamUpdate();

        myapp.render();
    }
}

// Application is a global container for all the rest
//Application app = Application();  // New global after refactor
Application app{};  // New global after refactor



int main(int argc, char** argv) {
    // FIX: !!! Add cli option parser, so -s can specify a Python script to run !!!
    // 
    // This is main. The entry point of execution when launching
    // Do as little as possible here.
    app.start_fullscreen = false;  // Starting full screen can make debugging difficult during a hang
    if (app.initWindow() == -1) return -1; // Bail if Window and OpenGL context setup fails

    // FIX: !!! This is not very robust, plenty of room for improvement !!!
    std::string pythonscript;
    if (argc > 1) {
        pythonscript = "c:\\Coding\\Eartharium\\Eartharium\\";
        pythonscript += +argv[1];
        // Do path/file validation here
    }
    if (pythonscript.size() > 0) {  // If there is a python file, execute it
        // if (std::filesystem::exists("helloworld.txt"))
        std::cout << "Running Python script: " << pythonscript << '\n';
        PyIface pyiface{};
        pyiface.runscript(pythonscript);
        if (app.interactive) IdleArea(app);
        glfwTerminate();
        return 0; // FIX: !!! Letting the OS deal with all the memory leaks, Not all destructors are up to date currently !!!
    }
    // If no python script was run, simply drop to a test area that sets up via C++ instead.
    // Good for things not yet implemented in python interface, or while developing things,
    // but requires a recompile for every change. Python scripts can simply be saved after changes, and Eartharium can be run again.
    
    // Call scenario
    //TimeGetDetails(app);
    //testPlanetaryDetail(app);
    //testLunarPosition(app);
    //testMoonPhases(app);
    //testLunarNodes(app);
    //testAElliptical(app);
    //testDetailedEarth(app);
    //LunarData(app);
    //planetaryTest(app);
    //stellarTest(app);
    //testDetailedSky(app);
    //UnionGlacierSun(app);
    //testDetailedEarth(app);
    //ellipsoidDetailedEarth(app);
    //testEarthMoonSun(app);
    //testGeometry(app);


    //blinkTest(app);
    testEarthSun(app);


    // Cleanup - Move this to cleanup function in Application.
    glfwTerminate();

    return 0;  // Poor Windows will deal with cleaning up the heap
}


// ----------------
//  GLFW Callbacks
// ----------------
void keyboard_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
    // GLFW has a C interface for callbacks, so this has not yet been moved into Application. I'll have to look into that.
    // Mapped Keys: <ESC>,<SPACE>,a,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,s,u,w,z
    // Unmapped Keys: b,r,v,x,y,1,2,3,4,5,6,7,8,9,0
    if (action == GLFW_RELEASE || action == GLFW_REPEAT) {
        // ESC - End application - may move to end animation when timeline edits have been implemented
        if (key == GLFW_KEY_ESCAPE) { glfwSetWindowShouldClose(app.window, true); }
        // SPACE - Toggle Time Animation
        if (key == GLFW_KEY_SPACE) { app.anim = !app.anim; }
        // G - Toggle GUI
        if (key == GLFW_KEY_G) { app.gui = !app.gui; }
        // F - Toggle Full Screen / Windowed mode
        if (key == GLFW_KEY_F) { app.togglefullwin = true; }
        // P - Print Camera Settings - Replace with a button in Camera GUI !!!
        if (key == GLFW_KEY_P) { app.dumpcam = true; }
        // T - Print Current Time - Replace with a button in Time GUI !!!
        if (key == GLFW_KEY_T) { app.dumptime = true; }
        // B - Dump Data - To be used during troubleshooting to dump data in a given function
        if (key == GLFW_KEY_B) { app.dumpdata = true; }
        // N M - Morph Plus/Minus
        if (key == GLFW_KEY_N && app.currentEarth != nullptr) {
            if (app.currentEarth->param == 0.0f) return;
            app.currentEarth->param -= 0.025f;
            if (app.currentEarth->param < 0.0f) app.currentEarth->param = 0.0f;
        }
        if (key == GLFW_KEY_N && app.currentEarth2) {
            float curmorph = app.currentEarth2->getMorphParameter() - 0.025f;
            app.currentEarth2->setMorphParameter(curmorph > 0.0f ? curmorph : 0.0f);
        }
        if (key == GLFW_KEY_M && app.currentEarth != nullptr) {
            if (app.currentEarth->param == 1.0) return;
            app.currentEarth->param += 0.025f;
            if (app.currentEarth->param > 1.0f) app.currentEarth->param = 1.0f;
        }
        if (key == GLFW_KEY_M && app.currentEarth2) {
            float curmorph = app.currentEarth2->getMorphParameter() + 0.025f;
            app.currentEarth2->setMorphParameter(curmorph < 1.0f ? curmorph : 1.0f);
        }
        // V B - Earth Eccentricity Plus / Minus - May remove or replace !!!
        if (key == GLFW_KEY_V && app.currentEarth != nullptr) {
            if (app.currentEarth->eccen == 0.0) return;
            app.currentEarth->eccen -= 0.01;
            if (app.currentEarth->eccen < 0.0) app.currentEarth->eccen = 0.0;
        }
        if (key == GLFW_KEY_B && app.currentEarth != nullptr) {
            if (app.currentEarth->eccen == 1.0) return;
            app.currentEarth->eccen += 0.01;
            if (app.currentEarth->eccen > 1.0) app.currentEarth->eccen = 1.0;
        }
        // W A S D - Spin Camera around LookAt point
        if (key == GLFW_KEY_A && app.currentCam != nullptr) {
            app.currentCam->camLon -= CAMERA_ANGLE_STEP;
            if (app.currentCam->camLon > 180.0f) app.currentCam->camLon -= 360.0f;
            if (app.currentCam->camLon < -180.0f) app.currentCam->camLon += 360.0f;
        }
        if (key == GLFW_KEY_D && app.currentCam != nullptr) {
            app.currentCam->camLon += CAMERA_ANGLE_STEP;
            if (app.currentCam->camLon > 180.0f) app.currentCam->camLon -= 360.0f;
            if (app.currentCam->camLon < -179.9f) app.currentCam->camLon += 360.0f;
        }
        if (key == GLFW_KEY_W && app.currentCam != nullptr) {
            app.currentCam->camLat += CAMERA_ANGLE_STEP;
            if (app.currentCam->camLat > CAMERA_MAX_LATITUDE) app.currentCam->camLat = CAMERA_MAX_LATITUDE;
        }
        if (key == GLFW_KEY_S && app.currentCam != nullptr) {
            app.currentCam->camLat -= CAMERA_ANGLE_STEP;
            if (app.currentCam->camLat < CAMERA_MIN_LATIDUDE) app.currentCam->camLat = CAMERA_MIN_LATIDUDE;
        }
        // Q E - Increase / Decrease Camera FoV
        if (key == GLFW_KEY_Q && app.currentCam != nullptr) {
            app.currentCam->camFoV += app.currentCam->camFoV * CAMERA_FOV_FACTOR;
            if (app.currentCam->camFoV > CAMERA_MAX_FOV) app.currentCam->camFoV = CAMERA_MAX_FOV;
        }
        if (key == GLFW_KEY_E && app.currentCam != nullptr) {
            app.currentCam->camFoV -= app.currentCam->camFoV * CAMERA_FOV_FACTOR;
            if (app.currentCam->camFoV > CAMERA_MAX_FOV) app.currentCam->camFoV = CAMERA_MAX_FOV;
        }
        // Z C - Increase / Decrease Camera distance to LookAt point
        if (key == GLFW_KEY_C && app.currentCam != nullptr) {
            app.currentCam->camDst -= app.currentCam->camDst * CAMERA_STEP_FACTOR;
            if (app.currentCam->camDst < CAMERA_MIN_DIST) app.currentCam->camDst = CAMERA_MIN_DIST;
        }
        if (key == GLFW_KEY_Z && app.currentCam != nullptr) {
            app.currentCam->camDst += app.currentCam->camDst * CAMERA_STEP_FACTOR;
            if (app.currentCam->camDst > CAMERA_MAX_DIST) app.currentCam->camDst = CAMERA_MAX_DIST;
        }
        // I J K L - Location Camera controls
        if (key == GLFW_KEY_J && app.locationCam != nullptr) {
            app.locationCam->camLon -= CAMERA_ANGLE_STEP;
            if (app.locationCam->camLon > 360.0f) app.locationCam->camLon -= 360.0f;
            if (app.locationCam->camLon < 0.0f) app.locationCam->camLon += 360.0f;
        }
        if (key == GLFW_KEY_L && app.locationCam != nullptr) {
            app.locationCam->camLon += CAMERA_ANGLE_STEP;
            if (app.locationCam->camLon > 360.0f) app.locationCam->camLon -= 360.0f;
            if (app.locationCam->camLon < 0.0f) app.locationCam->camLon += 360.0f;
        }
        if (key == GLFW_KEY_I && app.locationCam != nullptr) {
            app.locationCam->camLat += CAMERA_ANGLE_STEP;
            if (app.locationCam->camLat > CAMERA_MAX_LATITUDE) app.locationCam->camLat = CAMERA_MAX_LATITUDE;
        }
        if (key == GLFW_KEY_K && app.locationCam != nullptr) {
            app.locationCam->camLat -= CAMERA_ANGLE_STEP;
            if (app.locationCam->camLat < CAMERA_MIN_LATIDUDE) app.locationCam->camLat = CAMERA_MIN_LATIDUDE;
        }
        // U O - Location Cam Field of View
        if (key == GLFW_KEY_U && app.locationCam != nullptr) {
            app.locationCam->camFoV += app.locationCam->camFoV * CAMERA_FOV_FACTOR;
            if (app.locationCam->camFoV > CAMERA_MAX_FOV) app.locationCam->camFoV = CAMERA_MAX_FOV;
        }
        if (key == GLFW_KEY_O && app.locationCam != nullptr) {
            app.locationCam->camFoV -= app.locationCam->camFoV * CAMERA_FOV_FACTOR;
            if (app.locationCam->camFoV > CAMERA_MAX_FOV) app.locationCam->camFoV = CAMERA_MAX_FOV;
        }
    };
}
// Called from GLFW when user resizes window. Registered above in setupEnv()
void framebuffer_size_callback(GLFWwindow* window, int mwidth, int mheight) {
    app.SetWH(mwidth, mheight);
}


// ----------------------
//  OpenGL Error handler
// ----------------------
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    // From: https://www.khronos.org/opengl/wiki/Debug_Output#Examples
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}
static void GLClearError() {
    unsigned int i = 0;
    while (glGetError() != GL_NO_ERROR) {
        i++;
    }
    std::cout << "Cleared errors: " << i << std::endl;
}
static void GLPrintError() {
    while (GLenum er = glGetError()) {
        std::cout << "New openGL error: " << er << std::endl;
    }
}
