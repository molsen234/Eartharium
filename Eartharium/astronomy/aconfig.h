#pragma once

enum Planet {
	A_MERCURY,
	A_VENUS,
	A_EARTH,
	A_MARS,
	A_JUPITER,
	A_SATURN,
	A_NEPTUNE,
	A_URANUS,
	A_SUN,
	A_EMB
};

enum Ephemeris {
	// Planetary ephemeris
	VSOP87_SHORT,
	VSOP87_FULL
};

enum Lunar_Ephemeris {
	MEEUS_SHORT,
	ELP2000_82,
	ELP_MPP02
};

enum ELPMPP02_Correction {
	ELPMPP02_Nominal,
	ELPMPP02_LLR,
	ELPMPP02_DE405,
	ELPMPP02_DE406
};

enum APrecession_Model {
	MEEUS,
	IAU2006
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

enum Hemisphere {
	NORTHERN,
	SOUTHERN
};