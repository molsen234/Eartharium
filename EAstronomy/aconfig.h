#pragma once

enum Planet {
	A_MERCURY,
	A_VENUS,
	A_EARTH,
	A_MARS,
	A_JUPITER,
	A_SATURN,
	A_URANUS,
	A_NEPTUNE,
	A_SUN,
	A_EMB,
	A_NONE
};

enum Planetary_Ephemeris {
	// Planetary ephemeris
	//EPH_DEFAULT,
	EPH_VSOP87_SHORT,
	EPH_VSOP87_FULL
};

enum Lunar_Ephemeris {
	MEEUS_SHORT,
	ELP2000_82,
	ELP_MPP02
};

enum Lunar_Phase {
	NEW_MOON,
	FIRST_QUARTER_MOON,
	FULL_MOON,
	THIRD_QUARTER_MOON
};

enum ELPMPP02_Correction {
	ELPMPP02_Nominal,
	ELPMPP02_LLR,
	ELPMPP02_DE405,
	ELPMPP02_DE406
};

enum APrecession_Model {
	// Do a proper survey of precession algorithms, considering valid ranges and accuracy vs performance
	MEEUS,      // MEUS98 eqn 21.2 to 21.4
	IAU2006,    // !!! FIX: ref
	VONDRAK     // https://www.aanda.org/articles/aa/pdf/2011/10/aa17274-11.pdf
};

// !!! FIX: These refraction enums could be named better. Give it a good think.
enum Fwd_Refraction {
	// True -> Apparent
	FWD_REFRACT_NONE,
	FWD_REFRACT_SAEMUNDSSON
};

enum Rev_Refraction {
	// Apparent -> True
	REV_REFRACT_NONE,
	REV_REFRACT_BENNET,
	REV_REFRACT_ALMANAC
};

enum Node {
	ASCENDING,
	DESCENDING
};

enum Hemisphere {
	NORTHERN,
	SOUTHERN
};