/* File: init1.c */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 *
 * UnAngband (c) 2001-6 Andrew Doull. Modifications to the Angband 2.9.1
 * source code are released under the Gnu Public License. See www.fsf.org
 * for current GPL license details. Addition permission granted to
 * incorporate modifications in all Angband variants as defined in the
 * Angband variants FAQ. See rec.games.roguelike.angband for FAQ.
 */

#include "angband.h"


/*
 * This file is used to initialize various variables and arrays for the
 * Angband game.  Note the use of "fd_read()" and "fd_write()" to bypass
 * the common limitation of "read()" and "write()" to only 32767 bytes
 * at a time.
 *
 * Several of the arrays for Angband are built from "template" files in
 * the "lib/file" directory, from which quick-load binary "image" files
 * are constructed whenever they are not present in the "lib/data"
 * directory, or if those files become obsolete, if we are allowed.
 *
 * Warning -- the "ascii" file parsers use a minor hack to collect the
 * name and text information in a single pass.  Thus, the game will not
 * be able to load any template file with more than 20K of names or 60K
 * of text, even though technically, up to 64K should be legal.
 *
 * Note that if "ALLOW_TEMPLATES" is not defined, then a lot of the code
 * in this file is compiled out, and the game will not run unless valid
 * "binary template files" already exist in "lib/data".  Thus, one can
 * compile Angband with ALLOW_TEMPLATES defined, run once to create the
 * "*.raw" files in "lib/data", and then quit, and recompile without
 * defining ALLOW_TEMPLATES, which will both save 20K and prevent people
 * from changing the ascii template files in potentially dangerous ways.
 *
 * The code could actually be removed and placed into a "stand-alone"
 * program, but that feels a little silly, especially considering some
 * of the platforms that we currently support.
 */


#ifdef ALLOW_TEMPLATES


#include "init.h"


/*** Helper arrays for parsing ascii template files ***/

/*
 * Room Special Flags
 */
static cptr d_info_sflags[] =
{
	"SEEN",
	"HEARD",
	"ENTERED",
	"QUEST",
	"BRIDGED",
	"EDGED",
	"LANGUAGE",
	"FLOODED",
	"DAYLITE",
	"ICKY",
	"BLOODY",
	"CURSED",
	"GLOOMY",
	"PORTAL",
	"SILENT",
	"STATIC",
	"STATIS",
	"SEALED",
	"HIDDEN",
	"ANCHOR",
	"ECHOES",
	"STENCH",
	"NOISY",
	"WINDY",
	"GRAVE",
	"STORE",
	"DISPEL",
	"RANDOM",
	"PUZZLE",
	"LAIR",
	"OBJECT",
	"TRAP"
};

/*
 * Room Level Flags
 */
static cptr d_info_pflags[] =
{
	"NORTH",
	"SOUTH",
	"WEST",
	"EAST",
	"CENTRE",
	"EDGE",
	"PLACE",
	"ALLOC",
	"CHECKER",
	"SCATTER",
	"OUTER",
	"RANDOM",
	"HAS_ITEM",
	"HAS_GOLD",
	"TRAIL",
	"INNER",
	"ROWS",
	"COLS",
	"MAZE_PATH",
	"MAZE_WALL",
	"MAZE_DECOR",
	"2X2",
	"3X3",
	"8WAY",
	"DOORWAY",
	"3X3HIDDEN",
	"STARBURST",
	"BRIDGE_EDGE",
	"IGNORE_EDGE",
	"BRIDGE_IN",
	"LITE",
	"DARK"
};

/*
 * Room Level Flags
 */
static cptr d_info_lflags[] =
{
	"WATER",
	"LAVA",
	"ICE",
	"ACID",
	"OIL",
	"CHASM",
	"DESTROYED",
	"LIVING",
	"SEWER",
	"SURFACE",
	"DAYLIGHT",
	"TOWER",
	"BATTLE",
	"VAULT",
	"DUNGEON",
	"STRONGHOLD",
	"CRYPT",
	"LAIR",
	"MINE",
	"CAVE",
	"TOWN",
	"WILD",
	"RUIN",
	"FEATURE",
	"CHAMBERS",
	"DARK",
	"QUEST",
	"GUARDIAN",
	"ROOMS",
	"TUNNELS",
	"LESS",
	"MORE"
};

/*
 * Monster/Trap/Spell Blow Methods
 */
static cptr r_info_blow_method[] =
{
	"",
	"HIT",
	"TOUCH",
	"PUNCH",
	"KICK",
	"CLAW",
	"BITE",
	"STING",
	"VOMIT",
	"BUTT",
	"CRUSH",
	"ENGULF",
	"PECK",
	"CRAWL",
	"DROOL",
	"SLIME",
	"SPIT",
	"GAZE",
	"WAIL",
	"SPORE",
	"LASH",
	"BEG",
	"INSULT",
	"MOAN",
	"SING",
	"TRAP",
	"BOULDER",
	"AURA",
	"SELF",
	"ADJACENT",
	"HANDS",
	"MISSILE",
	"BOLT_10",
	"BOLT",
	"BEAM",
	"BLAST",
	"WALL",
	"BALL",
	"CLOUD",
	"STORM",
	"BREATH",
	"AREA",
	"LOS",
	"LINE",
	"AIM",
	"ORB",
	"STAR",
	"SPHERE",
	"PANEL",
	"LEVEL",
	"CROSS",
	"STRIKE",
	"EXPLODE",
	"ARROW",
	"XBOLT",
	"DAGGER",
	"DART",
	"SHOT",
	"ARC_20",
	"ARC_30",
	"ARC_40",
	"ARC_50",
	"ARC_60",
	"FLASK",
	"TRAIL",
	"SHRIEK",
	"BOLT_MINOR",
	"BALL_MINOR",
	"BALL_II",
	"BALL_III",
	"AURA_MINOR",
	"8WAY",
	"8WAY_II",
	"8WAY_III",
	"SWARM",
	"SPIKE",
	"AIM_AREA",
	"SCATTER",
	"HOWL",
	NULL
};


/*
 * Blow and projection flags
 */
static cptr blow_info_flags1[] =
{
	"BEAM",
	"ARC",
	"STAR",
	"8WAY",
	"4WAY",
	"SCATTER",
	"BOOM",
	"WALL",
	"PASS",
	"MISS",
	"AREA",
	"GRID",
	"ITEM",
	"KILL",
	"PLAY",
	"SELF",
	"LITE",
	"MAGIC",
	"HIDE",
	"NO_REDRAW",
	"SAFE",
	"STOP",
	"JUMP",
	"THRU",
	"CHCK",
	"ORTH",
	"LOS",
	"HOOK",
	"PANEL",
	"LEVEL",
	"FORK",
	"WIDE"
};


/*
 * Blow and projection flags
 */
static cptr blow_info_flags2[] =
{
	"MELEE",
	"RANGED",
	"BREATH",
	"INNATE",
	"CUTS",
	"STUN",
	"SHRIEK",
	"SLIME",
	"TOUCH",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""	
};


/*
 * Monster/Trap/Spell Blow Effects
 */
static cptr r_info_blow_effect[] =
{
	"",
	"STORM",
	"WIND",
	"HELLFIRE",
	"MANA",
	"HOLY_ORB",
	"LITE_WEAK",
	"DARK_WEAK",
	"WATER_WEAK",
	"PLASMA",
	"METEOR",
	"ICE",
	"GRAVITY",
	"INERTIA",
	"FORCE",
	"TIME",
	"ACID",
	"ELEC",
	"FIRE",
	"COLD",
	"POISON",
	"ANIM_DEAD",
	"LITE",
	"DARK",
	"WATER",
	"CONFUSE",
	"SOUND",
	"SHARD",
	"NEXUS",
	"NETHER",
	"CHAOS",
	"DISENCHANT",
	"EXPLODE",
	"KILL_WALL",
	"KILL_DOOR",
	"KILL_TRAP",
	"MAKE_WALL",
	"MAKE_DOOR",
	"MAKE_TRAP",
	"BRIDGE",
	"ANIM_ELEMENT",
	"AWAY_UNDEAD",
	"AWAY_EVIL",
	"AWAY_ALL",
	"TURN_UNDEAD",
	"TURN_EVIL",
	"FEAR_WEAK",
	"DISPEL_UNDEAD",
	"DISPEL_EVIL",
	"DISPEL_ALL",
	"ANIM_OBJECT",
	"CLONE",
	"POLYMORPH",
	"HEAL",
	"HASTE",
	"SLOW_WEAK",
	"CONFUSE_WEAK",
	"SLEEP",
	"DRAIN_LIFE",
	"BWATER",
	"BMUD",
	"HURT",
	"LAVA",
	"UN_BONUS",
	"UN_POWER",
	"EAT_GOLD",
	"EAT_ITEM",
	"EAT_FOOD",
	"EAT_LITE",
	"FALL",
	"FALL_MORE",
	"FALL_SPIKE",
	"FALL_POISON",
	"BLIND",
	"SLOW",
	"TERRIFY",
	"PARALYZE",
	"LOSE_STR",
	"LOSE_INT",
	"LOSE_WIS",
	"LOSE_DEX",
	"LOSE_CON",
	"LOSE_CHR",
	"LOSE_ALL",
	"SHATTER",
	"EXP_10",
	"EXP_20",
	"EXP_40",
	"EXP_80",
	"RAISE",
	"LOWER",
	"PROBE",
	"LOCK_DOOR",
	"SALT_WATER",
	"HALLU",
	"FEATURE",
	"STEAM",
	"VAPOUR",
	"SMOKE",
	"SUFFOCATE",
	"HUNGER",
	"DISEASE",
	"LOSE_MANA",
	"WOUND",
	"BATTER",
	"BLIND_WEAK",
	"RAISE_DEAD",
	"GAIN_MANA",
	"FORGET",
	"CURSE",
	"DISPEL",
	"STASTIS",
	"PETRIFY",
	"WEB",
	"BLOOD",
	"SLIME",
	"RESIST_MAGIC",
	"FALL_LESS",
	"AWAY_DARK",
	"AWAY_JUMP",
	"HURT_WOOD",
	"AWAY_NATURE",
	"AWAY_FIRE",
	"ANIM_TREE",
	"CHARM_INSECT",
	"CHARM_ANIMAL",
	"CHARM_REPTILE",
	"CHARM_MONSTER",
	"CHARM_PERSON",
	"BIND_DEMON",
	"BIND_UNDEAD",
	"BIND_DRAGON",
	"BIND_FAMILIAR",
	"VAMP_DRAIN",
	"MANA_DRAIN",
	"MENTAL",
	"SNUFF",
	"RAGE",
	"HEAL_PERC",
	"GAIN_MANA_PERC",
	"TANGLE",
	"POISON_WATER",
	"INVISIBILITY",
	NULL
};

/*
 * Feature info flags
 */
static cptr f_info_flags1[] =
{
	"LOS",
	"PROJECT",
	"MOVE",
	"PLACE",
	"DROP",
	"SECRET",
	"NOTICE",
	"REMEMBER",
	"OPEN",
	"CLOSE",
	"BASH",
	"SPIKE",
	"DISARM",
	"ENTER",
	"TUNNEL",
	"STREAMER",
	"HAS_GOLD",
	"HAS_ITEM",
	"DOOR",
	"TRAP",
	"STAIRS",
	"GLYPH",
	"LESS",
	"MORE",
	"RUN",
	"FLOOR",
	"WALL",
	"PERMANENT",
	"INNER",
	"OUTER",
	"SOLID",
	"HIT_TRAP"
};

/*
 * Feature info flags
 */
static cptr f_info_flags2[] =
{
	"BRIDGE",
	"RIVER",
	"LAKE",
	"BRIDGED",
	"COVERED",
	"GLOW",
	"LAVA",
	"WATER",
	"FLAVOR",
	"SHALLOW",
	"DEEP",
	"FILLED",
	"HURT_ROCK",
	"HURT_FIRE",
	"HURT_COLD",
	"HURT_ACID",
	"ICE",
	"ACID",
	"OIL",
	"CHASM",
	"CAN_CLIMB",
	"CAN_FLY",
	"CAN_SWIM",
	"CAN_PASS",
	"CAN_OOZE",
	"CAN_DIG",
	"HIDE_ITEM",
	"HIDE_SNEAK",
	"HIDE_SWIM",
	"HIDE_DIG",
	"KILL_HUGE",
	"KILL_MOVE"
};

/*
 * Feature info flags
 */
static cptr f_info_flags3[] =
{
	"PICK_TRAP",
	"PICK_DOOR",
	"ALLOC",
	"CHEST",
	"DROP_1D2",
	"DROP_1D3",
	"DROP_GOOD",
	"DROP_GREAT",
	"HURT_POIS",
	"HURT_ELEC",
	"HURT_WATER",
	"HURT_BWATER",
	"USE_FEAT",
	"GET_FEAT",
	"GROUND",
	"OUTSIDE",
	"EASY_HIDE",
	"EASY_CLIMB",
	"MUST_CLIMB",
	"LIVING",
	"TREE",
	"NEED_TREE",
	"ATTR_LITE",
	"ATTR_ITEM",
	"ATTR_DOOR",
	"ATTR_WALL",
	"INSTANT",
	"ADJACENT",
	"TIMED",
	"ERUPT",
	"STRIKE",
	"SPREAD"
};

/*
 * Monster race flags
 */
static cptr r_info_flags1[] =
{
	"UNIQUE",
	"QUESTOR",
	"MALE",
	"FEMALE",
	"CHAR_CLEAR",
	"CHAR_MULTI",
	"ATTR_CLEAR",
	"ATTR_MULTI",
	"FORCE_DEPTH",
	"FORCE_MAXHP",
	"FORCE_SLEEP",
	"GUARDIAN",
	"FRIEND",
	"FRIENDS",
	"ESCORT",
	"ESCORTS",
	"NEVER_BLOW",
	"NEVER_MOVE",
	"RAND_25",
	"RAND_50",
	"ONLY_GOLD",
	"ONLY_ITEM",
	"DROP_30",
	"DROP_60",
	"DROP_90",
	"DROP_1D2",
	"DROP_1D3",
	"DROP_1D4",
	"DROP_GOOD",
	"DROP_GREAT",
	"DROP_USEFUL",
	"DROP_CHOSEN"
};
/*
 * Monster race flags
 */
static cptr r_info_flags2[] =
{
	"STUPID",
	"SMART",
	"CAN_DIG",
	"HAS_LITE",
	"INVISIBLE",
	"COLD_BLOOD",
	"EMPTY_MIND",
	"WEIRD_MIND",
	"MULTIPLY",
	"REGENERATE",
	"CAN_SWIM",
	"MUST_SWIM",
	"POWERFUL",
	"CAN_CLIMB",
	"CAN_FLY",
	"MUST_FLY",
	"OPEN_DOOR",
	"BASH_DOOR",
	"PASS_WALL",
	"KILL_WALL",
	"ARCHER",
	"EAT_BODY",
	"TAKE_ITEM",
	"TRAIL",
	"SNEAKY",
	"WARRIOR",
	"PRIEST",
	"MAGE",
	"HAS_AURA",
	"HAS_WEB",
	"NEED_LITE",
	"LOW_MANA_RUN"
};

/*
 * Monster race flags
 */
static cptr r_info_flags3[] =
{
	"ORC",
	"TROLL",
	"GIANT",
	"DRAGON",
	"DEMON",
	"UNDEAD",
	"EVIL",
	"ANIMAL",
	"OOZE",
	"HUGE",
	"NONVOCAL",
	"NONLIVING",
	"HURT_LITE",
	"HURT_ROCK",
	"PLANT",
	"INSECT",
	"IM_ACID",
	"IM_ELEC",
	"IM_FIRE",
	"IM_COLD",
	"IM_POIS",
	"RES_WATER",
	"RES_NETHR",
	"RES_LAVA",
	"RES_PLAS",
	"RES_NEXUS",
	"RES_DISEN",
	"HURT_WATER",
	"NO_FEAR",
	"NO_STUN",
	"NO_CONF",
	"NO_SLEEP"
};

/*
 * Monster race flags
 */
static cptr r_info_flags4[] =
{
	"BLOW_1",
	"BLOW_2",
	"BLOW_3",
	"BLOW_4",
	"ADD_AMMO",
	"QUAKE",
	"EXPLODE",
	"AURA",
	"BRTH_ACID",
	"BRTH_ELEC",
	"BRTH_FIRE",
	"BRTH_COLD",
	"BRTH_POIS",
	"BRTH_PLAS",
	"BRTH_LITE",
	"BRTH_DARK",
	"BRTH_CONFU",
	"BRTH_SOUND",
	"BRTH_SHARD",
	"BRTH_INERT",
	"BRTH_GRAV",
	"BRTH_WIND",
	"BRTH_FORCE",
	"BRTH_NEXUS",
	"BRTH_NETHR",
	"BRTH_CHAOS",
	"BRTH_DISEN",
	"BRTH_TIME",
	"BRTH_MANA",
	"BRTH_HOLY",
	"BRTH_FEAR",
	"BRTH_DISEA"
};


/*
 * Monster race flags
 */
static cptr r_info_flags5[] =
{
	"BALL_ACID",
	"BALL_ELEC",
	"BALL_FIRE",
	"BALL_COLD",
	"BALL_POIS",
	"BALL_LITE",
	"BALL_DARK",
	"BALL_CONFU",
	"BALL_SOUND",
	"BALL_SHARD",
	"BALL_WIND",
	"BALL_STORM",
	"BALL_NETHR",
	"BALL_CHAOS",
	"BALL_MANA",
	"BALL_WATER",
	"BOLT_ACID",
	"BOLT_ELEC",
	"BOLT_FIRE",
	"BOLT_COLD",
	"BOLT_POIS",
	"BOLT_PLAS",
	"BOLT_ICE",
	"BOLT_WATER",
	"BOLT_NETHR",
	"BOLT_MANA",
	"HOLY_ORB",
	"BEAM_ELEC",
	"BEAM_ICE",
	"BEAM_NETHR",
	"ARC_HFIRE",
	"ARC_FORCE"
};

/*
 * Monster race flags
 */
static cptr r_info_flags6[] =
{
	"HASTE",
	"ADD_MANA",
	"HEAL",
	"CURE",
	"BLINK",
	"TPORT",
	"INVIS",
	"TELE_SELF_TO",
	"TELE_TO",
	"TELE_AWAY",
	"TELE_LEVEL",
	"WRAITHFORM",
	"DARKNESS",
	"TRAPS",
	"FORGET",
	"DRAIN_MANA",
	"CURSE",
	"DISPEL",
	"MIND_BLAST",
	"ILLUSION",
	"WOUND",
	"BLESS",
	"BERSERK",
	"SHIELD",
	"OPPOSE_ELEM",
	"HUNGER",
	"PROBE",
	"SCARE",
	"BLIND",
	"CONF",
	"SLOW",
	"HOLD"
};

/*
 * Monster race flags
 */
static cptr r_info_flags7[] =
{
	"S_KIN",
	"R_KIN",
	"A_DEAD",
	"S_MONSTER",
	"S_MONSTERS",
	"R_MONSTER",
	"R_MONSTERS",
	"S_PLANT",
	"S_INSECT",
	"S_ANIMAL",
	"S_HOUND",
	"S_SPIDER",
	"S_CLASS",
	"S_RACE",
	"S_GROUP",
	"S_FRIEND",
	"S_FRIENDS",
	"S_ORC",
	"S_TROLL",
	"S_GIANT",
	"S_DRAGON",
	"S_HI_DRAGON",
	"A_ELEMENT",
	"A_OBJECT",
	"S_DEMON",
	"S_HI_DEMON",
	"R_UNIQUE",
	"S_UNIQUE",
	"S_HI_UNIQUE",
	"S_UNDEAD",
	"S_HI_UNDEAD",
	"S_WRAITH"
};

/*
 * Monster race flags
 */
static cptr r_info_flags8[] =
{
	"HAS_SKULL",
	"HAS_SKELETON",
	"HAS_TEETH",
	"HAS_CORPSE",
	"HAS_HEAD",
	"HAS_HAND",
	"HAS_CLAW",
	"HAS_ARM",
	"HAS_LEG",
	"HAS_WING",
	"HAS_SKIN",
	"HAS_SCALE",
	"HAS_FEATHER",
	"HAS_FUR",
	"HAS_BLOOD",
	"HAS_SLIME",
	"HAS_SPORE",
	"DROP_CHEST",
	"DROP_MISSILE",
	"DROP_TOOL",
	"DROP_WEAPON",
	"DROP_MUSIC",
	"DROP_CLOTHES",
	"DROP_ARMOR",
	"DROP_LITE",
	"DROP_JEWELRY",
	"DROP_RSW",
	"DROP_WRITING",
	"DROP_POTION",
	"DROP_FOOD",
	"DROP_JUNK",
	"ASSEMBLE"
};


/*
 * Monster race flags
 */
static cptr r_info_flags9[] =
{
	"LEVEL_CLASS",
	"NEVER_MISS",
	"LEVEL_SPEED",
	"EVASIVE",
	"SCENT",
	"SUPER_SCENT",
	"WATER_SCENT",
	"RES_BLIND",
	"RES_LITE",
	"RES_DARK",
	"RES_CHAOS",
	"RES_TPORT",
	"RES_EDGED",
	"RES_BLUNT",
	"IM_EDGED",
	"IM_BLUNT",
	"NO_CUTS",
	"NO_SLOW",
	"RES_MAGIC",
	"GOOD",
	"NEUTRAL",
	"DWARF",
	"ELF",
	"MAN",
	"TOWNSFOLK",
	"DROP_ESSENCE",
	"DROP_MUSHROOM",
	"DROP_MINERAL",
	"ATTR_METAL",
	"ATTR_INDEX",
	"LEVEL_SIZE",
	"LEVEL_POWER"
};

#if 0
/*
 * Flow flags
 */
static cptr flow_flags[] =
{
	"FLOW_WALK_WALL",
	"FLOW_WALK_OOZE",
	"FLOW_WALK_BASH_OPEN",
	"FLOW_WALK_BASH",
	"FLOW_WALK_OPEN",
	"FLOW_WALK",
	"FLOW_WALK_FLY",
	"FLOW_WALK_CLIMB",
	"FLOW_FLY",
	"FLOW_WALK_SWIM",
	"FLOW_SWIM",
	"FLOW_WALK_DIG",
	"FLOW_WALK_ACID",
	"FLOW_WALK_COLD",
	"FLOW_WALK_ELEC",
	"FLOW_WALK_FIRE",
	"FLOW_WALK_FIRE_DIG",
	"FLOW_WALK_FIRE_SWIM",
	"FLOW_WALK_NONLIVING",
	"FLOW_ACID",
	"FLOW_COLD",
	"FLOW_DIG",
	"FLOW_FIRE",
	"FLOW_FIRE_DIG",
	"FLOW_FIRE_SWIM",
	"FLOW_FLY_CHASM",
	"FLOW_SWIM_DEEP",
	"FLOW_HURT_FIRE",
	"FLOW_HURT_WATER",
	"",
	"",
	""
};
#endif


/*
 * Object flags
 */
static cptr k_info_flags1[] =
{
	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",
	"CHR",
	"SAVE",
	"DEVICE",
	"STEALTH",
	"SEARCH",
	"INFRA",
	"TUNNEL",
	"SPEED",
	"BLOWS",
	"SHOTS",
	"MIGHT",
	"SLAY_NATURAL",
	"BRAND_HOLY",
	"SLAY_UNDEAD",
	"SLAY_DEMON",
	"SLAY_ORC",
	"SLAY_TROLL",
	"SLAY_GIANT",
	"SLAY_DRAGON",
	"KILL_DRAGON",
	"KILL_DEMON",
	"KILL_UNDEAD",
	"BRAND_POIS",
	"BRAND_ACID",
	"BRAND_ELEC",
	"BRAND_FIRE",
	"BRAND_COLD"
};

/*
 * Object flags
 */
static cptr k_info_flags2[] =
{
	"SUST_STR",
	"SUST_INT",
	"SUST_WIS",
	"SUST_DEX",
	"SUST_CON",
	"SUST_CHR",
	"IGNORE_ACID",
	"IGNORE_ELEC",
	"IGNORE_FIRE",
	"IGNORE_COLD",
	"IGNORE_WATER",
	"IGNORE_THEFT",
	"IM_ACID",
	"IM_ELEC",
	"IM_FIRE",
	"IM_COLD",
	"RES_ACID",
	"RES_ELEC",
	"RES_FIRE",
	"RES_COLD",
	"RES_POIS",
	"RES_FEAR",
	"RES_LITE",
	"RES_DARK",
	"RES_BLIND",
	"RES_CONFU",
	"RES_SOUND",
	"RES_SHARD",
	"RES_NEXUS",
	"RES_NETHR",
	"RES_CHAOS",
	"RES_DISEN"
};

/*
 * Object flags
 */
static cptr k_info_flags3[] =
{
	"SLOW_DIGEST",
	"FEATHER",
	"LITE",
	"REGEN_HP",
	"TELEPATHY",
	"SEE_INVIS",
	"FREE_ACT",
	"HOLD_LIFE",
	"ESP_DEMON",
	"ESP_DRAGON",
	"ESP_GIANT",
	"ESP_ORC",
	"ESP_TROLL",
	"ESP_UNDEAD",
	"ESP_NATURE",
	"REGEN_MANA",
	"DRAIN_HP",
	"DRAIN_MANA",
	"DRAIN_EXP",
	"AGGRAVATE",
	"UNCONTROLLED",
	"RANDOM",
	"ACTIVATE",
	"BLESSED",
	"INSTA_ART",
	"HUNGER",
	"IMPACT",
	"HAS_ROPE",
	"THROWING",
	"LIGHT_CURSE",
	"HEAVY_CURSE",
	"PERMA_CURSE"
};

/*
 * Object flags
 */
static cptr k_info_flags4[] =
{
	"BRAND_DARK",
	"BRAND_LITE",
	"HURT_LITE",
	"HURT_WATER",
	"VAMP_HP",
	"VAMP_MANA",
	"IM_POIS",
	"RES_DISEASE",
	"RES_WATER",
	"SLAY_MAN",
	"SLAY_ELF",
	"SLAY_DWARF",
	"ANCHOR",
	"SILENT",
	"STATIC",
	"WINDY",
	"ANIMAL",
	"EVIL",
	"UNDEAD",
	"DEMON",
	"ORC",
	"TROLL",
	"GIANT",
	"DRAGON",
	"MAN",
	"DWARF",
	"ELF",
	"HURT_POIS",
	"HURT_ACID",
	"HURT_ELEC",
	"HURT_FIRE",
	"HURT_COLD"
};


/*
 * Weapon style type
 */
static cptr w_info_style[] =
{
	"NONE",
	"UNARMED",
	"ONE-HANDED",
	"TWO-HANDED",
	"WEAPON&SHIELD",
	"TWO-WEAPON",
	"HAFTED",
	"SWORD",
	"POLEARM",
	"THROWN",
	"SLING",
	"BOW",
	"XBOW",
	"BACKSTAB",
	"MAGIC_BOOK",
	"PRAYER_BOOK",
	"SONG_BOOK",
	"INSTRUMENT",
	"POTION",
	"SCROLL",
	"AMULET",
	"RING",
	"WAND",
	"STAFF",
	"SLAY_ORC",
	"SLAY_TROLL",
	"SLAY_GIANT",
	"SLAY_DRAGON",
	"SLAY_EVIL",
	"SLAY_UNDEAD",
	"SLAY_ANIMAL",
	"SLAY_DEMON"
};


/*
 * Weapon style benefits
 */
static cptr w_info_benefit[] =
{
	"NONE",
	"HIT",
	"DAM",
	"AC",
	"XTRA_BLOW",
	"XTRA_SHOT",
	"XTRA_MIGHT",
	"CRITICAL",
	"ID",
	"POWER",
	"ICKY_HANDS",
	"ICKY_WIELD",
	"BLESSED",
	"HOLD_SONG",
	"RES_FEAR"
};


/*
 * Spell effect flags
 */
static cptr s_info_flags1[] =
{
	"DETECT_DOORS",
	"DETECT_TRAPS",
	"DETECT_STAIRS",
	"DETECT_WATER",
	"DETECT_GOLD",
	"DETECT_OBJECT",
	"DETECT_MAGIC",
	"DETECT_CURSE",
	"DETECT_POWER",
	"DETECT_MONSTER",
	"DETECT_EVIL",
	"DETECT_LIFE",
	"MAP_AREA",
	"WIZ_LITE",
	"LITE_ROOM",
	"DARK_ROOM",
	"FORGET",
	"SELF_KNOW",
	"IDENT",
	"IDENT_MAGIC",
	"IDENT_SENSE",
	"IDENT_BONUS",
	"IDENT_RUNES",
	"IDENT_VALUE",
	"IDENT_RUMOR",
	"IDENT_FULLY",
	"ACQUIREMENT",
	"STAR_ACQUIREMENT",
	"ENCHANT_TOH",
	"ENCHANT_TOD",
	"ENCHANT_TOA",
	"ENCHANT_HIGH"
};

/*
 * Spell effect flags
 */
static cptr s_info_flags2[] =
{
	"AGGRAVATE",
	"CURSE_WEAPON",
	"CURSE_ARMOR",
	"CREATE_STAIR",
	"TELE_LEVEL",
	"ALTER_LEVEL",
	"BANISHMENT",
	"MASS_BANISHMENT",
	"CUT",
	"STUN",
	"POISON",
	"PARALYZE",
	"HALLUC",
	"SLOW",
	"BLIND",
	"CONFUSE",
	"FEAR",
	"INFRA",
	"HASTE",
	"HERO",
	"SHERO",
	"BLESS",
	"SHIELD",
	"INVIS",
	"SEE_INVIS",
	"PROT_EVIL",
	"RECALL",
	"OPP_FIRE",
	"OPP_COLD",
	"OPP_ACID",
	"OPP_ELEC",
	"OPP_POIS"
};

/*
 * Spell effect flags
 */
static cptr s_info_flags3[] =
{
	"INC_STR",
	"INC_INT",
	"INC_WIS",
	"INC_DEX",
	"INC_CON",
	"INC_CHR",
	"CURE_STR",
	"CURE_INT",
	"CURE_WIS",
	"CURE_DEX",
	"CURE_CON",
	"CURE_CHR",
	"INC_EXP",
	"CURE_EXP",
	"FREE_ACT",
	"CURE_MEM",
	"SLOW_CURSE",
	"CURE_CURSE",
	"SLOW_CUTS",
	"CURE_CUTS",
	"SLOW_STUN",
	"CURE_STUN",
	"CURE_POIS",
	"CURE_CONF",
	"CURE_FOOD",
	"CURE_FEAR",
	"CURE_BLIND",
	"CURE_IMAGE",
	"DEC_FOOD",
	"DEC_EXP",
	"HOLD_SONG",
	"THAUMATURGY"
};

/*
 * Spell parametised effects
 */
static cptr s_info_types[] =
{
	"",
	"RECHARGE",
	"IDENT_TVAL",
	"ENCHANT_TVAL",
	"BRAND_WEAPON",
	"BRAND_ARMOR",
	"BRAND_ITEM",
	"BRAND_AMMO",
	"WARD_GLYPH",
	"WARD_TRAP",
	"SUMMON",
	"SUMMON_RACE",
	"SUMMON_GROUP_IDX",
	"CREATE_KIND",
	"EARTHQUAKE",
	"DESTRUCTION",
	"CURE_DISEASE",
	"SLOW_CONF",
	"SLOW_POIS",
	"IDENT_PACK",
	"CHANGE_SHAPE",
	"REVERT_SHAPE",
	"DETECT_MIND",
	"REFUEL",
	"INVEN_WIELD",
	"INVEN_BOW",
	"INVEN_LEFT",
	"INVEN_RIGHT",
	"INVEN_NECK",
	"INVEN_LITE",
	"INVEN_BODY",
	"INVEN_OUTER",
	"INVEN_ARM",
	"INVEN_HEAD",
	"INVEN_HANDS",
	"INVEN_FEET",
	"USE_OBJECT",
	"IDENT_NAME",
	"RELEASE_CURSE",
	"CONCENTRATE_LITE",
	"CONCENTRATE_LIFE",
	"CONCENTRATE_WATER",
	"SET_RETURN",
	"SET_OR_MAKE_RETURN",
	"BLOOD_BOND",
	"MINDS_EYE",
	"LIGHT_CHAMBERS",
	"REST_UNTIL_DUSK",
	"REST_UNTIL_DAWN",
	"MAGIC_BLOW",
	"MAGIC_SHOT",
	"MAGIC_HURL",
	"ACCURATE_BLOW",
	"ACCURATE_SHOT",
	"ACCURATE_HURL",
	"DAMAGING_BLOW",
	"DAMAGING_SHOT",
	"DAMAGING_HURL",
	NULL
};



/*
 * Quest event flags
 */
static cptr quest_event_info_flags[] =
{
	"TRAVEL",
	"LEAVE",
	"STAY",
	"PASS_QUEST",
	"FAIL_QUEST",
	"FIND_ROOM",
	"FLAG_ROOM",
	"UNFLAG_ROOM",
	"ALTER_FEAT",
	"DEFEND_FEAT",
	"FIND_ITEM",
	"GET_ITEM",
	"DESTROY_ITEM",
	"LOSE_ITEM",
	"TALK_STORE",
	"BUY_STORE",
	"SELL_STORE",
	"GIVE_STORE",
	"STOCK_STORE",
	"GET_STORE",
	"DEFEND_STORE",
	"TALK_RACE",
	"GIVE_RACE",
	"GET_RACE",
	"FIND_RACE",
	"KILL_RACE",
	"ALLY_RACE",
	"HATE_RACE",
	"FEAR_RACE",
	"HEAL_RACE",
	"BANISH_RACE",
	"DEFEND_RACE"
};



/*** Initialize from ascii template files ***/


/*
 * Initialize an "*_info" array, by parsing an ascii "template" file
 */
errr init_info_txt(FILE *fp, char *buf, header *head,
   parse_info_txt_func parse_info_txt_line)
{
	errr err;

	/* Not ready yet */
	bool okay = FALSE;

	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = 0;


	/* Prepare the "fake" stuff */
	head->name_size = 0;
	head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf+2, "%d.%d.%d", &v1, &v2, &v3)) ||
				(v1 != head->v_major) ||
				(v2 != head->v_minor) ||
				(v3 != head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);

		/* Parse the line */
		if ((err = (*parse_info_txt_line)(buf, head)) != 0)
			return (err);
	}


	/* Complete the "name" and "text" sizes */
	if (head->name_size) head->name_size++;
	if (head->text_size) head->text_size++;


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}


/*
 * Add a text to the text-storage and store offset to it.
 *
 * Returns FALSE when there isn't enough space available to store
 * the text.
 */
static bool add_text(u32b *offset, header *head, cptr buf)
{
	/* Hack -- Verify space */
	if (head->text_size + strlen(buf) + 8 > z_info->fake_text_size)
		return (FALSE);

	/* New text? */
	if (*offset == 0)
	{
		/* Advance and save the text index */
		*offset = ++head->text_size;	
	}

	/* Append chars to the text */
	strcpy(head->text_ptr + head->text_size, buf);

	/* Advance the index */
	head->text_size += strlen(buf);

	/* Success */
	return (TRUE);
}


/*
 * Add a name to the name-storage and return an offset to it.
 *
 * Returns 0 when there isn't enough space available to store
 * the name.
 */
static u32b add_name(header *head, cptr buf)
{
	u32b index;

	/* Hack -- Verify space */
	if (head->name_size + strlen(buf) + 8 > z_info->fake_name_size)
		return (0);

	/* Advance and save the name index */
	index = ++head->name_size;

	/* Append chars to the names */
	strcpy(head->name_ptr + head->name_size, buf);

	/* Advance the index */
	head->name_size += strlen(buf);
	
	/* Return the name index */
	return (index);
}


/*
 * Initialize the "z_info" structure, by parsing an ascii "template" file
 */
errr parse_z_info(char *buf, header *head)
{
	maxima *z_info = head->info_ptr;

	/* Hack - Verify 'M:x:' format */
	if (buf[0] != 'M') return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	if (!buf[2]) return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	if (buf[3] != ':') return (PARSE_ERROR_UNDEFINED_DIRECTIVE);


	/* Process 'D' for "Maximum d_info[] index" */
	if (buf[2] == 'D')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->d_max = max;
	}
	/* Process 'F' for "Maximum f_info[] index" */
	else if (buf[2] == 'F')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->f_max = max;
	}

	/* Process 'K' for "Maximum k_info[] index" */
	else if (buf[2] == 'K')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->k_max = max;
	}

	/* Process 'A' for "Maximum number of standard artifacts " */
	else if (buf[2] == 'A')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->a_max = max;
	}

	/* Process 'a' for "Maximum a_info[] index" */
	else if (buf[2] == 'a')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->a_max_standard = max;
	}

	/* Process 'E' for "Maximum e_info[] index" */
	else if (buf[2] == 'E')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->e_max = max;
	}

	/* Process 'X' for "Maximum x_info[] index" */
	else if (buf[2] == 'X')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->x_max = max;
	}

	/* Process 'R' for "Maximum r_info[] index" */
	else if (buf[2] == 'R')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->r_max = max;
	}


	/* Process 'V' for "Maximum v_info[] index" */
	else if (buf[2] == 'V')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->v_max = max;
	}


	/* Process 'P' for "Maximum p_info[] index" */
	else if (buf[2] == 'P')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->p_max = max;
	}

	/* Process 'G' for "Maximum g_info[] index" */
	else if (buf[2] == 'G')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->g_max = max;
	}

	/* Process 'C' for "Maximum c_info[] index" */
	else if (buf[2] == 'C')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->c_max = max;
	}

	/* Process 'W' for "Maximum w_info[] index" */
	else if (buf[2] == 'W')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->w_max = max;
	}

	/* Process 'S' for "Maximum s_info[] index" */
	else if (buf[2] == 'S')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->s_max = max;
	}

	/* Process 'Y' for "Maximum y_info[] index" */
	else if (buf[2] == 'Y')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->y_max = max;
	}
	/* Process 'T' for "Maximum t_info[] index" */
	else if (buf[2] == 'T')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->t_max = max;
	}

	/* Process 'U' for "Maximum u_info[] index" */
	else if (buf[2] == 'U')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->u_max = max;
	}

	/* Process 'H' for "Maximum h_info[] index" */
	else if (buf[2] == 'H')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->h_max = max;
	}

	/* Process 'B' for "Maximum b_info[] subindex" */
	else if (buf[2] == 'B')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->b_max = max;
	}

	/* Process 'b' for "Maximum blow_info[] subindex" */
	else if (buf[2] == 'b')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->blow_max = max;
	}

	/* Process 'Q' for "Maximum q_info[] subindex" */
	else if (buf[2] == 'Q')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->q_max = max;
	}

	/* Process 'O' for "Maximum o_list[] index" */
	else if (buf[2] == 'O')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->o_max = max;
	}

	/* Process 'M' for "Maximum m_list[] index" */
	else if (buf[2] == 'M')
	{
		int max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->m_max = max;
	}

	/* Process 'N' for "Fake name size" */
	else if (buf[2] == 'N')
	{
		long max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%ld", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->fake_name_size = max;
	}

	/* Process 'I' for "Fake text size" */
	else if (buf[2] == 'I')
	{
		long max;

		/* Scan for the value */
		if (1 != sscanf(buf+4, "%ld", &max)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		z_info->fake_text_size = max;
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Initialize the "v_info" array, by parsing an ascii "template" file
 */
errr parse_v_info(char *buf, header *head)
{
	int i;

	char *s;

	/* Current entry */
	static vault_type *v_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		v_ptr = (vault_type*)head->info_ptr + i;

		/* Store the name */
		if (!(v_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current v_ptr */
		if (!v_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&v_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'X' for "Extra info" (one line only) */
	else if (buf[0] == 'X')
	{
		int typ, rat, hgt, wid;

		/* There better be a current v_ptr */
		if (!v_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			    &typ, &rat, &hgt, &wid)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		v_ptr->typ = typ;
		v_ptr->rat = rat;
		v_ptr->hgt = hgt;
		v_ptr->wid = wid;

		/* Check for maximum vault sizes */
		if ((v_ptr->typ == 7) && ((v_ptr->wid > 33) || (v_ptr->hgt > 22)))
			return (PARSE_ERROR_VAULT_TOO_BIG);

		if ((v_ptr->typ == 8) && ((v_ptr->wid > 66) || (v_ptr->hgt > 44)))
			return (PARSE_ERROR_VAULT_TOO_BIG);
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Grab one flag from a textual string
 */
static errr grab_one_flag(u32b *flags, cptr names[], cptr what)
{
	int i;
	
	/* Check flags */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, names[i]))
		{
			*flags |= (1L << i);
			
			return (0);
		}
	}

	return (-1);
}


/*
 * Grab one blow flag in a blow_type from a textual string
 */
static errr grab_one_blow_flag(blow_type *blow_ptr, cptr what)
{
	if (grab_one_flag(&blow_ptr->flags1, blow_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&blow_ptr->flags2, blow_info_flags2, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown blow flag '%s'.", what);

	/* Error */			
	return (PARSE_ERROR_GENERIC);
	
	return (0);
}


/*
* Grab one level scalar from a textual string 		 
*/ 		 
static errr grab_one_level_scalar(blow_level_scalar_type *scalar, char *what) 		 
{ 		 
	char *s, *t, *u; 		 
	  		 
	/* Start at start of string */ 		 
	s = what; 		 
	  		 
	/* Find either a plus or slash */
	for (t = s; *t && (*t != '+') && (*t != '/'); ++t) /* loop */;

	/* Find a slash */
	for (u = s; *u && (*u != '/'); ++u) /* loop */;
	
	/* No plus sign */
	if (*t == '/')
	{
		/* Move t backwards */
		t = s;
		
		/* Find the end of line */
		for (; *s && (*s != '\n'); ++s) /* loop */;			
	}
	/* Prepare t */
	else if (*t == '+')
	{
		/* Nuke plus sign and advance */
		*t++ = '\0';
	}
	
	/* Prepare u */
	if (*u == '/')
	{
		/* Nuke divisor and advance */
		*u++ = '\0';
	}

	/* Parse values */
	scalar->base = atoi(s);
	scalar->gain = atoi(t);
	scalar->levels = atoi(u);
	
	return (0); 		 
}

/*
 * Initialize the "project_info" array, by parsing an ascii "template" file
 */
errr parse_blow_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static blow_type *blow_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		blow_ptr = (blow_type*)head->info_ptr + i;

		/* Store the name */
		if (!(blow_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
		
		/* Set some values */
		blow_ptr->max_range = MAX_SIGHT;		
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current f_ptr */
		if (!blow_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_blow_flag(blow_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'T' for "Blow Description" */
	else if (buf[0] == 'T')
	{
		int n1;
		
		/* There better be a current blow_ptr */
		if (!blow_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Analyze effect */
		for (n1 = 0; blow_ptr->desc[n1].max > 0; n1++) /* loop */ ;

		if (n1 >= MAX_BLOW_DESCRIPTIONS) return (PARSE_ERROR_GENERIC);

		/* Find either a minus sign or colon */
		for (t = s = buf + 2; *t && (*t != '-') && (*t != '<') && (*t != ':'); ++t) /* loop */;

		/* Parse max */
		if ((*t == '-') || (*t == '<'))
		{
			/* Nuke minus size and advance */
			*t++ = '\0';
			
			blow_ptr->desc[n1].max = atoi(t);
			
			/* Find the colon */
			for (; *t && (*t != ':'); ++t) /* loop */;			
		}
		else
		{
			/* TODO: Define a MAX_DAMAGE and start using it */
			blow_ptr->desc[n1].max = 10000;
		}

		/* No colon */
		if (*t == 0) return (PARSE_ERROR_GENERIC);

		/* Nuke colon size and advance */
		*t++ = '\0';

		/* Hack -- allow greater than */
		if (*s == '>') *s++;
		
		/* Parse min */
		blow_ptr->desc[n1].min = atoi(s);
		
		/* Store the description */
		if (!(add_text(&(blow_ptr->desc[n1].text), head, t)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'I' for "Info" */
	else if (buf[0] == 'I')
	{
		int mana, best, max;
		
		/* There better be a current blow_ptr */
		if (!blow_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf, "I:%d:%d:%d",
			    &mana, &best, &max)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		blow_ptr->mana_cost = mana;
		blow_ptr->best_range = best;
		blow_ptr->max_range = max;		
	}
	
	/* Process 'D' for "Damage" */
	else if (buf[0] == 'D')
	{
		int mult, div, var, max;
		int n, n1;
		char *s;
		
		/* There better be a current blow_ptr */
		if (!blow_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* TODO: This is bound to cause memory problems. Should have effect listed first */
		/* Scan for 5th colon */
		for (s = buf, n = 0; *s && (n < 5);)
		{
			s++;
			if (*s == ':') n++;
		}
		
		/* Nuke colon and advance */
		*s++ = '\0';
		
		/* Analyze the effect */
		for (n1 = 0; r_info_blow_effect[n1]; n1++)
		{
			if (streq(s, r_info_blow_effect[n1])) break;
		}
		
		/* Save the value */
		blow_ptr->d_res = n1;
		
		/* Scan for the values */
		if (4 != sscanf(buf, "D:%d:%d:%d:%d",
			    &mult, &div, &var, &max)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		blow_ptr->dam_mult = mult;
		blow_ptr->dam_div = div;
		blow_ptr->dam_var = var;		
		blow_ptr->dam_max = max;		
	}
	
	/* Process 'C' for "Choice" */
	else if (buf[0] == 'C')
	{
		int base, summ, hurt, mana, esc, tact, range;
		
		/* There better be a current blow_ptr */
		if (!blow_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (7 != sscanf(buf, "C:%d:%d:%d:%d:%d:%d:%d",
				&base, &summ, &hurt, &mana, &esc, &tact, &range)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		blow_ptr->d_base = base;
		blow_ptr->d_summ = summ;
		blow_ptr->d_hurt = hurt;		
		blow_ptr->d_mana = mana;
		blow_ptr->d_esc = esc;
		blow_ptr->d_tact = tact;
		blow_ptr->d_range = range;		
	}

	/* Process 'A' for "Arc" */
	else if (buf[0] == 'A')
	{
		int arc, dia, deg;
		
		/* There better be a current blow_ptr */
		if (!blow_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf, "A:%d:%d:%d",
			    &arc, &dia, &deg)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		blow_ptr->arc = arc;
		blow_ptr->diameter_of_source = dia;
		blow_ptr->degree_factor = deg;		
	}
	
	/* Process 'R' for "Radius" */
	else if (buf[0] == 'R')
	{
		/* There better be a current blow_ptr */
		if (!blow_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the radius information */
		if (grab_one_level_scalar(&blow_ptr->radius, buf + 2)) return (PARSE_ERROR_GENERIC);
	}
	
	/* Process 'U' for "Number" */
	else if (buf[0] == 'U')
	{
		/* There better be a current blow_ptr */
		if (!blow_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the radius information */
		if (grab_one_level_scalar(&blow_ptr->number, buf + 2)) return (PARSE_ERROR_GENERIC);
	}
	
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
	
}


/*
 * Grab one flag from a textual string and convert to numeric
 */
static errr grab_one_offset(byte *offset, cptr names[], cptr what)
{
	int i;

	/* Check flags */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, names[i]))
		{
			*offset = *offset+i;
			return (0);
		}
	}

	*offset = *offset+32;
	return (-1);
}


/*
 * Grab one flag from a textual string and convert to numeric
 */
static errr grab_one_offset_u16b(u16b *offset, cptr names[], cptr what)
{
	int i;

	/* Check flags */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, names[i]))
		{
			*offset = *offset + i;
			return (0);
		}
	}

	*offset = *offset + 32;
	return (-1);
}


/*
 * Grab one special flag in an desc_type from a textual string
 */
static errr grab_one_special_flag(desc_type *d_ptr, cptr what)
{
	if (grab_one_flag(&d_ptr->flags, d_info_sflags, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown room special flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Grab one level flag in an desc_type from a textual string
 */
static errr grab_one_place_flag(desc_type *d_ptr, cptr what)
{
	if (grab_one_flag(&d_ptr->p_flag, d_info_pflags, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown room generate flag '%s'.", what);

	/* Error */			
	return (PARSE_ERROR_GENERIC);
}


/*
 * Grab one level flag in an desc_type from a textual string
 */
static errr grab_one_level_flag(desc_type *d_ptr, cptr what)
{
	if (grab_one_flag(&d_ptr->l_flag, d_info_lflags, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown room level flag '%s'.", what);

	/* Error */			
	return (PARSE_ERROR_GENERIC);
}


/*
 * Grab an race flag in an desc_type from a textual string
 */
static errr grab_one_room_race_flag(desc_type *d_ptr, cptr what)
{
	d_ptr->r_flag = 1;

	if (grab_one_offset_u16b(&d_ptr->r_flag, r_info_flags1, what) == 0)
		return (0);

	if (grab_one_offset_u16b(&d_ptr->r_flag, r_info_flags2, what) == 0)
		return (0);

	if (grab_one_offset_u16b(&d_ptr->r_flag, r_info_flags3, what) == 0)
		return (0);

	if (grab_one_offset_u16b(&d_ptr->r_flag, r_info_flags4, what) == 0)
		return (0);

	if (grab_one_offset_u16b(&d_ptr->r_flag, r_info_flags5, what) == 0)
		return (0);

	if (grab_one_offset_u16b(&d_ptr->r_flag, r_info_flags6, what) == 0)
		return (0);

	if (grab_one_offset_u16b(&d_ptr->r_flag, r_info_flags7, what) == 0)
		return (0);

	if (grab_one_offset_u16b(&d_ptr->r_flag, r_info_flags8, what) == 0)
		return (0);

	if (grab_one_offset_u16b(&d_ptr->r_flag, r_info_flags9, what) == 0)
		return (0);
#if 0
	if (grab_one_offset(&d_ptr->r_flag, flow_flags, what) == 0)
		return (0);
#endif

	/* Oops */
	msg_format("Unknown room race flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Initialize the "d_info" array, by parsing an ascii "template" file
 */
errr parse_d_info(char *buf, header *head)
{
	int i;

	char *s,*t;

	/* Current entry */
	static desc_type *d_ptr = NULL;

	/* Process 'N' for "New/Number" */
	if (buf[0] == 'N')
	{
		int prv, nxt, bon, bra, cha, noc, min, max;

		/* Hack - get the index */
		i = error_idx + 1;

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		d_ptr = (desc_type*)head->info_ptr + i;

		/* Scan for the values */
		if (8 != sscanf(buf, "N:%d:%d:%d:%d:%d:%d:%d:%d",
			    &prv, &nxt, &bon, &bra, &cha, &noc, &min, &max)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		d_ptr->chart = prv;
		d_ptr->next = nxt;
		d_ptr->branch_on = bon;
		d_ptr->branch = bra;
		d_ptr->chance = cha;
		d_ptr->not_chance = noc;
		d_ptr->level_min = min;
		d_ptr->level_max = max;

		/* Initialize other values */
		d_ptr->flags = 0;
		d_ptr->tval = 0;
		d_ptr->feat = 0;
		d_ptr->r_flag = 0;
		d_ptr->r_char = 0;

	}
	/* Process 'A' for "Name1" */
	else if (buf[0] == 'A')
	{
		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the name */
		if (!(d_ptr->name1 = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	/* Process 'B' for "Name2" */
	else if (buf[0] == 'B')
	{
		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the name */
		if (!(d_ptr->name2 = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&d_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Hack -- Process 'S' for special flags */
	else if (buf[0] == 'S')
	{
		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_special_flag(d_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Hack -- Process 'P' for placement flags */
	else if (buf[0] == 'P')
	{
		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_place_flag(d_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Hack -- Process 'L' for level flags */
	else if (buf[0] == 'L')
	{
		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_level_flag(d_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		d_ptr->r_char = buf[2];
	}

	/* Process 'K' for "Kind" (one line only) */
	else if (buf[0] == 'K')
	{
		int tval, min_sval, max_sval;

		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d", &tval, &min_sval, &max_sval)) return (1);

		/* Save the values */
		d_ptr->tval = tval;
		d_ptr->min_sval = min_sval;
		d_ptr->max_sval = max_sval;
	}

	/* Process 'F' for "Feature" (one line only) */
	else if (buf[0] == 'F')
	{
		int feat, theme[MAX_THEMES], k;

		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 + MAX_THEMES != sscanf(buf+2, "%d:%d:%d:%d:%d", &feat, &theme[0], &theme[1], &theme[2], &theme[3])) return (1);

		/* Save the values */
		d_ptr->feat = feat;
		
		/* Set the themes */
		for (k = 0; k < MAX_THEMES; k++)
		{
			d_ptr->theme[k] = theme[k];

			if (f_info[d_ptr->theme[k]].flags2 & (FF2_ICE)) d_ptr->l_flag |= LF1_ICE;
			if (f_info[d_ptr->theme[k]].flags2 & (FF2_WATER)) d_ptr->l_flag |= LF1_WATER;
			if (f_info[d_ptr->theme[k]].flags2 & (FF2_LAVA)) d_ptr->l_flag |= LF1_LAVA;
			if (f_info[d_ptr->theme[k]].flags2 & (FF2_ACID)) d_ptr->l_flag |= LF1_ACID;
			if (f_info[d_ptr->theme[k]].flags2 & (FF2_OIL)) d_ptr->l_flag |= LF1_OIL;
			if (f_info[d_ptr->theme[k]].flags2 & (FF2_CHASM)) d_ptr->l_flag |= LF1_CHASM;
			if (f_info[d_ptr->theme[k]].flags3 & (FF3_LIVING)) d_ptr->l_flag |= LF1_LIVING;

			if (f_info[d_ptr->theme[k]].flags2 & (FF2_ICE | FF2_WATER | FF2_LAVA | FF2_ACID | FF2_OIL | FF2_CHASM)) d_ptr->not_chance = 0;
			if (f_info[d_ptr->theme[k]].flags3 & (FF3_LIVING)) d_ptr->not_chance = 0;		
		}
		
		/* Hack -- ensure we have correct level flags */
		if (f_info[d_ptr->feat].flags2 & (FF2_ICE)) d_ptr->l_flag |= LF1_ICE;
		if (f_info[d_ptr->feat].flags2 & (FF2_WATER)) d_ptr->l_flag |= LF1_WATER;
		if (f_info[d_ptr->feat].flags2 & (FF2_LAVA)) d_ptr->l_flag |= LF1_LAVA;
		if (f_info[d_ptr->feat].flags2 & (FF2_ACID)) d_ptr->l_flag |= LF1_ACID;
		if (f_info[d_ptr->feat].flags2 & (FF2_OIL)) d_ptr->l_flag |= LF1_OIL;
		if (f_info[d_ptr->feat].flags2 & (FF2_CHASM)) d_ptr->l_flag |= LF1_CHASM;
		if (f_info[d_ptr->feat].flags3 & (FF3_LIVING)) d_ptr->l_flag |= LF1_LIVING;
	
		/* Ensure that level flags must occur */
		if (f_info[d_ptr->feat].flags2 & (FF2_ICE | FF2_WATER | FF2_LAVA | FF2_ACID | FF2_OIL | FF2_CHASM)) d_ptr->not_chance = 0;
		if (f_info[d_ptr->feat].flags3 & (FF3_LIVING)) d_ptr->not_chance = 0;
	}

	/* Process 'R' for "Race flag" (once only) */
	else if (buf[0] == 'R')
	{
		/* There better be a current f_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Set to the first field */
		s=buf+2;

		/* Parse this entry */
		if (0 != grab_one_room_race_flag(d_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Grab one flag in an feature_type from a textual string
 */
static errr grab_one_feat_flag(feature_type *f_ptr, cptr what)
{
	if (grab_one_flag(&f_ptr->flags1, f_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&f_ptr->flags2, f_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&f_ptr->flags3, f_info_flags3, what) == 0)
		return (0);
#if 0
	if (grab_one_flag(&f_ptr->flows, flow_flags, what) == 0)
		return (0);
#endif
	/* Oops */
	msg_format("Unknown feature flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Grab an action in an feature_type from a textual string
 */
static errr grab_one_feat_action(feature_type *f_ptr, cptr what, int count)
{
	if (grab_one_offset(&f_ptr->state[count].action, f_info_flags1, what) == 0)
		return (0);

	if (grab_one_offset(&f_ptr->state[count].action, f_info_flags2, what) == 0)
		return (0);

	if (grab_one_offset(&f_ptr->state[count].action, f_info_flags3, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown feature action '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Grab an spell in an feature_type from a textual string
 */
static errr grab_one_feat_spell(feature_type *f_ptr, cptr what)
{
	f_ptr->spell = 96;

	if (grab_one_offset(&f_ptr->spell, r_info_flags4, what) == 0)
		return (0);

	if (grab_one_offset(&f_ptr->spell, r_info_flags5, what) == 0)
		return (0);

	if (grab_one_offset(&f_ptr->spell, r_info_flags6, what) == 0)
		return (0);

	if (grab_one_offset(&f_ptr->spell, r_info_flags7, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown feature spell '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Initialize the "f_info" array, by parsing an ascii "template" file
 */
errr parse_f_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static feature_type *f_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		f_ptr = (feature_type*)head->info_ptr + i;

		/* Store the name */
		if (!(f_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Default "mimic" */
		f_ptr->mimic = i;

		/* Default "unseen" */
		f_ptr->unseen = i;

		/* Default "state change" -- if not specified */
		f_ptr->defaults = i;

		/* Default "states" */
		for (i = 0; i < MAX_FEAT_STATES; i++) f_ptr->state[i].action = FS_FLAGS_END;

		/* Hack -- handle graphics */
		/* Note that in a future version of Unangband, a preference 'Use special lighting
		 * for all features' will set this flag for all features, and the features that are
		 * dynamically lit in vanilla Angband will have this flag in terrain.txt.
		 */
		f_ptr->flags3 |= (FF3_ATTR_LITE);
	}

	/* Process 'M' for "Mimic" (one line only) */
	else if (buf[0] == 'M')
	{
		int mimic;

		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d",
			    &mimic)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		f_ptr->mimic = mimic;
	}

	/* Process 'U' for "Unseen" (one line only) */
	else if (buf[0] == 'U')
	{
		int unseen;

		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d",
			    &unseen)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		f_ptr->unseen = unseen;
	}

	/* Process 'O' for "Object" (one line only) */
	else if (buf[0] == 'O')
	{
		int k_idx;

		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d",
			    &k_idx)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		f_ptr->k_idx = k_idx;
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		int tmp;

		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);
		if (!buf[3]) return (PARSE_ERROR_GENERIC);
		if (!buf[4]) return (PARSE_ERROR_GENERIC);

		/* Extract the attr */
		tmp = color_char_to_attr(buf[4]);

		/* Paranoia */
		if (tmp < 0) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		f_ptr->d_attr = tmp;
		f_ptr->d_char = buf[2];
	}
	
	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* XXX XXX XXX Hack -- Read feature power */
			if (1 == sscanf(s, "POWER_%d", &i))
			{
				/* Extract a "frequency" */
				f_ptr->power =  i;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* Parse this entry */
			if (0 != grab_one_feat_flag(f_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int level, rarity, priority,edge;
		
		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
				&level, &rarity, &priority, &edge)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		f_ptr->level = (u16b)level;
		f_ptr->rarity = (u16b)rarity;
		f_ptr->priority = (u16b)priority;
		f_ptr->edge = (s16b)edge;
	}

	/* Process 'K' for "States" (up to eight lines + default (which cannot be last)) */
	else if (buf[0] == 'K')
	{
		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Find the next empty state slot (if any) */
		for (i = 0; i < MAX_FEAT_STATES; i++) if (f_ptr->state[i].action == FS_FLAGS_END) break;

		/* Oops, no more slots */
		if (i == MAX_FEAT_STATES) return (PARSE_ERROR_GENERIC);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Is this default entry? */
		if (streq(s, "DEFAULT"))
		{
			/* Analyze result */
			f_ptr->defaults = atoi(t);
		}
		else
		{
			/* Reset */
			f_ptr->state[i].action = 0;
			
			/* Parse this entry */
			if (0 != grab_one_feat_action(f_ptr, s, i)) return (PARSE_ERROR_INVALID_FLAG);

			/* Analyze result */
			f_ptr->state[i].result = atoi(t);
		}
	}

	/* Process 'T' for "Traps" (one line only) */
	else if (buf[0] == 'T')
	{
		int n1;

		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze effect */
		for (n1 = 0; r_info_blow_effect[n1]; n1++)
		{
			if (streq(s, r_info_blow_effect[n1])) break;
		}

		/* Invalid effect */
		if (!r_info_blow_effect[n1]) return (PARSE_ERROR_GENERIC);

		/* Analyze the third field */
		for (s = t; *t && (*t != 'd'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == 'd') *t++ = '\0';

		/* Save the method */
		f_ptr->blow.method = RBM_TRAP;

		/* Save the effect */
		f_ptr->blow.effect = n1;

		/* Extract the damage dice and sides */
		f_ptr->blow.d_dice = atoi(s);
		f_ptr->blow.d_side = atoi(t);
	}

	/* Process 'S' for "Spell" (once only) */
	else if (buf[0] == 'S')
	{
		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Set to the first field */
		s=buf+2;

		/* Parse this entry */
		if (0 != grab_one_feat_spell(f_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&f_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one flag in an object_kind from a textual string
 */
static errr grab_one_kind_flag(object_kind *k_ptr, cptr what)
{
	if (grab_one_flag(&k_ptr->flags1, k_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&k_ptr->flags2, k_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&k_ptr->flags3, k_info_flags3, what) == 0)
		return (0);

	if (grab_one_flag(&k_ptr->flags4, k_info_flags4, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown object flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}



/*
 * Initialize the "k_info" array, by parsing an ascii "template" file
 */
errr parse_k_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static object_kind *k_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		k_ptr = (object_kind*)head->info_ptr + i;

		/* Store the name */
		if (!(k_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		char sym;
		int tmp;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);
		if (!buf[3]) return (PARSE_ERROR_GENERIC);
		if (!buf[4]) return (PARSE_ERROR_GENERIC);

		/* Extract the char */
		sym = buf[2];

		/* Extract the attr */
		tmp = color_char_to_attr(buf[4]);

		/* Paranoia */
		if (tmp < 0) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		k_ptr->d_attr = tmp;
		k_ptr->d_char = sym;
	}

	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		int tval, sval, pval;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
			    &tval, &sval, &pval)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		k_ptr->tval = tval;
		k_ptr->sval = sval;
		k_ptr->pval = pval;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int level, charges, wgt;
		long cost;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
			    &level, &charges, &wgt, &cost)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		k_ptr->level = level;
		k_ptr->charges = charges;
		k_ptr->weight = wgt;
		k_ptr->cost = cost;
	}

	/* Process 'A' for "Allocation" (one line only) */
	else if (buf[0] == 'A')
	{
		int i;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* XXX Simply read each number following a colon */
		for (i = 0, s = buf+1; s && (s[0] == ':') && s[1]; ++i)
		{
			/* Sanity check */
			if (i > 3) return (PARSE_ERROR_TOO_MANY_ALLOCATIONS);

			/* Default chance */
			k_ptr->chance[i] = 1;

			/* Store the attack damage index */
			k_ptr->locale[i] = atoi(s+1);

			/* Find the slash */
			t = strchr(s+1, '/');

			/* Find the next colon */
			s = strchr(s+1, ':');

			/* If the slash is "nearby", use it */
			if (t && (!s || t < s))
			{
				int chance = atoi(t+1);
				if (chance > 0) k_ptr->chance[i] = chance;
			}
		}
	}

	/* Hack -- Process 'P' for "power" and such */
	else if (buf[0] == 'P')
	{
		int ac, hd1, hd2, th, td, ta;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
			    &ac, &hd1, &hd2, &th, &td, &ta)) return (PARSE_ERROR_GENERIC);

		k_ptr->ac = ac;
		k_ptr->dd = hd1;
		k_ptr->ds = hd2;
		k_ptr->to_h = th;
		k_ptr->to_d = td;
		k_ptr->to_a =  ta;
	}

	/* Process 'Y' for "Rune" (one line only) */
	else if (buf[0] == 'Y')
	{
		int runest,runesc;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d",
			    &runest,&runesc)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		k_ptr->runest = runest;
		k_ptr->runesc = runesc;
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_kind_flag(k_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&k_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Grab one flag in an artifact_type from a textual string
 */
static errr grab_one_artifact_flag(artifact_type *a_ptr, cptr what)
{
	if (grab_one_flag(&a_ptr->flags1, k_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&a_ptr->flags2, k_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&a_ptr->flags3, k_info_flags3, what) == 0)
		return (0);

	if (grab_one_flag(&a_ptr->flags4, k_info_flags4, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown artifact flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Initialize the "a_info" array, by parsing an ascii "template" file
 */
errr parse_a_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static artifact_type *a_ptr = NULL;


	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		a_ptr = (artifact_type*)head->info_ptr + i;

		/* Store the name */
		if (!(a_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Ignore everything */
		a_ptr->flags2 |= (TR2_IGNORE_MASK);
	}

	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		int tval, sval, pval;

		/* There better be a current a_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
			    &tval, &sval, &pval)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		a_ptr->tval = tval;
		a_ptr->sval = sval;
		a_ptr->pval = pval;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int level, rarity, wgt;
		long cost;

		/* There better be a current a_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
			    &level, &rarity, &wgt, &cost)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		a_ptr->level = level;
		a_ptr->rarity = rarity;
		a_ptr->weight = wgt;
		a_ptr->cost = cost;
	}

	/* Process 'P' for "power" and such */
	else if (buf[0] == 'P')
	{
		int ac, hd1, hd2, th, td, ta;

		/* There better be a current a_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
			    &ac, &hd1, &hd2, &th, &td, &ta)) return (PARSE_ERROR_GENERIC);

		a_ptr->ac = ac;
		a_ptr->dd = hd1;
		a_ptr->ds = hd2;
		a_ptr->to_h = th;
		a_ptr->to_d = td;
		a_ptr->to_a = ta;
	}

	/* Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current a_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_artifact_flag(a_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'A' for "Activation & time" */
	else if (buf[0] == 'A')
	{
		int act, time, rand;

		/* Scan for the values */
		if (3 != sscanf(buf + 2, "%d:%d:%d",&act, &time, &rand)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		a_ptr->activation = act;
		a_ptr->time = time;
		a_ptr->randtime = rand;
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Add a name to the probability tables
 */
static errr build_prob(char *name, names_type *n_ptr)
{
	int c_prev, c_cur, c_next;

	while (*name && !isalpha((unsigned char) *name))
      ++name;

	if (!*name)	return PARSE_ERROR_GENERIC;

	c_prev = c_cur = S_WORD;

	do
	{
		if (isalpha ((unsigned char) *name))
		{
			c_next = A2I (tolower ((unsigned char) *name));
			n_ptr->lprobs[c_prev][c_cur][c_next]++;
			n_ptr->ltotal[c_prev][c_cur]++;
			c_prev = c_cur;
			c_cur = c_next;
		}
	}
	while (*++name);

	n_ptr->lprobs[c_prev][c_cur][E_WORD]++;
	n_ptr->ltotal[c_prev][c_cur]++;

	return 0;
}

/*
 * Initialize the "n_info" array, by parsing an ascii "template" file
 */
errr parse_n_info(char *buf, header *head)
{
	names_type *n_ptr = head->info_ptr;

	/*
	 * This function is called once, when the raw file does not exist.
	 * If you want to initialize some stuff before parsing the txt file
 	 * you can do:
	 *
	 * static int do_init = 1;
	 *
	 * if (do_init)
	 * {
	 *    do_init = 0;
	 *    ...
	 *    do_stuff_with_n_ptr
	 *    ...
	 * }
	 *
	 */

	if (buf[0] == 'N')
	{
	    	return build_prob (buf + 2, n_ptr);
	}

 	/*
	 * If you want to do something after parsing the file you can add
	 * a special directive at the end of the txt file, like:
	 *
	 * else
	 * if (buf[0] == 'X')          (Only at the end of the txt file)
	 * {
	 *    ...
	 *    do_something_else_with_n_ptr
	 *    ...
	 * }
	 *
	 */
	else
	{
    	/* Oops */
    	return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
  	}
}


/*
 * Grab one flag in a ego-item_type from a textual string
 */
static bool grab_one_ego_item_flag(ego_item_type *e_ptr, cptr what)
{
	if (grab_one_flag(&e_ptr->flags1, k_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&e_ptr->flags2, k_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&e_ptr->flags3, k_info_flags3, what) == 0)
		return (0);

	if (grab_one_flag(&e_ptr->flags4, k_info_flags4, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown ego-item flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Grab one flag in a ego-item_type from a textual string
 */
static bool grab_one_obvious_ego_item_flag(ego_item_type *e_ptr, cptr what)
{
	if (grab_one_flag(&e_ptr->obv_flags1, k_info_flags1, what) == 0)
        {
                e_ptr->flags1 |= e_ptr->obv_flags1;
		return (0);
        }

	if (grab_one_flag(&e_ptr->obv_flags2, k_info_flags2, what) == 0)
        {
                e_ptr->flags2 |= e_ptr->obv_flags2;
		return (0);
        }

	if (grab_one_flag(&e_ptr->obv_flags3, k_info_flags3, what) == 0)
        {
                e_ptr->flags3 |= e_ptr->obv_flags3;
		return (0);
        }

	if (grab_one_flag(&e_ptr->obv_flags4, k_info_flags4, what) == 0)
        {
                e_ptr->flags4 |= e_ptr->obv_flags4;
		return (0);
        }

	/* Oops */
	msg_format("Unknown ego-item flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Initialize the "e_info" array, by parsing an ascii "template" file
 */
errr parse_e_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static ego_item_type *e_ptr = NULL;

	static int cur_t = 0;


	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		e_ptr = (ego_item_type*)head->info_ptr + i;

		/* Store the name */
		if (!(e_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Start with the first of the tval indices */
		cur_t = 0;

                /* Fix initialisation */
                e_ptr->aware = 0;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int level, rarity, pad2;
		long cost;

		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
			    &level, &rarity, &pad2, &cost)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		e_ptr->level = level;
		e_ptr->rarity = rarity;
		/* e_ptr->weight = wgt; */
		e_ptr->cost = cost;
	}

	/* Process 'X' for "Xtra" (one line only) */
	else if (buf[0] == 'X')
	{
		int slot, rating, xtra;

		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
			    &slot, &rating, &xtra)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		e_ptr->slot = slot;
		e_ptr->rating = rating;
		e_ptr->xtra = xtra;
	}

	/* Process 'T' for "Types allowed" (up to three lines) */
	else if (buf[0] == 'T')
	{
		int tval, sval1, sval2;

		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
			    &tval, &sval1, &sval2)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		e_ptr->tval[cur_t] = (byte)tval;
		e_ptr->min_sval[cur_t] = (byte)sval1;
		e_ptr->max_sval[cur_t] = (byte)sval2;

		/* increase counter for 'possible tval' index */
		cur_t++;

		/* only three T: lines allowed */
		if (cur_t > 3) return (PARSE_ERROR_GENERIC);
	}

	/* Hack -- Process 'C' for "creation" */
	else if (buf[0] == 'C')
	{
		int th, td, ta, pv;

		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			    &th, &td, &ta, &pv)) return (PARSE_ERROR_GENERIC);

		e_ptr->max_to_h = th;
		e_ptr->max_to_d = td;
		e_ptr->max_to_a = ta;
		e_ptr->max_pval = pv;
	}

	/* Process 'Y' for "Rune" (one line only) */
	else if (buf[0] == 'Y')
	{
		int runest,runesc;

		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d",
			    &runest,&runesc)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		e_ptr->runest = runest;
		e_ptr->runesc = runesc;
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_ego_item_flag(e_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Hack -- Process 'O' for obvious flags */
	else if (buf[0] == 'O')
	{
		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_obvious_ego_item_flag(e_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}
	/* Process 'A' for "Activation & time" */
	else if (buf[0] == 'A')
	{
		int act, time, rand;

		/* Scan for the values */
		if (3 != sscanf(buf + 2, "%d:%d:%d",&act, &time, &rand)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		e_ptr->activation = act;
		e_ptr->time = time;
		e_ptr->randtime = rand;
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Initialize the "x_info" array, by parsing an ascii "template" file
 */
errr parse_x_info(char *buf, header *head)
{
	int i;
	
	/* Current entry */
	static flavor_type *x_ptr;


	/* Process 'N' for "Number" */
	if (buf[0] == 'N')
	{
		int tval, sval;
		int result;

		/* Scan the value */
		result = sscanf(buf, "N:%d:%d:%d", &i, &tval, &sval);

		/* Either two or three values */
		if ((result != 2) && (result != 3)) return (PARSE_ERROR_GENERIC);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		x_ptr = (flavor_type*)head->info_ptr + i;

		/* Save the tval */
		x_ptr->tval = (byte)tval;

		/* Save the sval */
		if (result == 2)
		{
			/* Megahack - unknown sval */
			x_ptr->sval = SV_UNKNOWN;
		}
		else
			x_ptr->sval = (byte)sval;
	}

	/* Process 'G' for "Graphics" */
	else if (buf[0] == 'G')
	{
		char d_char;
		int d_attr;

		/* There better be a current x_ptr */
		if (!x_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);
		if (!buf[3]) return (PARSE_ERROR_GENERIC);
		if (!buf[4]) return (PARSE_ERROR_GENERIC);

		/* Extract d_char */
		d_char = buf[2];

		/* If we have a longer string than expected ... */
		if (buf[5])
		{
			/* Advance "buf" on by 4 */
			buf += 4;

			/* Extract the colour */
			d_attr = color_text_to_attr(buf);
		}
		else
		{
			/* Extract the attr */
			d_attr = color_char_to_attr(buf[4]);
		}

		/* Paranoia */
		if (d_attr < 0) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		x_ptr->d_attr = d_attr;
		x_ptr->d_char = d_char;
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current x_ptr */
		if (!x_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[1]) return (PARSE_ERROR_GENERIC);
		if (!buf[2]) return (PARSE_ERROR_GENERIC);

		/* Store the text */
		if (!add_text(&x_ptr->text, head, buf + 2))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Grab one (basic) flag in a monster_race from a textual string
 */
static errr grab_one_basic_flag(monster_race *r_ptr, cptr what)
{
	if (grab_one_flag(&r_ptr->flags1, r_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->flags2, r_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->flags3, r_info_flags3, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->flags8, r_info_flags8, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->flags9, r_info_flags9, what) == 0)
		return (0);
#if 0
	if (grab_one_flag(&r_ptr->flows, flow_flags, what) == 0)
		return (0);
#endif
	/* Oops */
	msg_format("Unknown monster flag '%s'.", what);

	/* Failure */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Grab one (spell) flag in a monster_race from a textual string
 */
static errr grab_one_spell_flag(monster_race *r_ptr, cptr what)
{
	if (grab_one_flag(&r_ptr->flags4, r_info_flags4, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->flags5, r_info_flags5, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->flags6, r_info_flags6, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->flags7, r_info_flags7, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown monster spell '%s'.", what);

	/* Failure */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Initialize the "r_info" array, by parsing an ascii "template" file
 */
errr parse_r_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static monster_race *r_ptr = NULL;


	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		r_ptr = (monster_race*)head->info_ptr + i;

		/* Store the name */
		if (!(r_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&(r_ptr->text), head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Hack: assume 'D' is always after 'B' and 'F' and catch
		   monsters than have no attacks but no NEVER_BLOW flag */
		if (!(r_ptr->flags1 & RF1_NEVER_BLOW))
		  {
		    int blows = 0;

		    for (i = 0; i < 4; i++) 
		      if (r_ptr->blow[i].method > 0
			  && r_ptr->blow[i].method <= RBM_MAX_NORMAL)
			blows++;

		    if (!blows)
		      return (PARSE_ERROR_GENERIC);
		  }
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		char sym;
		int tmp;

		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);
		if (!buf[3]) return (PARSE_ERROR_GENERIC);
		if (!buf[4]) return (PARSE_ERROR_GENERIC);

		/* Extract the char */
		sym = buf[2];

		/* Extract the attr */
		tmp = color_char_to_attr(buf[4]);

		/* Paranoia */
		if (tmp < 0) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		r_ptr->d_attr = tmp;
		r_ptr->d_char = sym;
	}
	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		int spd, hp1, hp2, aaf, ac, slp;

		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the other values */
		if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
			    &spd, &hp1, &hp2, &aaf, &ac, &slp)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		r_ptr->speed = spd;
		r_ptr->hdice = hp1;
		r_ptr->hside = hp2;
		r_ptr->aaf = aaf;
		r_ptr->ac = ac;
		r_ptr->sleep = slp;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int lev, rar, grp;
		long exp;

		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
			    &lev, &rar, &grp, &exp)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		r_ptr->level = lev;
		r_ptr->rarity = rar;
		r_ptr->grp_idx = grp;
		r_ptr->mexp = exp;
	}

	/* Process 'M' for "Magic Info" (one line only) */
	else if (buf[0] == 'M')
	{
		int innate, spell, power, mana;

		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			    &innate, &spell, &power, &mana)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		r_ptr->freq_innate = innate;
		r_ptr->freq_spell = spell;
		/* No more spellcasters with no spell power */
		r_ptr->spell_power = MAX(1 + r_ptr->level / 10, power);
		if (mana > 250) 
			return (PARSE_ERROR_MISSING_RECORD_HEADER);
		r_ptr->mana = mana;
	}

	/* Process 'B' for "Blows" (up to four lines) */
	else if (buf[0] == 'B')
	{
		int n1, n2;

		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Find the next empty blow slot (if any) */
		for (i = 0; i < 4; i++) if (!r_ptr->blow[i].method) break;

		/* Oops, no more slots */
		if (i == 4) return (PARSE_ERROR_GENERIC);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze the method */
		for (n1 = 0; r_info_blow_method[n1]; n1++)
		{
			if (streq(s, r_info_blow_method[n1])) break;
		}

		/* Invalid method */
		if (!r_info_blow_method[n1]) return (PARSE_ERROR_GENERIC);

		/* Update the rf4 flags */
		if (n1 >= RBM_MIN_RANGED) r_ptr->flags4 |= (RF4_BLOW_1 << i);

		/* Analyze the second field */
		for (s = t; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze effect */
		for (n2 = 0; r_info_blow_effect[n2]; n2++)
		{
			if (streq(s, r_info_blow_effect[n2])) break;
		}

		/* Invalid effect */
		if (!r_info_blow_effect[n2]) return (PARSE_ERROR_GENERIC);

		/* Analyze the third field */
		for (s = t; *t && (*t != 'd'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == 'd') *t++ = '\0';

		/* Save the method */
		r_ptr->blow[i].method = n1;

		/* Save the effect */
		r_ptr->blow[i].effect = n2;

		/* Extract the damage dice and sides */
		r_ptr->blow[i].d_dice = atoi(s);
		r_ptr->blow[i].d_side = atoi(t);

		/* Catch fraudulent NEVER_BLOW monsters */
		if (n1 <= RBM_MAX_NORMAL && r_ptr->flags1 & RF1_NEVER_BLOW)
		  return (PARSE_ERROR_GENERIC);
	}
	/* Process 'F' for "Basic Flags" (multiple lines) */
	else if (buf[0] == 'F')
	{
		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_basic_flag(r_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}

		/* Catch fraudulent NEVER_BLOW monsters */
		if (r_ptr->flags1 & RF1_NEVER_BLOW)
		  for (i = 0; i < 4; i++) 
		    if (r_ptr->blow[i].method > 0
			&& r_ptr->blow[i].method <= RBM_MAX_NORMAL)
		      return (PARSE_ERROR_GENERIC);		  
	}

	/* Process 'S' for "Spell Flags" (multiple lines) */
	else if (buf[0] == 'S')
	{
		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* XXX Hack -- Read spell frequency */
			if (1 == sscanf(s, "1_IN_%d", &i))
			{
				/* Sanity check */
				if ((i < 1) || (i > 100))
					return (PARSE_ERROR_INVALID_SPELL_FREQ);

				/* Extract a "frequency" */
				r_ptr->freq_spell = r_ptr->freq_innate = 100 / i;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* Parse this entry */
			if (0 != grab_one_spell_flag(r_ptr, s))
				return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

#if 0
	/* XXX XXX XXX The ghost is unused */

	/* Mega-Hack -- acquire "ghost" */
	r_ptr = &r_info[z_info->r_max-1];

	/* Get the next index */
	r_ptr->name = head->name_size;
	r_ptr->text = head->text_size;

	/* Save some space for the ghost info */
	head->name_size += 64;
	head->text_size += 64;

	/* Hack -- Default name/text for the ghost */
	strcpy(r_name + r_ptr->name, "Nobody, the Undefined Ghost");
	strcpy(r_text + r_ptr->text, "It seems strangely familiar...");

	/* Hack -- set the attr/char info */
	r_ptr->d_attr = r_ptr->x_attr = TERM_WHITE;
	r_ptr->d_char = r_ptr->x_char = 'G';

	/* Hack -- Try to prevent a few "potential" bugs */
	r_ptr->flags1 |= (RF1_UNIQUE);

	/* Hack -- Try to prevent a few "potential" bugs */
	r_ptr->flags1 |= (RF1_NEVER_MOVE | RF1_NEVER_BLOW);

	/* Hack -- Try to prevent a few "potential" bugs */
	r_ptr->hdice = r_ptr->hside = 1;

	/* Hack -- Try to prevent a few "potential" bugs */
	r_ptr->mexp = 1L;
#endif

	/* Success */
	return (0);
}


/*
 * Grab one flag in a player_race from a textual string
 */
static errr grab_one_racial_flag(player_race *pr_ptr, cptr what)
{
	if (grab_one_flag(&pr_ptr->flags1, k_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&pr_ptr->flags2, k_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&pr_ptr->flags3, k_info_flags3, what) == 0)
		return (0);

	if (grab_one_flag(&pr_ptr->flags4, k_info_flags4, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown player flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}



/*
 * Initialize the "p_info" array, by parsing an ascii "template" file
 */
errr parse_p_info(char *buf, header *head)
{
	int i, j;

	char *s, *t;

	/* Current entry */
	static player_race *pr_ptr = NULL;


	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		pr_ptr = (player_race*)head->info_ptr + i;

		/* Store the name */
		if (!(pr_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&pr_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	
	/* Process 'S' for "Stats" (one line only) */
	else if (buf[0] == 'S')
	{
		int adj;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Start the string */
		s = buf+1;

		/* For each stat */
		for (j = 0; j < A_MAX; j++)
		{
			/* Find the colon before the subindex */
			s = strchr(s, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the subindex */
			*s++ = '\0';

			/* Get the value */
			adj = atoi(s);

			/* Save the value */
			pr_ptr->r_adj[j] = adj;

			/* Next... */
			continue;
		}
	}

	/* Process 'R' for "Racial Skills" (one line only) */
	else if (buf[0] == 'R')
	{
		int dis, dev, sav, stl, srh, dig, tht, thn, thb;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (9 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d",
			    &dis, &dev, &sav, &stl,
			    &srh, &dig, &tht, &thn, &thb)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pr_ptr->r_dis = dis;
		pr_ptr->r_dev = dev;
		pr_ptr->r_sav = sav;
		pr_ptr->r_stl = stl;
		pr_ptr->r_srh = srh;
		pr_ptr->r_dig = dig;
		pr_ptr->r_tht = tht;
		pr_ptr->r_thn = thn;
		pr_ptr->r_thb = thb;
	}

	/* Process 'X' for "Extra Info" (one line only) */
	else if (buf[0] == 'X')
	{
		int exp, infra, r_idx, slot;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			    &exp, &infra, &r_idx, &slot)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pr_ptr->r_exp = exp;
		pr_ptr->infra = infra;
		pr_ptr->r_idx = r_idx;
		pr_ptr->slots[END_EQUIPMENT - INVEN_WIELD] = slot;
	}

	/* Hack -- Process 'I' for "info" and such */
	else if (buf[0] == 'I')
	{
		int hist, b_age, m_age, home;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			    &hist, &b_age, &m_age, &home)) return (PARSE_ERROR_GENERIC);

		pr_ptr->hist = hist;
		pr_ptr->b_age = b_age;
		pr_ptr->m_age = m_age;
		pr_ptr->home = home;
	}

	/* Hack -- Process 'H' for "Height" */
	else if (buf[0] == 'H')
	{
		int m_b_ht, m_m_ht, f_b_ht, f_m_ht;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			    &m_b_ht, &m_m_ht, &f_b_ht, &f_m_ht)) return (PARSE_ERROR_GENERIC);

		pr_ptr->m_b_ht = m_b_ht;
		pr_ptr->m_m_ht = m_m_ht;
		pr_ptr->f_b_ht = f_b_ht;
		pr_ptr->f_m_ht = f_m_ht;
	}

	/* Hack -- Process 'W' for "Weight" */
	else if (buf[0] == 'W')
	{
		int m_b_wt, f_b_wt;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d",
			    &m_b_wt, &f_b_wt)) return (PARSE_ERROR_GENERIC);

		pr_ptr->m_b_wt = m_b_wt;
		pr_ptr->f_b_wt = f_b_wt;
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_racial_flag(pr_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'O' for "Built-in Objects" (one line only) */
	else if (buf[0] == 'O')
	{
		i = 0;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s && (i < END_EQUIPMENT - INVEN_WIELD); )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ':'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
			}

			/* Parse this entry */
			pr_ptr->slots[i] = atoi(s);

			/* Start the next entry */
			s = t;
			i++;
		}
	}

	/* Hack -- Process 'C' for class choices */
	else if (buf[0] == 'C')
	{
		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Hack - Parse this entry */
			pr_ptr->choice |= (1 << atoi(s));

			/* Start the next entry */
			s = t;
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Initialize the "c_info" array, by parsing an ascii "template" file
 */
errr parse_c_info(char *buf, header *head)
{
	int i, j;

	char *s;

	/* Current entry */
	static player_class *pc_ptr = NULL;

	static int cur_title = 0;
	static int cur_equip = 0;


	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		pc_ptr = (player_class*)head->info_ptr + i;

		/* Store the name */
		if (!(pc_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* No titles and equipment yet */
		cur_title = 0;
		cur_equip = 0;
	}

	/* Process 'S' for "Stats" (one line only) */
	else if (buf[0] == 'S')
	{
		int adj;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Start the string */
		s = buf+1;

		/* For each stat */
		for (j = 0; j < A_MAX; j++)
		{
			/* Find the colon before the subindex */
			s = strchr(s, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the subindex */
			*s++ = '\0';

			/* Get the value */
			adj = atoi(s);

			/* Save the value */
			pc_ptr->c_adj[j] = adj;

			/* Next... */
			continue;
		}
	}

	/* Process 'C' for "Class Skills" (one line only) */
	else if (buf[0] == 'C')
	{
		int dis, dev, sav, stl, srh, dig, tht, thn, thb;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (9 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d",
			    &dis, &dev, &sav, &stl,
			    &srh, &dig, &tht, &thn, &thb)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pc_ptr->c_dis = dis;
		pc_ptr->c_dev = dev;
		pc_ptr->c_sav = sav;
		pc_ptr->c_stl = stl;
		pc_ptr->c_srh = srh;
		pc_ptr->c_dig = dig;
		pc_ptr->c_tht = tht;
		pc_ptr->c_thn = thn;
		pc_ptr->c_thb = thb;
	}

	/* Process 'X' for "Extra Skills" (one line only) */
	else if (buf[0] == 'X')
	{
		int dis, dev, sav, stl, srh, dig, tht, thn, thb;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (9 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d",
			    &dis, &dev, &sav, &stl,
			    &srh, &dig, &tht, &thn, &thb)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pc_ptr->x_dis = dis;
		pc_ptr->x_dev = dev;
		pc_ptr->x_sav = sav;
		pc_ptr->x_stl = stl;
		pc_ptr->x_srh = srh;
		pc_ptr->x_dig = dig;
		pc_ptr->x_tht = tht;
		pc_ptr->x_thn = thn;
		pc_ptr->x_thb = thb;
	}

	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		int exp, sense_div, sense_type, sense_squared;
		long sense_base;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (5 != sscanf(buf+2, "%d:%ld:%d:%d:%d",
			    &exp, &sense_base, &sense_div, &sense_type, &sense_squared))
			return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pc_ptr->c_exp = exp;
		pc_ptr->sense_base = sense_base;
		pc_ptr->sense_div = sense_div;
		pc_ptr->sense_type = sense_type;
		pc_ptr->sense_squared = sense_squared;
	}

	/* Process 'A' for "Attack Info" (one line only) */
	else if (buf[0] == 'A')
	{
		int max_attacks, min_weight, att_multiply, chg_weight;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			    &max_attacks, &min_weight, &att_multiply, &chg_weight))
			return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pc_ptr->max_attacks = max_attacks;
		pc_ptr->min_weight = min_weight;
		pc_ptr->att_multiply = att_multiply;
		pc_ptr->chg_weight = chg_weight;
	}

	/* Process 'M' for "Magic Info" (one line only) */
	else if (buf[0] == 'M')
	{
		int spell_book, spell_stat_study, spell_stat_mana, spell_stat_fail, spell_first, spell_weight, spell_power;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (7 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d",
		&spell_book, &spell_stat_study, &spell_stat_mana, &spell_stat_fail,
		&spell_first, &spell_weight, &spell_power))
			return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pc_ptr->spell_book = spell_book;
		pc_ptr->spell_stat_study = spell_stat_study;
		pc_ptr->spell_stat_mana = spell_stat_mana;
		pc_ptr->spell_stat_fail = spell_stat_fail;
		pc_ptr->spell_first = spell_first;
		pc_ptr->spell_weight = spell_weight;
		pc_ptr->spell_power = spell_power;
	}

	/* Process 'T' for "Titles" */
	else if (buf[0] == 'T')
	{
		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&pc_ptr->title[cur_title], head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
		
		/* Next title */
		cur_title++;

		/* Limit number of titles */
		if (cur_title > PY_MAX_LEVEL / 5)
			return (PARSE_ERROR_TOO_MANY_ARGUMENTS);
	}

	/* Process 'E' for "Starting Equipment" */
	else if (buf[0] == 'E')
	{
		int tval, sval, number_min, number_max, charge_min, charge_max, social_min, social_max;

		start_item *e_ptr;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Access the item */
		e_ptr = &pc_ptr->start_items[cur_equip];

		/* Scan for the values */
		if (8 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d",
			    &tval, &sval, &number_min, &number_max, &charge_min, &charge_max, &social_min, &social_max)) return (PARSE_ERROR_GENERIC);

		if ((number_min < 0) || (number_max < 0) || (number_min > 99) || (number_max > 99))
			return (PARSE_ERROR_INVALID_ITEM_NUMBER);

		/* Save the values */
		e_ptr->tval = tval;
		e_ptr->sval = sval;
		e_ptr->number_min = number_min;
		e_ptr->number_max = number_max;
		e_ptr->charge_min = charge_min;
		e_ptr->charge_max = charge_max;
		e_ptr->social_min = social_min;
		e_ptr->social_max = social_max;

		/* Next item */
		cur_equip++;

		/* Limit number of starting items */
		if (cur_equip > MAX_CLASS_ITEMS)
			return (PARSE_ERROR_GENERIC);
	}

	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Grab one style in a weapon style from a textual string
 */
static errr grab_one_style(weapon_style *w_ptr, cptr what)
{
	int i;

	/* Check styles */
	for (i = 0; i < MAX_WEAP_STYLES; i++)
	{
		if (streq(what, w_info_style[i]))
		{
			w_ptr->styles |= (1L << i);
			return (0);
		}
	}

	/* Oops */
	msg_format("Unknown weapon style '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Grab one style in a weapon style from a textual string
 */
static errr grab_one_benefit(weapon_style *w_ptr, cptr what)
{
	int i;

	/* Check styles */
	for (i = 0; i < MAX_WEAP_BENEFITS; i++)
	{
		if (streq(what, w_info_benefit[i]))
		{
			w_ptr->benefit =i;
			return (0);
		}
	}

	/* Oops */
	msg_format("Unknown weapon benefit '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}



/*
 * Initialize the "w_info" array, by parsing an ascii "template" file
 */
errr parse_w_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static weapon_style *w_ptr = NULL;

	/* Process 'W' for "Class/Level" */
	if (buf[0] == 'W')
	{
		int class,level;

		/* Hack - get the index */
		i = error_idx + 1;

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d", &class,&level)) return (PARSE_ERROR_GENERIC);

		/* Point at the "info" */
		w_ptr = (weapon_style*)head->info_ptr + i;

		w_ptr->class = class;
		w_ptr->level = level;
		w_ptr->styles = 0;
	}

	/* Hack -- Process 'S' for styles */
	else if (buf[0] == 'S')
	{
		/* There better be a current w_ptr */
		if (!w_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_style(w_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Hack -- Process 'B' for benefits */
	else if (buf[0] == 'B')
	{
		/* There better be a current w_ptr */
		if (!w_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_benefit(w_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}

	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Initialize the "h_info" array, by parsing an ascii "template" file
 */
errr parse_h_info(char *buf, header *head)
{
	int i;

	char *s;

	/* Current entry */
	static hist_type *h_ptr = NULL;


	/* Process 'N' for "New/Number" */
	if (buf[0] == 'N')
	{
		int prv, nxt, prc, soc;

		/* Hack - get the index */
		i = error_idx + 1;

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		h_ptr = (hist_type*)head->info_ptr + i;

		/* Scan for the values */
		if (4 != sscanf(buf, "N:%d:%d:%d:%d",
			    &prv, &nxt, &prc, &soc)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		h_ptr->chart = prv;
		h_ptr->next = nxt;
		h_ptr->roll = prc;
		h_ptr->bonus = soc;
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current h_ptr */
		if (!h_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&h_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Grab one flag in an spell_type from a textual string
 */
static errr grab_one_cast_flag(spell_type *s_ptr, cptr what)
{
	if (grab_one_flag(&s_ptr->flags1, s_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&s_ptr->flags2, s_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&s_ptr->flags3, s_info_flags3, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown spell flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Initialize the "s_info" array, by parsing an ascii "template" file
 */
errr parse_s_info(char *buf, header *head)
{
	int i;

	char *s, *t, *u, *v, *w;


	/* Current entry */
	static spell_type *s_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		s_ptr = (spell_type*)head->info_ptr + i;

		/* Store the name */
		if (!(s_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'A' for "Appears in" (up to 10 lines) */
	else if (buf[0] == 'A')
	{
		int tval,sval,slot;
		
		bool collision = FALSE;
		bool last_slot = 0;
		int j, k;

		/* There better be a current s_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Find the next empty appears slot (if any) */
		for (i = 0; i < MAX_SPELL_APPEARS; i++) if (!s_ptr->appears[i].tval) break;

		/* Check bounds */
		if (i==MAX_SPELL_APPEARS) return (PARSE_ERROR_GENERIC);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",&tval,&sval,&slot)) return (PARSE_ERROR_GENERIC);		

#ifdef ALLOW_TEMPLATES_OUTPUT
		/* Debug: check if objects tval, sval exist, 
			except shapechanges and WIP books;
			is slow, so only enabled if generating templates */
		if (tval != 97 && tval != 91 && !lookup_kind(tval, sval))
			return (PARSE_ERROR_GENERIC);
#endif /* ALLOW_TEMPLATES_OUTPUT */

		/* Hack -- check for next free slot in book */
		if (tval != TV_RUNESTONE) for (j = 0; j < error_idx; j++)
		{
			/* Point at the "info" */
			spell_type *s2_ptr = (spell_type*)head->info_ptr + j;
			
			/* Find the next empty appears slot (if any) */
			for (k = 0; k < MAX_SPELL_APPEARS; k++)
			{
				if (!s2_ptr->appears[k].tval) break;
				
				/* Check if same book */
				if ((s2_ptr->appears[k].tval == tval) &&
					(s2_ptr->appears[k].sval == sval))
				{
					if (last_slot < s2_ptr->appears[k].slot) last_slot = s2_ptr->appears[k].slot;
					
					if (s2_ptr->appears[k].slot == slot) collision = TRUE;
				}
			}
		}
		
		/* If collision, get the next slot */
		if /*(collision) */ (tval != TV_RUNESTONE)
		{
			/*return (PARSE_ERROR_GENERIC);*/

			slot = last_slot + 1;
		}

		/* Extract the info */
		s_ptr->appears[i].tval = tval;
		s_ptr->appears[i].sval = sval;
		s_ptr->appears[i].slot = slot;
	}

	/* Process 'C' for "Cast by" (up to five lines) */
	else if (buf[0] == 'C')
	{
		int class,level,mana,fail,min;

		/* There better be a current s_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Find the next empty appears slot (if any) */
		for (i = 0; i < MAX_SPELL_CASTERS; i++) if (!s_ptr->cast[i].class) break;

		/* Check bounds */
		if (i==MAX_SPELL_CASTERS) return (PARSE_ERROR_GENERIC);

		/* Scan for the values */
		if (5 != sscanf(buf+2, "%d:%d:%d:%d:%d",
			&class,&level,&mana,&fail,&min)) return (PARSE_ERROR_GENERIC);

		/* Extract the damage dice and sides */
		s_ptr->cast[i].class = class;
		s_ptr->cast[i].level = level;
		s_ptr->cast[i].mana = mana;
		s_ptr->cast[i].fail = fail;
		s_ptr->cast[i].min = min;
	}

	/* Process 'P' for "Pre-requisites" */
	else if (buf[0] == 'P')
	{
		int p0, p1;

		/* There better be a current s_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d", &p0, &p1)) return (PARSE_ERROR_GENERIC);

		/* Extract the prerequisites */
		s_ptr->preq[0] = p0;
		s_ptr->preq[1] = p1;
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current s_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_cast_flag(s_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'B' for "Blows" (up to four lines) */
	else if (buf[0] == 'B')
	{
		int n1, n2;
		int tmp1, tmp2;

		/* There better be a current s_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Find the next empty blow slot (if any) */
		for (i = 0; i < 4; i++) if (!s_ptr->blow[i].method) break;

		/* Oops, no more slots */
		if (i == 4) return (PARSE_ERROR_GENERIC);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze the method */
		for (n1 = 0; r_info_blow_method[n1]; n1++)
		{
			if (streq(s, r_info_blow_method[n1])) break;
		}

		/* Invalid method */
		if (!r_info_blow_method[n1]) return (PARSE_ERROR_GENERIC);

		/* Analyze the second field */
		for (s = t; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze effect */
		for (n2 = 0; r_info_blow_effect[n2]; n2++)
		{
			if (streq(s, r_info_blow_effect[n2])) break;
		}

		/* Invalid effect */
		if (!r_info_blow_effect[n2]) return (PARSE_ERROR_GENERIC);

		/* Advance */
		s = t;
		
		/* Find a left bracket */
		for (; *t && (*t != '('); ++t) /* loop */;

		/* Find a slash */
		for (u = s; *u && (*u != '/'); ++u) /* loop */;
		
		/* Find a plus before a left bracket */
		for (v = s; *v && (*v != '+') && (*v != '('); ++v) /* loop */;

		/* Find a plus following t */
		for (w = t; *w && (*w != '+'); ++w) /* loop */;
		
		/* Prepare t */
		if (*t == '(')
		{
			/* Nuke plus sign and advance */
			*t++ = '\0';
		}
		else if (*u == '/')
		{
			char *t1 = t;
			
			/* Move t backwards */
			if (*v == '+')
			{
				*v++ = '\0';
				
				/* Swap around */
				t = v;
			}
			else
			{
				/* Swap around */
				t = s;
				v = t1;
				s = t1;
			}
		}
		
		/* Prepare u */
		if (*u == '/')
		{
			/* Nuke divisor and advance */
			*u++ = '\0';
		}
		
		/* Prepare v */
		if (*v == '+')
		{
			/* Nuke divisor and advance */
			*v++ = '\0';
		}
		
		/* Prepare v */
		if (*v == '(')
		{
			/* Nuke divisor and advance */
			*v++ = '\0';
		}

		/* Prepare w */
		if (*w == '+')
		{
			/* Nuke divisor and advance */
			*w++ = '\0';
		}

		/* Save the method */
		s_ptr->blow[i].method = n1;

		/* Save the effect */
		s_ptr->blow[i].effect = n2;

		/* Scan for the values */
		if (2 == sscanf(s, "%dd%d", &tmp1, &tmp2))
		{
			s_ptr->blow[i].d_dice = tmp1;
			s_ptr->blow[i].d_side = tmp2;
			s_ptr->blow[i].d_plus = atoi(v); 
		}
		else s_ptr->blow[i].d_plus = atoi(s);

		/* Scan for the values */
		if (2 == sscanf(t, "%dd%d", &tmp1, &tmp2))
		{
			s_ptr->blow[i].l_dice = tmp1;
			s_ptr->blow[i].l_side = tmp2;
			s_ptr->blow[i].l_plus = atoi(w); 
		}	
		else s_ptr->blow[i].l_plus = atoi(t);

		/* Get levels */
		s_ptr->blow[i].levels = atoi(u);		
	}

	/* Process 'S' for "Spell" */
	else if (buf[0] == 'S')
	{
		int n1;

		/* There better be a current s_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze the type */
		for (n1 = 0; s_info_types[n1]; n1++)
		{
			if (streq(s, s_info_types[n1])) break;
		}

		/* Invalid type */
		if (!s_info_types[n1]) return (PARSE_ERROR_GENERIC);

		/* Store the type */
		s_ptr->type=n1;

		/* Store the parameter */
		s_ptr->param=atoi(t); 

	}

	/* Process 'L' for "Lasts" */
	else if (buf[0] == 'L')
	{
		/* There better be a current s_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != 'd'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == 'd') *t++ = '\0';

		/* Extract the lasts dice */
		s_ptr->l_dice = atoi(s);

		/* Analyze the second field */
		for (s = t; *t && (*t != '+'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == 'd') *t++ = '\0';

		/* Extract the damage sides and plus */
		s_ptr->l_side = atoi(s);
		s_ptr->l_plus = atoi(t);

	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current s_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&(s_ptr->text), head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Grab one flag in a rune_type from a textual string
 */
static bool grab_one_rune_flag(rune_type *y_ptr, cptr what, int count)
{
	if (grab_one_offset(&y_ptr->flag[count], k_info_flags1, what) == 0)
		return (0);

	if (grab_one_offset(&y_ptr->flag[count], k_info_flags2, what) == 0)
		return (0);

	if (grab_one_offset(&y_ptr->flag[count], k_info_flags3, what) == 0)
		return (0);

	if (grab_one_offset(&y_ptr->flag[count], k_info_flags4, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown rune flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Initialize the "y_info" array, by parsing an ascii "template" file
 */
errr parse_y_info(char *buf, header *head)
{
	int i;

	char *s,*t;

	/* Current entry */
	static rune_type *y_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		y_ptr = (rune_type*)head->info_ptr + i;

		/* Store the name */
		if (!(y_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'Y' for "Rune flag" up to four lines */
	else if (buf[0] == 'Y')
	{
		/* There better be a current y_ptr */
		if (!y_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Find the next empty blow slot (if any) */
		for (i = 0; i < 4; i++) if (!y_ptr->count[i]) break;

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Get the count */
		y_ptr->count[i] = atoi(s);

		/* Parse this entry */
		if (0 != grab_one_rune_flag(y_ptr, t, i)) return (PARSE_ERROR_INVALID_FLAG);
	}


	/* Process 'B' for "Blows" (up to four lines) */
	else if (buf[0] == 'B')
	{
		int n1, n2;

		/* There better be a current y_ptr */
		if (!y_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Find the next empty blow slot (if any) */
		for (i = 0; i < 4; i++) if (!y_ptr->blow[i].method) break;

		/* Oops, no more slots */
		if (i == 4) return (PARSE_ERROR_GENERIC);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze the method */
		for (n1 = 0; r_info_blow_method[n1]; n1++)
		{
			if (streq(s, r_info_blow_method[n1])) break;
		}

		/* Invalid method */
		if (!r_info_blow_method[n1]) return (PARSE_ERROR_GENERIC);


		/* Analyze the second field */
		for (s = t; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze effect */
		for (n2 = 0; r_info_blow_effect[n2]; n2++)
		{
			if (streq(s, r_info_blow_effect[n2])) break;
		}

		/* Invalid effect */
		if (!r_info_blow_effect[n2]) return (PARSE_ERROR_GENERIC);

		/* Analyze the third field */
		for (s = t; *t && (*t != 'd'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == 'd') *t++ = '\0';

		/* Save the method */
		y_ptr->blow[i].method = n1;

		/* Save the effect */
		y_ptr->blow[i].effect = n2;

		/* Extract the damage dice */
		y_ptr->blow[i].d_dice = atoi(s);

		/* Analyze the fourth field */
		for (s = t; *t && (*t != '+'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == 'd') *t++ = '\0';

		/* Extract the damage sides and plus */
		y_ptr->blow[i].d_side = atoi(s);
		y_ptr->blow[i].d_plus = atoi(t);

	}
	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current y_ptr */
		if (!y_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&(y_ptr->text), head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab an race flag in an town_type from a textual string
 */
static errr grab_one_town_race_flag(town_type *t_ptr, cptr what)
{
	t_ptr->r_flag = 1;

	if (grab_one_offset(&t_ptr->r_flag, r_info_flags1, what) == 0)
		return (0);

	if (grab_one_offset(&t_ptr->r_flag, r_info_flags2, what) == 0)
		return (0);

	if (grab_one_offset(&t_ptr->r_flag, r_info_flags3, what) == 0)
		return (0);

	if (grab_one_offset(&t_ptr->r_flag, r_info_flags4, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown town race flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

static bool monster_not_unique(int monster_id)
{
	return monster_id && !(r_info[monster_id].name 
								  && r_info[monster_id].flags1 & RF1_UNIQUE);
}

/*
 * Initialize the "t_info" array, by parsing an ascii "template" file
 */
errr parse_t_info(char *buf, header *head)
{
	int i;

	char *s;

	/* Current entry */
	static town_type *t_ptr = NULL;

	/* Zone number */
	static int zone = 0;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		t_ptr = (town_type*)head->info_ptr + i;

		/* Take care it displays properly */
		if (strlen(s) > 19)
		  return (PARSE_ERROR_GENERIC);

		/* Store the name */
		if (!(t_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Reset the counters */
		zone = 0;


	}
	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		/* There better be a current t_ptr */
		if (!t_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		t_ptr->r_char = buf[2];
	}

	/* Process 'R' for "Race flag" (once only) */
	else if (buf[0] == 'R')
	{
		/* There better be a current t_ptr */
		if (!t_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Set to the first field */
		s=buf+2;

		/* Parse this entry */
		if (0 != grab_one_town_race_flag(t_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

	}

	/* Process 'X' for "Xtra" (one line only) */
	else if (buf[0] == 'X')
	{
		int n0, n1, n2, n3;

		/* There better be a current t_ptr */
		if (!t_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d", &n0,&n1,&n2,&n3)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		t_ptr->nearby[0]=n0;
		t_ptr->nearby[1]=n1;
		t_ptr->nearby[2]=n2;
		t_ptr->nearby[3]=n3;
	}

	/* Process 'Q' for "Quests" (one line only) */
	else if (buf[0] == 'Q')
	{
		int quest_opens, quest_monster;

		/* There better be a current t_ptr */
		if (!t_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d", &quest_opens, &quest_monster)) 
			return (PARSE_ERROR_GENERIC);

		if (monster_not_unique(quest_monster))
			return (PARSE_ERROR_GENERIC);

		/* Save the values */
		t_ptr->quest_opens = quest_opens;
		t_ptr->quest_monster = quest_monster;
	}

	/* Process 'Y' for "Lock town" (one line only) */
	else if (buf[0] == 'Y')
	{
		int town_lockup_monster, town_lockup_ifvisited;

		/* There better be a current t_ptr */
		if (!t_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d", &town_lockup_monster, &town_lockup_ifvisited)) return (PARSE_ERROR_GENERIC);

		if (monster_not_unique(town_lockup_monster))
			return (PARSE_ERROR_GENERIC);

		/* Save the values */
		t_ptr->town_lockup_monster = town_lockup_monster;
		t_ptr->town_lockup_ifvisited = town_lockup_ifvisited;
	}
	
	/* Process 'W' for "Replace guardian by" (one line only) */
	else if (buf[0] == 'W')
	{
		int replace_guardian, guardian_ifvisited;

		/* There better be a current t_ptr */
		if (!t_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d", &replace_guardian, &guardian_ifvisited)) return (PARSE_ERROR_GENERIC);

		if (monster_not_unique(replace_guardian))
			return (PARSE_ERROR_GENERIC);

		/* Save the values */
		t_ptr->replace_guardian = replace_guardian;
		t_ptr->guardian_ifvisited = guardian_ifvisited;
	}	
	
	/* Process 'Z' for "Replace by" (one line only) */
	else if (buf[0] == 'Z')
	{
		int replace_with, replace_ifvisited;

		/* There better be a current t_ptr */
		if (!t_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d", &replace_with, &replace_ifvisited)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		t_ptr->replace_with = replace_with;
		t_ptr->replace_ifvisited = replace_ifvisited;
	}
	
	/* Process 'L' for "Levels" (up to four lines) */
	else if (buf[0] == 'L')
	{
		int name,level,fill,big,small,guard,tower;

		/* There better be a current t_ptr */
		if (!t_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Oops, no more slots */
		if (zone == MAX_DUNGEON_ZONES) return (PARSE_ERROR_GENERIC);

		/* Scan for the values */
		if (6 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d",
			&level, &fill, &big, &small, &guard, &tower)) return (PARSE_ERROR_GENERIC);

		/* Find the 7th colon, the one before text (if any) */
		s = strchr(buf, ':');
		s = strchr(s+1, ':');
		s = strchr(s+1, ':');
		s = strchr(s+1, ':');
		s = strchr(s+1, ':');
		s = strchr(s+1, ':');
		s = strchr(s+1, ':');

		/* Verify that colon */
		if (!s) {
		  name = 0;
		}
		else {
		  /* Take care it displays properly */
		  if (strlen(s+1) > 25)
		    return (PARSE_ERROR_GENERIC);
		  /* Store the level name */
		  if (!(name = add_name(head, s+1)))
		    return (PARSE_ERROR_OUT_OF_MEMORY);
		}

		if (monster_not_unique(guard))
			return (PARSE_ERROR_GENERIC);

		/* Save the values */
		t_ptr->zone[zone].name = name;
		t_ptr->zone[zone].level = level;
		t_ptr->zone[zone].fill = fill;
		t_ptr->zone[zone].big = big;
		t_ptr->zone[zone].small = small;
		t_ptr->zone[zone].guard = guard;
		t_ptr->zone[zone].tower = tower;

		/* Find the next empty zone slot (if any) */
		zone++;
	}

	/* Process 'S' for "Stores" */
	else if (buf[0] == 'S')
	{
		int store1,store2,store3,store4,store5,store6,store7,store8;

		/* There better be a current t_ptr */
		if (!t_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (8 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d",
			&store1, &store2, &store3, &store4, &store5, &store6, &store7, &store8)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		t_ptr->store[0] = store1;
		t_ptr->store[1] = store2;
		t_ptr->store[2] = store3;
		t_ptr->store[3] = store4;
		t_ptr->store[4] = store5;
		t_ptr->store[5] = store6;
		t_ptr->store[6] = store7;
		t_ptr->store[7] = store8;

	}
	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current t_ptr */
		if (!t_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&t_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Initialize the "u_info" array, by parsing an ascii "template" file
 */
errr parse_u_info(char *buf, header *head)
{
	int i;

	char *s;

	/* Current entry */
	static store_type *u_ptr = NULL;

	static int cur_t = 0;
	static int cur_w = 0;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		u_ptr = (store_type*)head->info_ptr + i;

		/* Store the name */
		if (!(u_ptr->name = add_name(head, s))) return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Save the index into the store structure */
		u_ptr->index = i;
		
		/* Reset the store */
		cur_t = 0;
		cur_w = 0;
	}

	/* Process 'X' for "Extra" */
	else if (buf[0] == 'X')
	{
		int base;

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d", &base)) return (PARSE_ERROR_GENERIC);

		u_ptr->base = base;
		
		/* Switch on the store */
		switch (u_ptr->base)
		{
			/* General Store */
			case STORE_GENERAL:
			{
				u_ptr->tvals_will_buy[0] = TV_FOOD;
				u_ptr->tvals_will_buy[1] = TV_LITE;
				u_ptr->tvals_will_buy[2] = TV_FLASK;
				u_ptr->tvals_will_buy[3] = TV_SPIKE;
				u_ptr->tvals_will_buy[4] = TV_SHOT;
				u_ptr->tvals_will_buy[5] = TV_ARROW;
				u_ptr->tvals_will_buy[6] = TV_BOLT;
				u_ptr->tvals_will_buy[7] = TV_DIGGING;
				u_ptr->tvals_will_buy[8] = TV_CLOAK;
				u_ptr->tvals_will_buy[9] = TV_INSTRUMENT;
				u_ptr->tvals_will_buy[10] = TV_MAP;
				u_ptr->tvals_will_buy[11] = TV_BAG;
				u_ptr->tvals_will_buy[12] = TV_ROPE;
				break;
			}

			/* Armoury */
			case STORE_ARMOR:
			{
				u_ptr->tvals_will_buy[0] = TV_BOOTS;
				u_ptr->tvals_will_buy[1] = TV_GLOVES;
				u_ptr->tvals_will_buy[2] = TV_CROWN;
				u_ptr->tvals_will_buy[3] = TV_HELM;
				u_ptr->tvals_will_buy[4] = TV_SHIELD;
				u_ptr->tvals_will_buy[5] = TV_CLOAK;
				u_ptr->tvals_will_buy[6] = TV_SOFT_ARMOR;
				u_ptr->tvals_will_buy[7] = TV_HARD_ARMOR;
				u_ptr->tvals_will_buy[8] = TV_DRAG_ARMOR;
				break;
			}

			/* Weapon Shop */
			case STORE_WEAPON:
			{
				u_ptr->tvals_will_buy[0] = TV_SHOT;
				u_ptr->tvals_will_buy[1] = TV_BOLT;
				u_ptr->tvals_will_buy[2] = TV_ARROW;
				u_ptr->tvals_will_buy[3] = TV_BOW;
				u_ptr->tvals_will_buy[4] = TV_DIGGING;
				u_ptr->tvals_will_buy[5] = TV_HAFTED;
				u_ptr->tvals_will_buy[6] = TV_POLEARM;
				u_ptr->tvals_will_buy[7] = TV_SWORD;
				break;
			}

			/* Temple */
			case STORE_TEMPLE:
			{
				u_ptr->tvals_will_buy[0] = TV_PRAYER_BOOK;
				u_ptr->tvals_will_buy[1] = TV_SCROLL;
				u_ptr->tvals_will_buy[2] = TV_POTION;
				u_ptr->tvals_will_buy[3] = TV_HAFTED;
				u_ptr->tvals_will_buy[4] = TV_STATUE;
				u_ptr->tvals_will_buy[5] = TV_SHOT;				
				/*u_ptr->tvals_will_buy[6] = TV_POLEARM; Was blessed only */
				/*u_ptr->tvals_will_buy[7] = TV_SWORD; Was blessed only */
				break;
			}

			/* Alchemist */
			case STORE_ALCHEMY:
			{
				u_ptr->tvals_will_buy[0] = TV_FOOD; /* Was mushrooms only */
				u_ptr->tvals_will_buy[1] = TV_SCROLL;
				u_ptr->tvals_will_buy[2] = TV_POTION;
				u_ptr->tvals_will_buy[3] = TV_RUNESTONE;
				break;
			}

			/* Magic Shop */
			case STORE_MAGIC:
			{
				u_ptr->tvals_will_buy[0] = TV_MAGIC_BOOK;
				u_ptr->tvals_will_buy[1] = TV_AMULET;
				u_ptr->tvals_will_buy[2] = TV_RING;
				u_ptr->tvals_will_buy[3] = TV_STAFF;
				u_ptr->tvals_will_buy[4] = TV_WAND;
				u_ptr->tvals_will_buy[5] = TV_ROD;
				u_ptr->tvals_will_buy[6] = TV_SCROLL;
				u_ptr->tvals_will_buy[7] = TV_POTION;
				u_ptr->tvals_will_buy[8] = TV_RUNESTONE;
				break;
			}
		}
	}

	/* Process 'O' for "Offered" (up to thirty two lines) */
	else if (buf[0] == 'O')
	{
		int tval, sval, count;

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d", &tval, &sval, &count)) return (PARSE_ERROR_GENERIC);

		/* only thirty two O: lines allowed */
		if (cur_t >= STORE_CHOICES) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		u_ptr->tval[cur_t] = (byte)tval;
		u_ptr->sval[cur_t] = (byte)sval;
		u_ptr->count[cur_t] = (byte)count;

		/* increase counter for 'possible tval' index */
		cur_t++;
	}
	
	/* Process 'B' for "Buy" (up to sixteen lines) */
	else if (buf[0] == 'B')
	{
		int tval;

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d", &tval)) return (PARSE_ERROR_GENERIC);

		/* only thirty two O: lines allowed */
		if (cur_w >= STORE_CHOICES) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		u_ptr->tvals_will_buy[cur_w] = (byte)tval;

		/* increase counter for 'possible tval' index */
		cur_w++;
	}
	
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);}




/*
 * Initialize the "b_info" array, by parsing an ascii "template" file
 */
errr parse_b_info(char *buf, header *head)
{
	int i, j;

	char *s, *t;

	/* Current entry */
	static owner_type *ot_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the subindex */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the subindex */
		*s++ = '\0';

		/* Get the index */
		i = atoi(buf+2);

		/* Find the colon before the name */
		t = strchr(s, ':');

		/* Verify that colon */
		if (!t) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*t++ = '\0';

		/* Paranoia -- require a name */
		if (!*t) return (PARSE_ERROR_GENERIC);

		/* Get the subindex */
		j = atoi(s);

		/* Verify information */
		if (j >= z_info->b_max) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Get the *real* index */
		i = (i * z_info->b_max) + j;

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		ot_ptr = (owner_type*)head->info_ptr + i;

		/* Store the name */
		if (!(ot_ptr->owner_name = add_name(head, t)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		int idx, gld, max, min, hgl, tol;

		/* There better be a current ot_ptr */
		if (!ot_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (6 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d",
			    &idx, &gld, &max, &min, &hgl, &tol)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		ot_ptr->owner_race = idx;
		ot_ptr->max_cost = gld;
		ot_ptr->max_inflate = max;
		ot_ptr->min_inflate = min;
		ot_ptr->haggle_per = hgl;
		ot_ptr->insult_max = tol;
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}




/*
 * Initialize the "g_info" array, by parsing an ascii "template" file
 */
errr parse_g_info(char *buf, header *head)
{
	int i, j;

	char *s;

	/* Current entry */
	static byte *g_ptr;


	/* Process 'A' for "Adjustments" */
	if (buf[0] == 'A')
	{
		int adj;

		/* Start the string */
		s = buf+1;

		/* Initialize the counter to max races */
		j = z_info->g_max;

		/* Repeat */
		while (j-- > 0)
		{
			/* Hack - get the index */
			i = error_idx + 1;

			/* Verify information */
			if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

			/* Verify information */
			if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			g_ptr = (byte*)head->info_ptr + i;

			/* Find the colon before the subindex */
			s = strchr(s, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the subindex */
			*s++ = '\0';

			/* Get the value */
			adj = atoi(s);

			/* Save the value */
			*g_ptr = adj;
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Grab an action in an quest_type from a textual string
 */
static errr grab_one_quest_action(quest_event *qe_ptr, cptr what)
{
	if (grab_one_offset(&qe_ptr->action, f_info_flags1, what) == 0)
		return (0);

	if (grab_one_offset(&qe_ptr->action, f_info_flags2, what) == 0)
		return (0);

	if (grab_one_offset(&qe_ptr->action, f_info_flags3, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown quest action '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Grab one flag in an quest_event_info_flags from a textual string
 */
static errr grab_one_quest_flag(quest_event *qe_ptr, cptr what)
{
	if (grab_one_flag(&qe_ptr->flags, quest_event_info_flags, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown quest event flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Grab one flag in an room_type from a textual string
 */
static errr grab_one_quest_room_flag(quest_event *qe_ptr, cptr what)
{
	if (grab_one_flag(&qe_ptr->flags, d_info_sflags, what) == 0)
		return (0);

	/* Oops */
	msg_format("Unknown quest event flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Initialize the "q_info" array, by parsing an ascii "template" file
 */
errr parse_q_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static quest_type *q_ptr = NULL;

	/* Current entry */
	static quest_event *qe_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		q_ptr = (quest_type*)head->info_ptr + i;

		/* Point at the first event */
		qe_ptr = &(q_ptr->event[0]);

		/* Store the name */
		if (!(q_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Hack -- quest stage */
		q_ptr->stage = 100;
	}

	/* Process 'S' for "Stage" (up to MAX_QUEST_EVENT lines) */
	else if (buf[0] == 'S')
	{
		int stage;

		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d",
				&stage)) return (PARSE_ERROR_GENERIC);

		/* Paranoia */
		if (stage < 0) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Paranoia */
		if (stage >= MAX_QUEST_EVENTS) return (PARSE_ERROR_TOO_MANY_ENTRIES);
		
		/* Point at the first event */
		qe_ptr = &q_ptr->event[stage];

		/* Hack -- set first quest stage */
		if (q_ptr->stage == 100) q_ptr->stage = stage;

		/* Hack -- set default number */
		qe_ptr->number = 1;
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_quest_flag(qe_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'T' for "Travel to" (one line per stage) */
	else if (buf[0] == 'T')
	{
		int dungeon, level, store;
		
		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
				&dungeon, &level, &store)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		qe_ptr->dungeon = dungeon;
		qe_ptr->level = level;
		qe_ptr->store = store;
	}

	/* Process 'A' for "Artifact" (one line per stage) */
	else if (buf[0] == 'A')
	{
		int artifact;

		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d",
			    &artifact)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		qe_ptr->artifact = artifact;
	}
	
	/* Process 'Q' for "Quest" (one line per stage) */
	else if (buf[0] == 'Q')
	{
		int quest;

		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d",
			    &quest)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		qe_ptr->quest = quest;
	}

	/* Process 'Z' for "Number" (one line per stage) */
	else if (buf[0] == 'Z')
	{
		int number;

		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d",
			    &number)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		qe_ptr->number = number;
	}

	/* Process 'K' for "Kinds" (one line per stage) */
	else if (buf[0] == 'K')
	{
		int kind;
		
		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d",
				&kind)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		qe_ptr->kind = kind;
	}

	/* Process 'E' for "Ego item" (one line per stage) */
	else if (buf[0] == 'E')
	{
		int ego_item_type;
		
		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d",
				&ego_item_type)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		qe_ptr->ego_item_type = ego_item_type;
	}

	/* Process 'R' for "Races" (one line per stage) */
	else if (buf[0] == 'R')
	{
		int race;

		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d",
				&race)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		qe_ptr->race = race;
	}

	/* Process 'O' for "Rooms" (one line per stage) */
	else if (buf[0] == 'O')
	{
		int room_type_a, room_type_b;

		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Parse this entry */
		if (0 != grab_one_quest_room_flag(qe_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

		/* Scan for the values */
		if (2 != sscanf(t, "%d:%d",
				&room_type_a, &room_type_b)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		qe_ptr->room_type_a = room_type_a;
		qe_ptr->room_type_b = room_type_b;
	}

	/* Process 'X' for "Features" (one line per stage) */
	else if (buf[0] == 'X')
	{
		int feat;

		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Parse this entry */
		if (0 != grab_one_quest_action(qe_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

		/* Scan for the values */
		if (1 != sscanf(t, "%d",
				&feat)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		qe_ptr->feat = feat;

	}

	/* Process 'W' for "Worth" (one line per stage) */
	else if (buf[0] == 'W')
	{
		int experience, power, gold;
		
		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
				&experience, &power, &gold)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		qe_ptr->experience = experience;
		qe_ptr->power = power;
		qe_ptr->gold = gold;
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
		if (!add_text(&q_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}


/*
 * Initialise the info
 */
errr eval_info(eval_info_power_func eval_info_process, header *head)
{
	int err;

	/* Process the info */
	err = (*eval_info_process)(head);

	return(err);
}



static long eval_blow_effect(int effect, int atk_dam, int rlev)
{
	switch (effect)
	{
		/*other bad effects - minor*/
		case GF_EAT_GOLD:
		case GF_EAT_ITEM:
		case GF_EAT_FOOD:
		case GF_EAT_LITE:
		case GF_LOSE_CHR:
		case GF_WIND:
		{
			atk_dam += 5;
			break;
		}
		/*other bad effects - poison / disease */
		case GF_DISEASE:
		case GF_POIS:
		case GF_ICE:
		case GF_SHARD:
		{
			atk_dam *= 5;
			atk_dam /= 4;
			atk_dam += 5;
			break;
		}
		/*other bad effects - elements / sustains*/
		case GF_TERRIFY:
		case GF_ELEC:
		case GF_COLD:
		case GF_FIRE:
		case GF_HUNGER:
		case GF_LOSE_MANA:
		case GF_LITE:
		case GF_DARK:
		case GF_SLOW:
		case GF_DISPEL:
		{
			atk_dam += 10;
			break;
		}
		/*other bad effects - elements / major*/
		case GF_PLASMA:
		case GF_ACID:
		case GF_WATER:
		case GF_SOUND:
		case GF_NEXUS:
		case GF_BLIND:
		case GF_LAVA:
		case GF_CONFUSION:
		case GF_LOSE_STR:
		case GF_LOSE_INT:
		case GF_LOSE_WIS:
		case GF_LOSE_DEX:
		case GF_HALLU:
		case GF_PETRIFY:
		{
			atk_dam += 20;
			break;
		}
		/*other bad effects - major*/
		case GF_GRAVITY:
		case GF_INERTIA:
		case GF_FORCE:
		case GF_UN_BONUS:
		case GF_UN_POWER:
		case GF_LOSE_CON:
		{
			atk_dam += 30;
			break;
		}
		/*other bad effects - major*/
		case GF_PARALYZE:
		case GF_LOSE_ALL:
		{
			atk_dam += 40;
			break;
		}
		/*other bad effects - major*/
		case GF_DISENCHANT:
		{
			atk_dam += 50;
			break;
		}
		/* Experience draining attacks */
		case GF_EXP_10:
		case GF_EXP_20:
		{
			atk_dam += 1000 / (rlev + 1);
			break;
		}
		case GF_NETHER:
		case GF_CHAOS:
		case GF_TIME:
		case GF_EXP_40:
		case GF_EXP_80:
		{
			atk_dam += 2000 / (rlev + 1);
			break;
		}
		/*Earthquakes*/
		case GF_SHATTER:
		{
			atk_dam += 300;
			break;
		}
		/* No damage normally */
		case GF_LITE_WEAK:
		case GF_DARK_WEAK:
		case GF_WATER_WEAK:
		case GF_SALT_WATER:
		case GF_BLIND_WEAK:
		{
			atk_dam = 5;
		}
		/*nothing special*/
		default: break;
	}

	return (atk_dam);
}


static int threat_mod(int threat, monster_race *r_ptr, bool ranged)
{
	/*
	 * Adjust threat for friends.
	 */
	if (ranged)
	{
		if (r_ptr->flags1 & (RF1_FRIENDS))
			threat *= 3;
		else if (r_ptr->flags1 & (RF1_FRIEND))
			threat = threat * 3 / 2;
	}
	else
	{
		if (r_ptr->flags1 & (RF1_FRIENDS))
		{
			threat *= 2;
		}
	}

	/*
	 * Add threat from multiplication.
	 */
	if (r_ptr->flags2 & (RF2_MULTIPLY))
		threat += threat * extract_energy[r_ptr->speed + (r_ptr->flags6 & RF6_HASTE ? 5 : 0)] / 5;
	
	return (threat);
}


/*
 * Go through the attack types for this monster.
 * We look for the maximum possible maximum damage that this
 * monster can inflict in 10 game turns.
 *
 * We try to scale this based on assumed resists,
 * chance of casting spells and of spells failing,
 * chance of hitting in melee, and particularly speed.
 */

static long eval_max_dam(monster_race *r_ptr)
{
	int i, x;
	int dam = 1;
	int hp;
	int melee_dam, atk_dam, spell_dam, spell_dam_real;
	byte rlev;
	u32b flag, breath_mask, attack_mask, innate_mask;
	u32b flag_counter;

	bool shriek = FALSE;

	/*clear the counters*/
	melee_dam = atk_dam = spell_dam = spell_dam_real = 0;

	/* Evaluate average HP for this monster */
	if (r_ptr->flags1 & (RF1_FORCE_MAXHP)) hp = r_ptr->hdice * r_ptr->hside;
	else hp = r_ptr->hdice * (r_ptr->hside + 1) / 2;

	/* Extract the monster level, force 1 for town monsters */
	rlev = r_ptr->level ? r_ptr->level : 1;

	for (x = 0; x < 4; x++)
	{
		/*Get the flags 4 monster flags and masks*/
		switch (x)
		{
			case 0:
			{
		 		flag = r_ptr->flags4;
				attack_mask = RF4_ATTACK_MASK;
				breath_mask = RF4_BREATH_MASK;
				innate_mask = RF4_INNATE_MASK;
				break;
			}
			case 1:
			{
		 		flag = r_ptr->flags5;
				attack_mask = RF5_ATTACK_MASK;
				breath_mask = RF5_BREATH_MASK;
				innate_mask = RF5_INNATE_MASK;
				break;
			}
			case 2:
			{
		 		flag = r_ptr->flags6;
				attack_mask = RF6_ATTACK_MASK;
				breath_mask = RF6_BREATH_MASK;
				innate_mask = RF6_INNATE_MASK;
				break;
			}
			case 3:
			default:
			{
		 		flag = r_ptr->flags7;
				attack_mask = RF7_ATTACK_MASK;
				breath_mask = RF7_BREATH_MASK;
				innate_mask = RF7_INNATE_MASK;
				break;
			}
		}

		/*no spells here, continue*/
		if (!flag) continue;

		flag_counter = 0x00000001;

		/* using 32 assumes a u32b flag size*/
		for (i = 0; i < 32; i++)
		{
			int this_dam;
			bool real_dam = FALSE;

			this_dam = 0;

			/* We count ranged blows later */
			if (!(x) && ((flag <= RF4_BLOW_4) || (flag == RF4_EXPLODE) || (flag == RF4_AURA))) continue;

			/* First make sure monster has the flag*/
			if (flag & flag_counter)
			{
				/*Is it a breath? Should only be flag 4*/
				if (breath_mask & flag_counter)
				{
					int which_gf = 0;
					int mult = 1;
					int div_by = 1;

					/*hack - all breaths are in flag 4*/

					if (flag_counter == RF4_BRTH_ACID) 		which_gf = GF_ACID;
					else if (flag_counter == RF4_BRTH_ELEC) which_gf = GF_ELEC;
					else if (flag_counter == RF4_BRTH_FIRE) which_gf = GF_FIRE;
					else if (flag_counter == RF4_BRTH_COLD) which_gf = GF_COLD;
					else if (flag_counter == RF4_BRTH_POIS)
					{
						which_gf = GF_POIS;
						mult = 8;
						div_by = 7;
					}
					else if (flag_counter == RF4_BRTH_PLAS)
					{
						which_gf = GF_PLASMA;
						mult = 5;
						div_by = 4;
					}
					else if (flag_counter == RF4_BRTH_LITE)
					{
						which_gf = GF_LITE;
						mult = 5;
						div_by = 4;
					}
					else if (flag_counter == RF4_BRTH_DARK)
					{
						which_gf = GF_DARK;
						mult = 5;
						div_by = 4;
					}
					else if (flag_counter == RF4_BRTH_CONFU)
					{
						which_gf = GF_CONFUSION;
						mult = 4;
						div_by = 3;
					}
					else if (flag_counter == RF4_BRTH_SOUND)
					{
						which_gf = GF_SOUND;
						mult = 6;
						div_by = 5;
					}
					else if (flag_counter == RF4_BRTH_SHARD)
					{
						which_gf = GF_SHARD;
						mult = 8;
						div_by = 7;
					}
					else if (flag_counter == RF4_BRTH_INERT)
					{
						which_gf = GF_INERTIA;
						mult = 3;
						div_by = 2;
					}
					else if (flag_counter == RF4_BRTH_WIND)
					{
						which_gf = GF_WIND;
						mult = 6;
						div_by = 5;
					}
					else if (flag_counter == RF4_BRTH_GRAV)
					{
						which_gf = GF_GRAVITY;
						mult = 3;
						div_by = 2;
					}
					else if (flag_counter == RF4_BRTH_FORCE)
					{
						which_gf = GF_FORCE;
						mult = 6;
						div_by = 5;
					}
					else if (flag_counter == RF4_BRTH_NEXUS)
					{
						which_gf = GF_NEXUS;
						mult = 5;
						div_by = 4;
					}
					else if (flag_counter == RF4_BRTH_NETHR)
					{
						which_gf = GF_NETHER;
						mult = 5;
						div_by = 4;
					}
					else if (flag_counter == RF4_BRTH_CHAOS)
					{
						which_gf = GF_CHAOS;
						mult = 4;
						div_by = 3;
					}
					else if (flag_counter == RF4_BRTH_DISEN)
					{
						which_gf = GF_DISENCHANT;
						mult = 4;
						div_by = 3;
					}
					else if (flag_counter == RF4_BRTH_TIME)
					{
						which_gf = GF_TIME;
						mult = 3;
						div_by = 2;
					}
					else if (flag_counter == RF4_BRTH_MANA) which_gf = GF_MANA;
					else if (flag_counter == RF4_BRTH_HOLY)
					{
						which_gf = GF_HOLY_ORB;
						div_by = 2;
					}
					else if (flag_counter == RF4_BRTH_FEAR)
					{
						which_gf = GF_TERRIFY;
						mult = 5;
						div_by = 4;
					}
					else if (flag_counter == RF4_BRTH_DISEA)
					{
						which_gf = GF_DISEASE;
						mult = 5;
						div_by = 4;
					}

					if (which_gf)
					{
						this_dam = get_breath_dam(hp, which_gf,
									(r_ptr->flags2 & (RF2_POWERFUL) ? TRUE : FALSE));

						/* handle elemental breaths*/
						switch (which_gf)
						{
							case GF_ACID:
							case GF_FIRE:
							case GF_COLD:
							case GF_ELEC:
							case GF_POIS:
							{
								/* Scale for level */
								mult *= 20;
								div_by *= rlev + 20;

								break;
							}

						}

						/* Get damage */
						this_dam = eval_blow_effect(which_gf, (this_dam * mult) / div_by, rlev);

						/* Slight bonus for being powerful */
						if (r_ptr->flags2 & (RF2_POWERFUL)) this_dam = this_dam * 8 / 7;
					}
					
					real_dam = TRUE;
				}

				/*Is it an arrow, bolt, beam, or ball?*/
				else if (attack_mask & flag_counter)
				{
					switch (x)
					{
						case 0:
						{
							this_dam = eval_blow_effect(spell_desire_RF4[i][6], r_ptr->spell_power * spell_info_RF4[i][COL_SPELL_DAM_MULT], rlev);
							this_dam /=  MAX(1, spell_info_RF4[i][COL_SPELL_DAM_DIV]);
							break;
						}
						case 1:
						{
							this_dam = eval_blow_effect(spell_desire_RF5[i][6], r_ptr->spell_power * spell_info_RF5[i][COL_SPELL_DAM_MULT], rlev);
							this_dam /=  MAX(1, spell_info_RF5[i][COL_SPELL_DAM_DIV]);
							break;
						}
						case 2:
						{
							this_dam = eval_blow_effect(spell_desire_RF6[i][6], r_ptr->spell_power * spell_info_RF6[i][COL_SPELL_DAM_MULT], rlev);
							this_dam /=  MAX(1, spell_info_RF6[i][COL_SPELL_DAM_DIV]);
							break;
						}
						case 3:
						{
							this_dam = eval_blow_effect(spell_desire_RF7[i][6], r_ptr->spell_power * spell_info_RF7[i][COL_SPELL_DAM_MULT], rlev);
							this_dam /=  MAX(1, spell_info_RF7[i][COL_SPELL_DAM_DIV]);
							break;
						}
					}
					
					real_dam = TRUE;
					
					/* Hack - handle Wound spell differently */
					if ((x == 2) && (i == 20))
					{
						if (r_ptr->spell_power > 75) this_dam = 225 + r_ptr->spell_power - 75;
					}
				}

				else switch (x)
				{
					/*Misc flag4 flags*/
					case 0:
					{
						break;
					}

					case 1:
					{
						/*Right now all flag5 are attack mask spells*/
						break;
					}

					case 2:
					{
						/*Misc flag6 flags*/
						if (flag_counter == RF6_ADD_MANA) this_dam = 0;
						else if (flag_counter == RF6_BLINK) this_dam = rlev / 3;
						else if (flag_counter == RF6_TELE_SELF_TO) this_dam = rlev * 2;
						else if (flag_counter == RF6_TELE_TO) this_dam = rlev;
						else if (flag_counter == RF6_DARKNESS) this_dam = rlev;
						else if (flag_counter == RF6_TRAPS) this_dam = rlev;
						else if (flag_counter == RF6_FORGET) this_dam = rlev / 3;
						else if (flag_counter == RF6_DISPEL) this_dam = rlev;
						else if (flag_counter == RF6_ILLUSION) this_dam = rlev;
						else if (flag_counter == RF6_DRAIN_MANA) this_dam = 40 + r_ptr->spell_power; /* Especially dangerous at low levels */
						else if (flag_counter == RF6_HUNGER) this_dam = rlev;
						else if (flag_counter == RF6_PROBE) this_dam = rlev / 3;
						else if (flag_counter == RF6_SCARE) this_dam = rlev;
						else if (flag_counter == RF6_BLIND) this_dam = rlev;
						else if (flag_counter == RF6_CONF) this_dam = rlev;
						else if (flag_counter == RF6_SLOW) this_dam = rlev;
						else if (flag_counter == RF6_HOLD) this_dam = 25;
						else break;

						if (r_ptr->flags2 & (RF2_POWERFUL)) this_dam = this_dam * 3 / 2;
						break;
					}
					/*All flag7 flags*/
					case 3:
					{
						bool escalate = TRUE;
						
						/*Right now all flag7 are summon spells*/
						/* All summons are assigned arbitrary values according to their levels*/
						if 		(flag_counter == RF7_S_KIN) 	this_dam = rlev * 2 + 20;
						else if (flag_counter == RF7_R_KIN){ 	this_dam = rlev / 2 + 10; escalate = FALSE; }
						else if (flag_counter == RF7_A_DEAD){ 	this_dam = rlev / 5 + 10; escalate = FALSE; }
						else if (flag_counter == RF7_S_MONSTER)	this_dam = rlev * 2 / 5 + 10;
						else if (flag_counter == RF7_S_MONSTERS)this_dam = rlev * 4 / 5 + 20;
						else if (flag_counter == RF7_R_MONSTER){	this_dam = rlev * 1 / 5 + 10; escalate = FALSE; }
						else if (flag_counter == RF7_R_MONSTERS){	this_dam = rlev * 2 / 5 + 20; escalate = FALSE; }
						else if (flag_counter == RF7_S_PLANT)		this_dam = rlev / 5 + 5;
						else if (flag_counter == RF7_S_INSECT)		this_dam = rlev / 5 + 10;
						else if (flag_counter == RF7_S_ANIMAL)
						{
							/* XXX Feathers, fur or skin? */
							this_dam = rlev / 2 + 10;  /* Includes hydras */
							/* this_dam = rlev / 2;   Fur includes canines / cats */
							/* this_dam = rlev / 2;   Feathers includes birds / hybrids */
						}
						else if (flag_counter == RF7_S_SPIDER)	this_dam = rlev / 5 + 10;
						else if (flag_counter == RF7_S_HOUND)	this_dam = rlev + 15;
						else if (flag_counter == RF7_S_CLASS)	this_dam = rlev / 2 + 20;
						else if (flag_counter == RF7_S_RACE)	this_dam = rlev / 2 + 20;
						else if (flag_counter == RF7_S_GROUP)	this_dam = rlev / 2 + 20;
						else if (flag_counter == RF7_S_FRIEND)	this_dam = rlev / 3 + 10;
						else if (flag_counter == RF7_S_FRIENDS)	this_dam = rlev * 3 / 4 + 20;
						else if (flag_counter == RF7_S_DRAGON)	this_dam = rlev * 3 / 2 + 20;
						else if (flag_counter == RF7_S_HI_DRAGON) this_dam = rlev * 4 + 30;
						else if (flag_counter == RF7_A_ELEMENT){ this_dam = rlev / 3 + 10; escalate = FALSE; }
						else if (flag_counter == RF7_A_OBJECT){ 	this_dam = rlev / 5 + 5; escalate = FALSE; }
						else if (flag_counter == RF7_S_DEMON)	this_dam = rlev * 3 / 2 + 20;
						else if (flag_counter == RF7_S_HI_DEMON)this_dam = rlev * 3 + 30;
						else if (flag_counter == RF7_S_UNDEAD)	this_dam = rlev * 3 / 2 + 20;
						else if (flag_counter == RF7_S_HI_UNDEAD)this_dam = rlev * 4 + 30;
						else if (flag_counter == RF7_S_WRAITH)	this_dam = rlev * 9 / 2 + 30;
						else if (flag_counter == RF7_S_UNIQUE)	this_dam = rlev * 3 + 40;
						else if (flag_counter == RF7_S_HI_UNIQUE)	this_dam = rlev * 5 + 50;

						/* Fast summoners are more threatening */
						if (escalate) this_dam += (this_dam * extract_energy[r_ptr->speed + (r_ptr->flags6 & RF6_HASTE ? 5 : 0)]) / 10;

						break;
					}
				}

			}

			/* Hack - record most damaging attack */
			if (threat_mod(this_dam, r_ptr, TRUE) > r_ptr->highest_threat)
			{
				r_ptr->highest_threat = threat_mod(this_dam, r_ptr, TRUE);
				r_ptr->best_threat = 96 + x * 32 + i;
			}

			/* Hack - Apply over 10 rounds */
			this_dam *= 10;

			/* Scale for spell failure chance */
			if (!(r_ptr->flags2 & RF2_STUPID) && (x > 0)) this_dam = this_dam * MIN(75 + (rlev + 3) / 4, 100) / 100;

			/* Scale for frequency and availability of mana / ammo */
			if (this_dam)
			{
				int freq;
				int need_mana = 0;
				int has_ammo = 0;

				/* Get frequency */
				if ((x == 0) && (flag_counter & innate_mask)) freq = r_ptr->freq_innate;
				else freq = r_ptr->freq_spell;
#if 0
				/* Paranoia */
				if (!freq)
				{
					r_ptr->highest_threat = 30000;
					r_ptr->best_threat = 96 + x * 32 + i;
				}
#endif
				/* Get mana required */
				if (x == 0) need_mana = spell_info_RF4[i][0];
				else if (x == 1) need_mana = spell_info_RF5[i][0];
				else if (x == 2) need_mana = spell_info_RF6[i][0];
				else if (x == 3) need_mana = spell_info_RF7[i][0];
#if 0
				/* Paranoia */
				if ((need_mana) && !(r_ptr->mana))
				{
					r_ptr->highest_threat = 30000;
					r_ptr->best_threat = 96 + x * 32 + i;
				}
#endif
				/* Adjust frequency for ammo */
				freq = MIN(freq, has_ammo * 5);

				/* Adjust frequency for mana -
				   -- casters that can add mana and need to do so */
				if ((r_ptr->flags6 & (RF6_ADD_MANA)) 
					&& need_mana 
					&& (freq > r_ptr->mana * 10 / need_mana))
				{
					freq = (freq + r_ptr->mana * 10 / need_mana) / 2;
				}

				/* Adjust frequency for mana */
				else if (need_mana)
				{
					freq = MIN(freq, r_ptr->mana * 10 / need_mana);
				}

				/* Hack -- always get 1 shot */
				if (freq < 10) freq = 10;
				
				/* Paranoia -- maximum of 10 shots */
				if (freq > 100) freq = 100;

				/* Adjust for frequency */
				this_dam = this_dam * freq / 100;
			}

			/* Better spell? */
			if (this_dam > spell_dam)
			{
				spell_dam = this_dam;
				if (this_dam > 2 * spell_dam_real) 
					r_ptr->best_spell = 96 + x * 32 + i;
			}

			/* Better spell? */
			if ((real_dam) && (this_dam > spell_dam_real))
			{
				r_ptr->best_spell = 96 + x * 32 + i;
				spell_dam_real = this_dam;
			}

			/*shift one bit*/
			flag_counter = flag_counter << 1;
		}
	}

	/* Check attacks */
	if (TRUE)
	{
		for (i = 0; i < 4; i++)
		{
			/* Extract the attack infomation */
			int effect = r_ptr->blow[i].effect;
			int method = r_ptr->blow[i].method;
			int d_dice = r_ptr->blow[i].d_dice;
			int d_side = r_ptr->blow[i].d_side;

			/* Hack -- no more attacks */
			if (!method) continue;

			/* Multiply average damage by 2 to simplify calculations */
			atk_dam = eval_blow_effect(effect, 
									   d_dice * (d_side + 1), 
									   rlev);

			switch (method)
			{
				/*stun definitely most dangerous*/
				case RBM_PUNCH:
				case RBM_KICK:
				case RBM_BUTT:
				case RBM_CRUSH:
				{
					atk_dam *= 4;
					atk_dam /= 3;
					break;
				}
				/*cut*/
				case RBM_CLAW:
				case RBM_BITE:
				case RBM_PECK:
				{
					atk_dam *= 7;
					atk_dam /= 5;
					break;
				}
				/* shriek */
				case RBM_SHRIEK:
				{
					shriek = TRUE;
					break;
				}
				default: 
				{
					if ((effect == GF_WOUND) || (effect == GF_BATTER))
					{
						atk_dam *= 5;
						atk_dam /= 4;
					}
					break;
				}
			}

			/* Normal melee or ranged attack */
			if ((method <= RBM_MAX_NORMAL) && !(r_ptr->flags1 & (RF1_NEVER_BLOW)))
			{
				/* Keep a running total */
				melee_dam += atk_dam;
			}

			/* Ranged attacks can also apply spell dam */
			if (method >= RBM_MIN_RANGED)
			{
				int range = MAX_SIGHT, mana = 0, has_ammo = 0, freq;
				bool must_hit = FALSE;

				/* handle elemental breaths*/
				switch (effect)
				{
					case GF_ACID:
					case GF_FIRE:
					case GF_COLD:
					case GF_ELEC:
					case GF_POIS:
					{
						/* Scale for level */
						atk_dam *= 20;
						atk_dam /= rlev + 20;
						break;
					}
				}

				/* Hack - record most damaging attack */
				if (threat_mod(atk_dam, r_ptr, TRUE) > r_ptr->highest_threat)
				{
					r_ptr->highest_threat = threat_mod(atk_dam, r_ptr, TRUE);
					r_ptr->best_threat = 96 + i;
				}

				/* Some ranged blows can miss */
				switch(method)
				{
					case RBM_SPIT:	mana = 0; range = 3; must_hit = TRUE; break;
					case RBM_GAZE:	mana = 3; range = MIN(MAX_SIGHT, r_ptr->aaf);break;
					case RBM_WAIL:  mana = 5; range = 4; break;
					case RBM_SPORE:	mana = 0; range = 3; must_hit = TRUE; has_ammo = rlev; break;
					case RBM_LASH:  mana = 0; range = 3; break;
					case RBM_BEG:	mana = 0; range = 4; break;
					case RBM_INSULT: mana = 0; range = 4; break;
					case RBM_MOAN: mana = 0; range = 2; break;
					case RBM_SING:  mana = 0; range = 4; break;
					case RBM_TRAP:  mana = 0; range = 1; break;
					case RBM_BOULDER: mana = 0; range = 8; must_hit = TRUE; has_ammo = (rlev + 1) / 2; break;
					case RBM_AURA:	mana = 4; range = 2; break;
					case RBM_SELF:	mana = 3; range = 0; break;
					case RBM_ADJACENT: mana = 3; range = 1; break;
					case RBM_HANDS: mana = 2; range = 3; break;
					case RBM_MISSILE: mana = 2; range = MAX_RANGE; break;
					case RBM_BOLT_10: mana = 5; range = MAX_RANGE; break;
					case RBM_BOLT: mana = 4; range = MAX_RANGE; break;
					case RBM_BEAM: mana = 6; range = 10; break;
					case RBM_BLAST: mana = 3; range = 1; break;
					case RBM_WALL: mana = 6; range = MAX_RANGE; break;
					case RBM_BALL: mana = 4; range = MAX_RANGE; break;
					case RBM_CLOUD: mana = 5; range = MAX_RANGE; break;
					case RBM_STORM: mana = 6; range = MAX_RANGE; break;
					case RBM_BREATH: mana = 0; range = 6; break;
					case RBM_AREA: mana = 3; range = (rlev / 10) + 1; break;
					case RBM_LOS: mana = 6; range = MAX_SIGHT; break;
					case RBM_LINE: mana = 4; range = MAX_RANGE; break;
					case RBM_AIM: mana = 4; range = MAX_SIGHT; break;
					case RBM_ORB: mana = 5; range = MAX_RANGE; break;
					case RBM_STAR: mana = 5; range = MAX_RANGE; break;
					case RBM_SPHERE: mana = 6; range = MAX_RANGE; break;
					case RBM_PANEL: mana = 6; range = MAX_SIGHT; break;
					case RBM_LEVEL: mana = 8; range = 255; break;
					case RBM_CROSS: mana = 4; range = MAX_RANGE; break;
					case RBM_STRIKE: mana = 5; range = MAX_RANGE; break;
					case RBM_EXPLODE: mana = 0; range = 1; break;
					case RBM_ARROW: mana = 0; range = 10; must_hit = TRUE; has_ammo = rlev; break;
					case RBM_XBOLT: mana = 0; range = 12; must_hit = TRUE; has_ammo = rlev; break;
					case RBM_DAGGER: mana = 0; range = 6; must_hit = TRUE; has_ammo = rlev; break;
					case RBM_DART: mana = 0; range = 6; must_hit = TRUE; has_ammo = rlev; break;
					case RBM_SHOT: mana = 0; range = 8; must_hit = TRUE; break;
					case RBM_ARC_20: mana = 6; range = 8; break;
					case RBM_ARC_30: mana = 5; range = 6; break;
				   case RBM_ARC_40: mana = 5; range = 6; break;
				   case RBM_ARC_50: mana = 6; range = 6; break;
				   case RBM_ARC_60: mana = 6; range = 6; break;
					case RBM_FLASK:	mana = 0; range = 6; must_hit = TRUE; has_ammo = (rlev + 1) / 2; break;
				case RBM_TRAIL: mana = 1; range = 3; break;
				case RBM_SHRIEK: mana = 1; range = MAX_SIGHT; break;
				   case RBM_BOLT_MINOR: mana = 2; range = 4; break;
			      case RBM_BALL_MINOR: mana = 3; range = MAX_RANGE; break;
				  case RBM_BALL_II: mana = 5; range = MAX_RANGE; break;
				   case RBM_BALL_III: mana = 6; range = MAX_RANGE; break;
			      case RBM_AURA_MINOR:	mana = 3; range = 1; break;
				   case RBM_8WAY: mana = 4; range = MAX_RANGE; break;
				   case RBM_8WAY_II: mana = 5; range = MAX_RANGE; break;
				   case RBM_8WAY_III: mana = 6; range = MAX_RANGE; break;
				   case RBM_SWARM: mana = 6; range = MAX_RANGE; break;
					case RBM_SPIKE: mana = 0; range = 4; must_hit = TRUE; has_ammo = rlev; break;
					case RBM_AIM_AREA: mana = 5; range = MAX_SIGHT; break;
				   case RBM_SCATTER:  mana = 4; range = MAX_SIGHT; break;
				   case RBM_HOWL:  mana = 2; range = 2; break;
				default: assert (FALSE);
				}

				/* Archers get more shots */
				if (r_ptr->flags2 & (RF2_ARCHER)) has_ammo *= 2;

				/* Scale if needs to hit */
				if (must_hit && !(r_ptr->flags9 & (RF9_NEVER_MISS)))
				{
					atk_dam = atk_dam * MIN(45 + rlev * 3, 95) / 100;
				}

				/* Scale for maximum range */
				if (r_ptr->flags2 & (RF2_KILL_WALL | RF2_PASS_WALL)) atk_dam *= 10;
				else if (range >= 7) atk_dam *= 10;
				else
					atk_dam = atk_dam * (3 + range) + atk_dam * extract_energy[r_ptr->speed + (r_ptr->flags6 & RF6_HASTE ? 5 : 0)] / (7 - range);

				/* Get frequency */
				freq = r_ptr->freq_innate;
#if 0
				/* Paranoia */
				if (!freq)
				{
					r_ptr->highest_threat = 30000;
					r_ptr->best_threat = 96 + i;
				}

				/* Paranoia */
				if ((mana) && !(r_ptr->mana))
				{
					r_ptr->highest_threat = 30000;
					r_ptr->best_threat = 96 + i;
				}
#endif
				/* Adjust frequency for ammo */
				freq = MIN(freq, has_ammo * 5);

				/* Adjust frequency for mana 
				   --- casters that can add mana and need to do so */
				if ((r_ptr->flags6 & (RF6_ADD_MANA)) 
					&& mana 
					&& freq > r_ptr->mana * 10 / mana)
				{
					freq = (freq + r_ptr->mana * 10 / mana) / 2;
				}

				/* Adjust frequency for mana */
				else if (mana)
				{
					freq = MIN(freq, r_ptr->mana * 10 / mana);
				}

				/* Hack -- always get 1 shot */
				if (freq < 10) freq = 10;

				/* Hack -- aura frequency */
				if (method == RBM_AURA) freq += 100;

				/* Adjust for frequency */
				atk_dam = atk_dam * freq / 100;

				/* Best ranged attack? */
				if (atk_dam > spell_dam)
				{
					spell_dam = atk_dam;
					r_ptr->best_spell = 96 + i;
				}
			}

			/* Hack -- aura is extra tough in melee */
			if (method == RBM_AURA) 
			{
				/* Keep a running total */
				melee_dam += 3 * atk_dam;
			}
		}

		/* Hack - record most damaging attack */
		if ((threat_mod(melee_dam, r_ptr, FALSE) > r_ptr->highest_threat) && (!(r_ptr->flags1 & (RF1_NEVER_MOVE)) || (r_ptr->flags2 & (RF2_MULTIPLY))))
		{
			r_ptr->highest_threat = threat_mod(melee_dam, r_ptr, FALSE);
			r_ptr->best_threat = 0;
		}

		/* 
		 * Apply damage over 10 rounds. We assume that the monster has to make contact first.
		 * Hack - speed has more impact on melee as has to stay in contact with player.
		 * Hack - this is except for pass wall and kill wall monsters which can always get to the player.
		 * Hack - use different values for huge monsters as they strike out to range 2.
		 */
		if (r_ptr->flags2 & (RF2_KILL_WALL | RF2_PASS_WALL))
				melee_dam *= 10;
		else if ((r_ptr->flags3 & (RF3_HUGE)) ||
			(((r_ptr->flags2 & (RF2_INVISIBLE)) || (r_ptr->flags6 & (RF6_INVIS))) && (r_ptr->flags2 & (RF2_COLD_BLOOD))))
		{
			melee_dam = melee_dam * 4 + melee_dam * extract_energy[r_ptr->speed + (r_ptr->flags6 & RF6_HASTE ? 5 : 0)] / 6;
		}
		else
		{
			melee_dam = melee_dam * 3 + melee_dam * extract_energy[r_ptr->speed + (r_ptr->flags6 & RF6_HASTE ? 5 : 0)] / 7;
		}

		/*
		 * Scale based on attack accuracy. We make a massive number of assumptions here and just use monster level.
		 */
		if (!(r_ptr->flags9 & (RF9_NEVER_MISS)))
		{
			int power = rlev + (r_ptr->flags6 & (RF6_BLESS) ? 5 : 0) + (r_ptr->flags6 & (RF6_BERSERK) ? 12 : 0);

			melee_dam = melee_dam * MIN(45 + power * 3, 95) / 100;
		}

		/* Hack -- Monsters that multiply ignore the following reductions */
		if (!(r_ptr->flags2 & (RF2_MULTIPLY)))
		{
			/*Reduce damamge potential for monsters that move randomly */
			if ((r_ptr->flags1 & (RF1_RAND_25)) || (r_ptr->flags1 & (RF1_RAND_50)))
			{
				int reduce = 100;

				if (r_ptr->flags1 & (RF1_RAND_25)) reduce -= 25;
				if (r_ptr->flags1 & (RF1_RAND_50)) reduce -= 50;

				/*even moving randomly one in 8 times will hit the player*/
				reduce += (100 - reduce) / 8;

				/* adjust the melee damage*/
				melee_dam = (melee_dam * reduce) / 100;
			}

			/*monsters who can't move aren't nearly as much of a combat threat*/
			if (r_ptr->flags1 & (RF1_NEVER_MOVE))
			{
				if (r_ptr->flags6 & (RF6_TELE_TO | RF6_TELE_SELF_TO | RF6_BLINK))
				{
					/* Scale for frequency */
					melee_dam = melee_dam / 5 + 4 * melee_dam * r_ptr->freq_spell / 500;

					/* Incorporate spell failure chance */
					if (!(r_ptr->flags2 & RF2_STUPID)) melee_dam = melee_dam / 5 + 4 * melee_dam * MIN(75 + (rlev + 3) / 4, 100) / 500;
				}
				else if (r_ptr->flags2 & (RF2_HAS_AURA)) melee_dam /= 2;
				else if ((r_ptr->flags2 & (RF2_INVISIBLE)) && (r_ptr->flags2 & (RF2_COLD_BLOOD))) melee_dam /= 3;
				else melee_dam /= 5;
			}
		}

		/* But keep at a minimum */
		if (melee_dam < 1) melee_dam = 1;
	}

	/*
	 * Get the max damage attack
	 */

	if (dam < spell_dam_real) dam = spell_dam_real;
	if (dam < melee_dam) dam = melee_dam;
	
	/*
	 * Limit 'unreal' spell damage boost
	 */
	if (dam < spell_dam)
	{
		if (spell_dam < 2 * dam)
		{
			dam = spell_dam;
		}
		else
		{
			dam *= 2;
		}
	}

	/*
	 * Adjust for speed.  Monster at speed 120 will do double damage,
	 * monster at speed 100 will do half, etc.  Bonus for monsters who can haste self.
	 */
	dam = (dam * extract_energy[r_ptr->speed + (r_ptr->flags6 & RF6_HASTE ? 5 : 0)]) / 10;

	/*
	 * Adjust some 'global' threat values.
	 */
	/* Shriek applies here */
	if (shriek) r_ptr->highest_threat *= 2;
		
	/* Invisibility increases overall threat */
	else if (r_ptr->flags2 & (RF2_INVISIBLE)) r_ptr->highest_threat = r_ptr->highest_threat * 3 / 2;

	/* Even helpless monsters are after the player */
	if (dam < 3) dam  = 3;

	/* We're done */
	return (dam);
}


/* Evaluate and adjust a monsters hit points for how easily the monster is damaged */
static long eval_hp_adjust(monster_race *r_ptr)
{
	long raw_hp, hp;
	int resists = 1;
	int ac = 0;
	int hide_bonus = 0;

	/* Get the monster base hitpoints */
	if (r_ptr->flags1 & (RF1_FORCE_MAXHP)) raw_hp = r_ptr->hdice * r_ptr->hside;
	else raw_hp = r_ptr->hdice * (r_ptr->hside + 1) / 2;

	hp = raw_hp;

	/* Never moves with no ranged attacks - high hit points count for less */
	if ((r_ptr->flags1 & (RF1_NEVER_MOVE)) && !(r_ptr->freq_innate || r_ptr->freq_spell))
	{
		hp /= 2;
		if (hp < 1) hp = 1;
	}

	/* Just assume healers have more staying power */
	if (r_ptr->flags6 & RF6_HEAL) hp = (hp * 6) / 5;
	else if (r_ptr->flags6 & RF6_CURE) hp = (hp * 15) / 14;

	/* Miscellaneous improvements */
	if (r_ptr->flags2 & RF2_REGENERATE) {hp *= 10; hp /= 9;}
	if (r_ptr->flags2 & RF2_PASS_WALL) 	{hp *= 3; hp /= 2;}
	else if (r_ptr->flags6 & RF6_WRAITHFORM) {hp *= 6; hp /= 5;}

	/* Calculate hide bonus */
	if (r_ptr->flags2 & RF2_EMPTY_MIND) hide_bonus += 2;
	else
	{
		if (r_ptr->flags2 & RF2_COLD_BLOOD) hide_bonus += 1;
		if (r_ptr->flags2 & RF2_WEIRD_MIND) hide_bonus += 1;
	}

	/* Invisibility */
	if (r_ptr->flags2 & RF2_INVISIBLE)
	{
		hp = (hp * (r_ptr->level + hide_bonus + 1)) / MAX(1, r_ptr->level);
	}
	if (r_ptr->flags6 & RF6_INVIS)
	{
		hp = (hp * (r_ptr->level + hide_bonus)) / MAX(1, r_ptr->level);
	}

	/* Monsters that can teleport are a hassle, and can easily run away */
	if 	((r_ptr->flags6 & RF6_TPORT) ||
		 (r_ptr->flags6 & RF6_TELE_AWAY)||
		 (r_ptr->flags6 & RF6_TELE_LEVEL)) hp = (hp * 6) / 5;

	/*
 	 * Monsters that multiply are tougher to kill and each takes a blow
	 */
	if (r_ptr->flags2 & (RF2_MULTIPLY)) 
		hp = (3 + hp * 3) 
			* extract_energy[r_ptr->speed + (r_ptr->flags6 & RF6_HASTE ? 5 : 0)] 
			/ 10;


	/* Get the monster ac */
	ac = r_ptr->ac;

	/* Some abilities modify armour */
	if (r_ptr->flags2 & RF2_ARMOR) ac = ac * 4 / 3;
	if (r_ptr->flags6 & RF6_SHIELD) ac += 25;
	if (r_ptr->flags6 & RF6_BLESS) ac += 5;
	if (r_ptr->flags6 & RF6_BERSERK) ac -= 5;

	/* Upper limit on ac */
	if (ac > 150) ac = 150;

	/* Immune to weapons */
	if (r_ptr->flags9 & RF9_IM_EDGED)	ac += 500;
	else if (r_ptr->flags9 & RF9_RES_EDGED)	ac += 200;
	if (r_ptr->flags9 & RF9_IM_BLUNT) 	ac += 500;
	else if (r_ptr->flags9 & RF9_RES_BLUNT) ac += 200;	

	/* Hack -- Huge = tough */
	if (r_ptr->flags3 & RF3_HUGE) ac += 300;
	
	/* Monsters with resistances are harder to kill.
	   Therefore effective slays / brands against them are worth more. */
	if (r_ptr->flags3 & RF3_IM_ACID) resists += 2;
	if (r_ptr->flags3 & RF3_IM_FIRE) resists += 2;
	if (r_ptr->flags3 & RF3_IM_COLD) resists += 2;
	if (r_ptr->flags3 & RF3_IM_ELEC) resists += 2;
	if (r_ptr->flags3 & RF3_IM_POIS) resists += 2;

	/* Oppose elements */
	if (r_ptr->flags6 & RF6_OPPOSE_ELEM)
	{
		if (resists < 5)	resists = 5;
		else if (resists < 10)  resists++;
	}

	/* Hack - Immune to weapons & basic resists = tough */
	if (r_ptr->flags9 & RF9_IM_EDGED)	resists += 5;
	else if (r_ptr->flags9 & RF9_RES_EDGED)	resists += 2;
	if (r_ptr->flags9 & RF9_IM_BLUNT) 	resists += 5;
	else if (r_ptr->flags9 & RF9_RES_BLUNT) resists += 2;

	/* Hack -- Evasive = tough */
	if (r_ptr->flags9 & RF9_EVASIVE) resists += 3;
 
	/* Bonus for multiple basic resists and weapon resists */
	if (resists >= 12) resists *= 6;
	else if (resists >= 10) resists *= 4;
	else if (resists >= 8) resists *= 3;
	else if (resists >= 6) resists *= 2;

	/* Terrain exploits reduce overall resists */
	if (r_ptr->flags3 & RF3_HURT_LITE) 	resists /= 2;
	if (r_ptr->flags2 & RF2_MUST_FLY)	resists /= 2;
	if (r_ptr->flags2 & RF2_MUST_SWIM)	resists /= 3;

	/* If quite resistant, reduce resists by defense holes -- these are critical blow effects */
	if (resists >= 6)
	{
		if (!(r_ptr->flags3 & RF3_NO_STUN))	resists -= 1;
		if (!(r_ptr->flags9 & RF9_NO_CUTS))	resists -= 1;
	}
	
	/* If quite resistant, bonus for high resists -- these items have weapon brands */
	if (resists >= 3)
	{
		if (r_ptr->flags9 & RF9_RES_DARK)	resists += 1;
		if (r_ptr->flags9 & RF9_RES_LITE)	resists += 1;
	}

	/* Modify ac based on resists, for weapon brands */
	ac += resists * 10;

	/* If quite resistant, modify ac directly for slays */
	if (resists >= 3)
	{
		/* Modify ac by slays */
		if (r_ptr->flags3 & RF3_ORC) ac -= 10;
		if (r_ptr->flags3 & RF3_TROLL) ac -= 10;
		if (r_ptr->flags3 & RF3_GIANT) ac -= 10;
		if (r_ptr->flags3 & RF3_DRAGON) ac -= 30;
		if (r_ptr->flags3 & RF3_DEMON) ac -= 30;
		if (r_ptr->flags3 & RF3_UNDEAD) ac -= 30;
		if (r_ptr->flags3 & RF3_EVIL) ac -= 20;
		if (r_ptr->flags3 & (RF3_ANIMAL | RF3_INSECT | RF3_PLANT)) ac -= 20;
		if (r_ptr->flags9 & RF9_MAN) ac -= 10;
		if (r_ptr->flags9 & RF9_ELF) ac -= 10;
		if (r_ptr->flags9 & RF9_DWARF) ac -= 10;		
	}

	/* If quite resistant, reduce resists by defense holes */
	if (resists >= 6)
	{
		if (r_ptr->flags3 & RF3_HURT_ROCK) resists -= 1;
		if (r_ptr->flags3 & RF3_HURT_WATER) resists -= 1;
		if (!(r_ptr->flags3 & RF3_NO_SLEEP)) resists -= 3;
		if (!(r_ptr->flags3 & RF3_NO_FEAR)) resists -= 2;
		if (!(r_ptr->flags3 & RF3_NO_CONF)) resists -= 2;
		if (!(r_ptr->flags9 & RF9_NO_SLOW)) resists -= 2;
		if (!(r_ptr->flags3 & RF9_RES_BLIND)) resists -= 2;
		if (!(r_ptr->flags9 & RF9_RES_TPORT)) resists -= 2;

		if (resists < 5) resists = 5;
	}

	/* If quite resistant, bonus for high resists */
	if (resists >= 3)
	{
		if (r_ptr->flags9 & RF9_RES_CHAOS)	resists += 1;
		if (r_ptr->flags3 & RF3_RES_WATER)	resists += 1;
		if (r_ptr->flags3 & RF3_RES_NETHR)	resists += 1;
		if (r_ptr->flags3 & RF3_RES_LAVA)	resists += 1;
		if (r_ptr->flags3 & RF3_RES_NEXUS)	resists += 1;
		if (r_ptr->flags3 & RF3_RES_DISEN)	resists += 1;
	}

	/* Scale resists to ac */
	resists *= 25;

	/* Apply magic resistance */
	if (r_ptr->flags9 & RF9_RES_MAGIC)	resists += 40 + r_ptr->level;

	/* Sanity check */
	if (ac < 0) ac = 0;

	/* Monster resistances */
	if (resists < ac)
	{
		hp += (hp * resists) / (150 + r_ptr->level); 	
	}
	else
	{
		hp += (hp * ac) / (150 + r_ptr->level); 			
	}

	/* Low level monsters don't gain anything above */
	hp += (raw_hp + resists/3) / 4;			

	/* boundary control */
	if (hp < 1) hp = 1;

	return (hp);
}


/*
 * Evaluate the monster power ratings to be stored in r_info.raw
 */
errr eval_r_power(header *head)
{
	int i, j;
	byte lvl;
	long hp, av_hp, av_dam;
	long tot_hp[MAX_DEPTH];
	long dam;
	long *power;
	long tot_dam[MAX_DEPTH];
	long mon_count[MAX_DEPTH];
	monster_race *r_ptr = NULL;

#ifdef ALLOW_TEMPLATES_OUTPUT
	int iteration;

	/* Hack -- Clear item drop flags when monster doesn't drop anything */
	for (i = 0; i < z_info->r_max; i++)
	{
		/* Point at the "info" */
		r_ptr = (monster_race*)head->info_ptr + i;
		
		if ((r_ptr->flags1 & (RF1_DROP_30 | RF1_DROP_60 | RF1_DROP_90 | RF1_DROP_1D2 | RF1_DROP_1D3 | RF1_DROP_1D4)) == 0)
		{
			r_ptr->flags1 &= ~(RF1_ONLY_GOLD | RF1_ONLY_ITEM | RF1_DROP_GOOD | RF1_DROP_GREAT | RF1_DROP_USEFUL);
			r_ptr->flags8 &= ~(RF8_DROP_CHEST | RF8_DROP_MISSILE | RF8_DROP_TOOL | RF8_DROP_WEAPON | RF8_DROP_MUSIC |
								RF8_DROP_CLOTHES | RF8_DROP_LITE | RF8_DROP_JEWELRY | RF8_DROP_RSW | RF8_DROP_WRITING |
								RF8_DROP_POTION | RF8_DROP_FOOD | RF8_DROP_JUNK);			
		}
	}
#endif /* ALLOW_TEMPLATES_OUTPUT */

	/* Allocate space for power */
	power = C_ZNEW(z_info->r_max, long);

#ifdef ALLOW_TEMPLATES_OUTPUT
	/* We iterate through the following evaluation multiple times to try to find 'settled' values.
	 * This is because evaluating the monster can result in it changing depth and/or rarity */
	for (iteration = 0; iteration < 3; iteration ++)
	{
#endif /* ALLOW_TEMPLATES_OUTPUT */
		/* Reset the sum of all monster power values */
		tot_mon_power = 0;
	
		/* Make sure all arrays start at zero */
		for (i = 0; i < MAX_DEPTH; i++)
		{
			tot_hp[i] = 0;
			tot_dam[i] = 0;
			mon_count[i] = 0;
		}
	
		/*
		 * Go through r_info and evaluate power ratings & flows.
		 */
		for (i = 0; i < z_info->r_max; i++)
		{
			/* Point at the "info" */
			r_ptr = (monster_race*)head->info_ptr + i;
	#if 0
			/*** Evaluate flows ***/
	
			/* Evaluate 'unusual' flows */
			if (r_ptr->flags2 & (RF2_KILL_WALL | RF2_PASS_WALL)) r_ptr->flows |= (1L << FLOW_WALK_WALL);
			if (r_ptr->flags2 & (RF2_MUST_FLY)) r_ptr->flows |= (1L << FLOW_FLY);
			if (r_ptr->flags2 & (RF2_MUST_SWIM)) r_ptr->flows |= (1L << FLOW_SWIM);
	
			/* Evaluate walking flows */
			if ((r_ptr->flags2 & (RF2_MUST_FLY | RF2_MUST_SWIM)) == 0)
			{
				/* Use walking */
				r_ptr->flows |= (1L << FLOW_WALK);
	
				/* Evaluate walk + other */
				if (r_ptr->flags2 & (RF2_CAN_DIG)) r_ptr->flows |= (1L << FLOW_WALK_DIG) | (1L << FLOW_DIG);
				if (r_ptr->flags2 & (RF2_CAN_FLY)) r_ptr->flows |= (1L << FLOW_WALK_FLY) | (1L << FLOW_FLY_CHASM);
				if (r_ptr->flags2 & (RF2_CAN_SWIM)) r_ptr->flows |= (1L << FLOW_WALK_SWIM) | (1L << FLOW_SWIM);
				if (r_ptr->flags2 & (RF2_CAN_CLIMB)) r_ptr->flows |= (1L << FLOW_WALK_CLIMB);
				if (r_ptr->flags2 & (RF2_BASH_DOOR)) r_ptr->flows |= (1L << FLOW_WALK_BASH);
				if (r_ptr->flags2 & (RF2_OPEN_DOOR)) r_ptr->flows |= (1L << FLOW_WALK_OPEN);
				if ((r_ptr->flags2 & (RF2_BASH_DOOR)) && (r_ptr->flags2 & (RF2_OPEN_DOOR))) r_ptr->flows |= (1L << FLOW_WALK_BASH_OPEN);
				if (r_ptr->flags3 & (RF3_OOZE)) r_ptr->flows |= (1L << FLOW_WALK_OOZE);
				if (r_ptr->flags3 & (RF3_NONLIVING)) r_ptr->flows |= (1L << FLOW_WALK_NONLIVING);
				if (r_ptr->flags3 & (RF3_IM_ACID)) r_ptr->flows |= (1L << FLOW_WALK_ACID) | (1L << FLOW_ACID);
				if (r_ptr->flags3 & (RF3_IM_COLD)) r_ptr->flows |= (1L << FLOW_WALK_COLD);
				if (r_ptr->flags3 & (RF3_IM_ELEC)) r_ptr->flows |= (1L << FLOW_WALK_ELEC);
	
				/* Evaluate fire immunity flows */
				if (r_ptr->flags3 & (RF3_IM_FIRE))
				{
					/* Use walking + fire immunity */
					r_ptr->flows |= (1L << FLOW_WALK_FIRE) | (1L << FLOW_FIRE);
	
					if (r_ptr->flags2 & (RF2_CAN_DIG)) r_ptr->flows |= (1L << FLOW_WALK_FIRE_DIG) | (1L << FLOW_FIRE_DIG);
					if (r_ptr->flags2 & (RF2_CAN_SWIM)) r_ptr->flows |= (1L << FLOW_WALK_FIRE_SWIM) | (1L << FLOW_FIRE_SWIM);
				}
			}
	#endif
			/*** Evaluate power ratings ***/
	
			/* Set the current level */
			lvl = r_ptr->level;
	
			/* Maximum damage this monster can do in 10 game turns */
			dam = eval_max_dam(r_ptr);
	
			/* Adjust hit points based on resistances */
			hp = eval_hp_adjust(r_ptr);

#ifdef ALLOW_TEMPLATES_OUTPUT
			/* Hack -- set exp */
			if (lvl == 0) r_ptr->mexp = 0L;
			else
			{
				/* Compute depths and exp of non-unique monsters.
				   Enable occasionally even for uniques and then manually
				   repair for the 3 Trolls, Beorn, etc. */
				if (!(r_ptr->flags1 & (RF1_UNIQUE)))
				{
					long mexp = (hp * dam) / 25;
					long threat = r_ptr->highest_threat;
	
					/* Compute level algorithmically */
					for (j = 1; (mexp > j + 4) || (threat > j + 5); mexp -= j * j, threat -= (j + 4), j++);
	
					/* Set level */
					lvl = MIN(( j > 250 ? 90 + (j - 250) / 20 : 	/* Level 90 and above */
							(j > 130 ? 70 + (j - 130) / 6 :	/* Level 70 and above */
							(j > 40 ? 40 + (j - 40) / 3 :	/* Level 40 and above */
							j))), 99);
	
					/* Set level */
					r_ptr->level = lvl;

					/* Hack -- for Ungoliant-like monsters */
					if (hp > 10000) r_ptr->mexp = (hp / 25) * (dam / lvl);
					else r_ptr->mexp = (hp * dam) / (lvl * 25);
	
					/* Round to 2 significant figures */
					if (r_ptr->mexp > 100)
					{
						if (r_ptr->mexp < 1000) { r_ptr->mexp = (r_ptr->mexp + 5) / 10; r_ptr->mexp *= 10; }
						else if (r_ptr->mexp < 10000) { r_ptr->mexp = (r_ptr->mexp + 50) / 100; r_ptr->mexp *= 100; }
						else if (r_ptr->mexp < 100000) { r_ptr->mexp = (r_ptr->mexp + 500) / 1000; r_ptr->mexp *= 1000; }
						else if (r_ptr->mexp < 1000000) { r_ptr->mexp = (r_ptr->mexp + 5000) / 10000; r_ptr->mexp *= 10000; }
						else if (r_ptr->mexp < 10000000) { r_ptr->mexp = (r_ptr->mexp + 50000) / 100000; r_ptr->mexp *= 100000; }
					}
				}
			}
	
			if ((lvl) && (r_ptr->mexp < 1L)) 
				r_ptr->mexp = 1L;
#endif /* ALLOW_TEMPLATES_OUTPUT */

			/*
			 * Hack - We have to use an adjustment factor to prevent overflow.
			 */
			if (lvl >= 90)
			{
				hp /= 1000;
				dam /= 1000;
			}
			else if (lvl >= 65)
			{
				hp /= 100;
				dam /= 100;
			}
			else if (lvl >= 40)
			{
				hp /= 10;
				dam /= 10;
			}
	
			/* Define the power rating */
			power[i] = hp * dam;
	
			/* Adjust for group monsters.  Average in-level group size is 5 */
			if (r_ptr->flags1 & RF1_UNIQUE) ;
	
			else if (r_ptr->flags1 & RF1_FRIEND) power[i] *= 2;
	
			else if (r_ptr->flags1 & RF1_FRIENDS) power[i] *= 5;
	
			/* Adjust for multiplying monsters. This is modified by the speed,
			 * as fast multipliers are much worse than slow ones. We also adjust for
			 * ability to bypass walls or doors.
			 */
			if (r_ptr->flags2 & RF2_MULTIPLY)
			{
				if (r_ptr->flags2 & (RF2_KILL_WALL | RF2_PASS_WALL))
					power[i] = MAX(power[i], power[i] * extract_energy[r_ptr->speed
						+ (r_ptr->flags6 & RF6_HASTE ? 5 : 0)]);
				else if (r_ptr->flags2 & (RF2_OPEN_DOOR | RF2_BASH_DOOR))
					power[i] = MAX(power[i], power[i] *  extract_energy[r_ptr->speed
						+ (r_ptr->flags6 & RF6_HASTE ? 5 : 0)] * 3 / 2);
				else
					power[i] = MAX(power[i], power[i] * extract_energy[r_ptr->speed
						+ (r_ptr->flags6 & RF6_HASTE ? 5 : 0)] / 2);
			}
	
			/*
			 * Update the running totals - these will be used as divisors later
			 * Total HP / dam / count for everything up to the current level
			 */
			for (j = lvl; j < (lvl == 0 ? lvl + 1: MAX_DEPTH); j++)
			{
				int count = 10;
	
				/*
				 * Uniques don't count towards monster power on the level.
				 */
				if (r_ptr->flags1 & RF1_UNIQUE) continue;
	
				/*
				 * Specifically placed monsters don't count towards monster power on the level.
				 */
				if (!(r_ptr->rarity)) continue;
	
				/*
				 * Hack -- provide adjustment factor to prevent overflow
				 */
				if ((j == 90) && (r_ptr->level < 90))
				{
					hp /= 10;
					dam /= 10;
				}
	
				if ((j == 65) && (r_ptr->level < 65))
				{
					hp /= 10;
					dam /= 10;
				}
	
				if ((j == 40) && (r_ptr->level < 40))
				{
					hp /= 10;
					dam /= 10;
				}
	
				/*
				 * Hack - if it's a group monster or multiplying monster, 
				 * add several to the count
				 * so that the averages don't get thrown off
				 */
	
				if (r_ptr->flags1 & RF1_FRIEND) count = 20;
				else if (r_ptr->flags1 & RF1_FRIENDS) count = 50;
	
				if (r_ptr->flags2 & RF2_MULTIPLY)
				{
					if (r_ptr->flags2 & (RF2_KILL_WALL | RF2_PASS_WALL))
						count = MAX(1, extract_energy[r_ptr->speed
							+ (r_ptr->flags6 & RF6_HASTE ? 5 : 0)]) * count;
					else if (r_ptr->flags2 & (RF2_OPEN_DOOR | RF2_BASH_DOOR))
						count = MAX(1, extract_energy[r_ptr->speed
							+ (r_ptr->flags6 & RF6_HASTE ? 5 : 0)] * 3 / 2) * count;
					else
						count = MAX(1, extract_energy[r_ptr->speed
							+ (r_ptr->flags6 & RF6_HASTE ? 5 : 0)] / 2) * count;
				}
	
				/*
				 * Very rare monsters count less towards total monster power on the level.
				 */
				if (r_ptr->rarity > count)
				{
					hp = hp * count / r_ptr->rarity;
					dam = dam * count / r_ptr->rarity;
	
					count = r_ptr->rarity;
				}
	
				tot_hp[j] += hp;
				tot_dam[j] += dam;
	
				mon_count[j] += count / r_ptr->rarity;
			}
		}
	
		/* Apply divisors now */
		for (i = 0; i < z_info->r_max; i++)
		{
#ifdef ALLOW_TEMPLATES_OUTPUT
			int new_power;
#endif /* ALLOW_TEMPLATES_OUTPUT */
	
			/* Point at the "info" */
			r_ptr = (monster_race*)head->info_ptr + i;
	
			/* Extract level */
			lvl = r_ptr->level;
	
			/* Paranoia */
			if (tot_hp[lvl] != 0 && tot_dam[lvl] != 0)
			{
				/* Divide by average HP and av damage for all in-level monsters */
				/* Note we have factored in the above 'adjustment factor' */
				av_hp = tot_hp[lvl] * 10 / mon_count[lvl];
				av_dam = tot_dam[lvl] * 10 / mon_count[lvl];
	
				/* XXX Justifiable paranoia - avoid divide by zero errors */
				if (av_hp > 0) power[i] = power[i] / av_hp;
				if (av_dam > 0) power[i] = power[i] / av_dam;
	
				/* Assign monster power */
				r_ptr->power = (s16b)power[i];
	
				/* Never less than 1 */
				if (r_ptr->power < 1) r_ptr->power = 1;

#ifdef ALLOW_TEMPLATES_OUTPUT
				/* Get power */
				new_power = r_ptr->power;
	
				/* Compute rarity algorithmically */
				for (j = 1; new_power > j; new_power -= j * j, j++);
	
				/* Set rarity */
				r_ptr->rarity = j;
#endif /* ALLOW_TEMPLATES_OUTPUT */
			}
		}	
#ifdef ALLOW_TEMPLATES_OUTPUT
	}
#endif /* ALLOW_TEMPLATES_OUTPUT */

	/* Free power array */
	FREE(power);

	/* Success */
	return(0);
}


/*
 * Evaluate the ego slay ratings to be stored in e_info.raw
 */
errr eval_e_power(header *head)
{
	int i;
	ego_item_type *e_ptr = NULL;

	/* Precompute values for ego item slays for ego items */
	for (i = 0; i < z_info->e_max; i++)
	{
		/* Point at the "info" */
		e_ptr = (ego_item_type*)head->info_ptr + i;

		e_ptr->slay_power = slay_power(slay_index(e_ptr->flags1, e_ptr->flags2, e_ptr->flags3, e_ptr->flags4));
	}

	/* Success */
	return(0);
}


/*
 * Evaluate the feature flows to be stored in f_info.raw
 */
errr eval_f_power(header *head)
{
	int i;
	feature_type *f_ptr = NULL;
#if 0
	feature_type *f2_ptr = NULL;
#endif

	/* Precompute values for feature flows */
	for (i = 0; i < z_info->f_max; i++)
	{
		/* Point at the "info" */
		f_ptr = (feature_type*)head->info_ptr + i;

	}


	/* Success */
	return(0);
}



#ifdef ALLOW_TEMPLATES_OUTPUT

/*
 * Emit a "template" file.
 * 
 * This allows us to modify an existing template file by parsing it
 * in and then modifying the data structures.
 * 
 * We parse the previous "template" file to allow us to include comments.
 */
errr emit_info_txt(FILE *fp, FILE *template, char *buf, header *head,
   emit_info_txt_index_func emit_info_txt_index, emit_info_txt_always_func emit_info_txt_always)
{
	errr err;

	/* Not ready yet */
	bool okay = FALSE;
	bool comment = FALSE;
	int blanklines = 0;

	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = 0;

	/* Parse */
	while (0 == my_fgets(template, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip blank lines */
		if (!buf[0])
		{
			if (comment) blanklines++;
			continue;
		}

		/* Emit a comment line */
		if (buf[0] == '#')
		{
			/* Skip comments created by emission process */
			if (buf[1] == '$') continue;

			while (blanklines--) fprintf(fp,"\n");
			fprintf(fp,"%s\n",buf);
			comment = TRUE;
			blanklines = 0;
			continue;
		}

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);

		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			if (comment) fprintf(fp,"\n");
			comment = FALSE;

			/* Output the version number */
			fprintf(fp, "\nV:%d.%d.%d\n\n", head->v_major, head->v_minor, head->v_patch);
			
			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);

		/* Hack -- Process 'N' for "Name" */
		if ((emit_info_txt_index) && (buf[0] == 'N'))
		{
			int idx;
			
			idx = atoi(buf + 2);

			/* Verify index order */
			if (idx < ++error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);
			
			/* Verify information */
			if (idx >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);
			
			if (comment) fprintf(fp,"\n");
			comment = FALSE;
			blanklines = 0;

			while (error_idx < idx)
			{
				fprintf(fp,"#$# %d - Unused ###\n\n",error_idx++);	
			}

			if ((err = (emit_info_txt_index(fp, head, idx))) != 0)
				return (err);
		}

		/* Ignore anything else and continue */
		continue;
	}

	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);

	if ((emit_info_txt_always) && ((err = (emit_info_txt_always(fp, head))) != 0))
				return (err);

	/* Success */
	return (0);
}


/*
 * Emit one textual string based on a flag.
 */
static errr emit_flags_32(FILE *fp, cptr intro_text, u32b flags, cptr names[])
{
	int i;
	bool intro = TRUE;
	int len = 0;

	/* Check flags */
	for (i = 0; i < 32; i++)
	{
		if ((flags & (1L << i)) != 0)
		{
			/* Newline needed */
			if (len + strlen(names[i]) > 75)
			{
					fprintf(fp,"\n");
					len = 0;
					intro = TRUE;
			}
			
			/* Introduction needed */
			if (intro)
			{
				fprintf(fp, intro_text);
				len += strlen(intro_text);
				intro = FALSE;
			}
			else
			{
				fprintf(fp," ");
				len++;
			}
			
			/* Output flag */
			fprintf(fp, "%s |", names[i]);
			len += strlen(names[i]) + 2;
		}
	}

	/* Something output */
	if (!intro) fprintf(fp, "\n");

	return (0);
}

/*
 * Emit description to file.
 * 
 * TODO: Consider merging with text_out_to_file in util.c,
 * where most of this came from.
 */
static errr emit_desc(FILE *fp, cptr intro_text, cptr text)
{	
	/* Current position on the line */
	int pos = 0;

	/* Wrap width */
	int wrap = 75 - strlen(intro_text);

	/* Current location within "str" */
	cptr s = text;
	
	/* Process the string */
	while (*s)
	{
		char ch;
		int n = 0;
		int len = wrap - pos;
		int l_space = 0;

		/* If we are at the start of the line... */
		if (pos == 0)
		{
			fprintf(fp, intro_text);
		}

		/* Find length of line up to next newline or end-of-string */
		while ((n < len) && !((s[n] == '\n') || (s[n] == '\0')))
		{
			/* Mark the most recent space in the string */
			if (s[n] == ' ') l_space = n;

			/* Increment */
			n++;
		}

		/* If we have encountered no spaces */
		if ((l_space == 0) && (n == len))
		{
			/* If we are at the start of a new line */
			if (pos == 0)
			{
				len = n;
			}
			else
			{
				/* Begin a new line */
				fputc('\n', fp);

				/* Reset */
				pos = 0;

				continue;
			}
		}
		else
		{
			/* Wrap at the newline */
			if ((s[n] == '\n') || (s[n] == '\0')) len = n;

			/* Wrap at the last space */
			else len = l_space;
		}

		/* Write that line to file */
		for (n = 0; n < len; n++)
		{
			/* Ensure the character is printable */
			ch = (isprint(s[n]) ? s[n] : ' ');

			/* Write out the character */
			fputc(ch, fp);

			/* Increment */
			pos++;
		}

		/* Move 's' past the stuff we've written */
		s += len;

		/* Skip newlines */
		if (*s == '\n') s++;

		/* Begin a new line */
		fputc('\n', fp);

		/* If we are at the end of the string, end */
		if (*s == '\0') return (0);

		/* Reset */
		pos = 0;
	}

	/* We are done */
	return (0);
}


static char color_attr_to_char(int a)
{
	return (color_table[a].index_char);
}


/*
 * Emit the "blow_info" array into an ascii "template" file
 */
errr emit_blow_info_index(FILE *fp, header *head, int i)
{
	int n;

	/* Point at the "info" */
	blow_type *blow_ptr = (blow_type*)head->info_ptr + i;

	/* Output 'N' for "New/Number/Name" */
	fprintf(fp, "N:%d:%s\n", i,head->name_ptr + blow_ptr->name);
	
	/* Output 'T' for "Text" */
	for (n = 0; n < MAX_BLOW_DESCRIPTIONS; n++)
	{
		if ((blow_ptr->desc[n].max) && (blow_ptr->desc[n].max < 10000))
		{
			/* Output 'T' for "Text" */
			fprintf(fp, "T:%d-%d:%s\n", blow_ptr->desc[n].min, blow_ptr->desc[n].max,head->text_ptr + blow_ptr->desc[n].text);			
		}
		else if (blow_ptr->desc[n].max)
		{
			/* Output 'T' for "Text" */
			fprintf(fp, "T:%d:%s\n", blow_ptr->desc[n].min, head->text_ptr + blow_ptr->desc[n].text);
		}
	}

	/* Output 'I' for "Blow information" */
	if (blow_ptr->mana_cost || blow_ptr->best_range || blow_ptr->max_range)
	{
		fprintf(fp, "I:%d:%d:%d\n",blow_ptr->mana_cost,blow_ptr->best_range, blow_ptr->max_range);
	}
	
	/* Output 'D' for "Blow damage" */
	if (blow_ptr->dam_mult || blow_ptr->dam_div || blow_ptr->dam_var || blow_ptr->dam_max || blow_ptr->d_res)
	{
		fprintf(fp, "D:%d:%d:%d:%d:%s\n",blow_ptr->dam_mult, blow_ptr->dam_div, blow_ptr->dam_var, blow_ptr->dam_max,
			r_info_blow_effect[blow_ptr->d_res]);
	}
	
	/* Output 'C' for "Blow choices" */
	if (blow_ptr->d_base || blow_ptr->d_hurt || blow_ptr->d_esc
			|| blow_ptr->d_summ || blow_ptr->d_mana || blow_ptr->d_tact || blow_ptr->d_range)
	{
		fprintf(fp, "C:%d:%d:%d:%d:%d:%d:%d\n",blow_ptr->d_base, blow_ptr->d_summ, blow_ptr->d_hurt, blow_ptr->d_mana,
				blow_ptr->d_esc, blow_ptr->d_tact, blow_ptr->d_range);
	}
	
	/* Output 'S' for "Summoning choices" */
	if (blow_ptr->summon_type)
	{
		fprintf(fp, "S:%d\n",blow_ptr->summon_type);
	}
	
	/* Output 'A' for "Arc information" */
	if (blow_ptr->arc)
	{
		fprintf(fp, "A:%d:%d:%d\n", blow_ptr->arc, blow_ptr->diameter_of_source, blow_ptr->degree_factor);
	}
	
	/* Output 'R' for "Radius information */
	if ((blow_ptr->radius.base) || (blow_ptr->radius.gain))
	{
		fprintf(fp, "R:");
		if (blow_ptr->radius.base)
		{
			fprintf(fp,"%d", blow_ptr->radius.base);
			if (blow_ptr->radius.gain) fprintf(fp, "+");
		}
		if (blow_ptr->radius.gain)
		{
			fprintf(fp,"%d/%d", blow_ptr->radius.gain, blow_ptr->radius.levels);
		}
		fprintf(fp, "\n");
	}
	
	/* Output 'U' for "Number information */
	if ((blow_ptr->number.base) || (blow_ptr->number.gain))
	{
		fprintf(fp, "U:");
		if (blow_ptr->number.base)
		{
			fprintf(fp,"%d", blow_ptr->number.base);
			if (blow_ptr->number.gain) fprintf(fp, "+");
		}
		if (blow_ptr->number.gain)
		{
			fprintf(fp,"%d/%d", blow_ptr->number.gain, blow_ptr->number.levels);
		}
		fprintf(fp, "\n");
	}
	
	/* Output 'F' for "Flags" */
	emit_flags_32(fp, "F:", blow_ptr->flags1, blow_info_flags1);
	emit_flags_32(fp, "F:", blow_ptr->flags2, blow_info_flags2);

	fprintf(fp, "\n");	
	
	/* Success */
	return (0);
}



/*
 * Emit the "d_info" array into an ascii "template" file
 */
errr emit_d_info_always(FILE *fp, header *head)
{
	int i, n;
	
	for (i = 0; i < z_info->d_max; i++)
	{
		/* Current entry */
		desc_type *d_ptr = (desc_type*)head->info_ptr + i;
		
		bool introduced = FALSE;		
		
		/* Output 'N' for "New/Number/Name" */
		fprintf(fp, "N:%d:%d:%d:%d:%d:%d:%d:%d\n", d_ptr->chart, d_ptr->next,
				d_ptr->branch_on, d_ptr->branch, d_ptr->chance, d_ptr->not_chance,
				d_ptr->level_min, d_ptr->level_max);
	
		/* Output 'A' for "Name (adjective)" */
		if (strlen(head->name_ptr + d_ptr->name1)) fprintf(fp, "A:%s\n", head->name_ptr + d_ptr->name1);
	
		/* Output 'B' for "Name (noun)" */
		if (strlen(head->name_ptr + d_ptr->name2)) fprintf(fp, "B:%s\n", head->name_ptr + d_ptr->name2);
		
		/* Output 'D' for "Description" */
		if (strlen(head->text_ptr + d_ptr->text)) fprintf(fp, "D:%s\n", head->text_ptr + d_ptr->text);
	
		/* Output 'P' for "Placement Flags" */
		emit_flags_32(fp, "P:", d_ptr->p_flag, d_info_pflags);
	
		/* Output 'S' for "Special Flags" */
		emit_flags_32(fp, "S:", d_ptr->flags, d_info_sflags);
	
		/* Output 'P' for "Level Flags" */
		emit_flags_32(fp, "L:", d_ptr->l_flag, d_info_lflags);
	
		/* Output feature */
		if (d_ptr->feat)
		{
			fprintf(fp, "F:%d", d_ptr->feat);
			introduced = TRUE;
		}
	
		/* Not introduced yet */
		if (!introduced)
		{
			for (n = 0; n < 4; n++)
			{
				if (!d_ptr->theme[n]) continue;
				introduced = TRUE;
			}
			
			if (introduced) fprintf(fp, "F:0");
		}
		
		/* Output theme features */
		if (introduced)
		{
			for (n = 0; n < 4; n++)
			{
				if (!introduced) fprintf(fp, "F");
				fprintf(fp, ":%d", d_ptr->theme[n]);
				introduced = TRUE;
			}
		
			/* Terminate */
			fprintf(fp,"\n");
		}
	
		/* Output 'K' for "Kind" */
		if (d_ptr->tval) fprintf(fp, "K:%d:%d:%d\n", d_ptr->tval, d_ptr->min_sval, d_ptr->max_sval);
		
		/* Output 'G' for "Graphics" */
		if (d_ptr->r_char) fprintf(fp, "G:%c\n", d_ptr->r_char);
	
		/* Output 'R' for "Race flag" */
		if ((d_ptr->r_flag > 0) && (d_ptr->r_flag <= 32)) fprintf(fp, "R:%s\n", r_info_flags1[d_ptr->r_flag-1]);
		else if ((d_ptr->r_flag > 32) && (d_ptr->r_flag <= 64)) fprintf(fp, "R:%s\n", r_info_flags1[d_ptr->r_flag-33]);
		else if ((d_ptr->r_flag > 64) && (d_ptr->r_flag <= 96)) fprintf(fp, "R:%s\n", r_info_flags1[d_ptr->r_flag-65]);
		else if ((d_ptr->r_flag > 96) && (d_ptr->r_flag <= 128)) fprintf(fp, "R:%s\n", r_info_flags1[d_ptr->r_flag-97]);
		else if ((d_ptr->r_flag > 128) && (d_ptr->r_flag <= 160)) fprintf(fp, "R:%s\n", r_info_flags1[d_ptr->r_flag-129]);
		else if ((d_ptr->r_flag > 160) && (d_ptr->r_flag <= 192)) fprintf(fp, "R:%s\n", r_info_flags1[d_ptr->r_flag-161]);
		else if ((d_ptr->r_flag > 192) && (d_ptr->r_flag <= 224)) fprintf(fp, "R:%s\n", r_info_flags1[d_ptr->r_flag-193]);
		else if ((d_ptr->r_flag > 224) && (d_ptr->r_flag <= 256)) fprintf(fp, "R:%s\n", r_info_flags1[d_ptr->r_flag-225]);
		else if ((d_ptr->r_flag > 256) && (d_ptr->r_flag <= 288)) fprintf(fp, "R:%s\n", r_info_flags1[d_ptr->r_flag-257]);
	
		fprintf(fp,"\n");

	}
	
	/* Success */
	return (0);	
}



/*
 * Emit the "r_info" array into an ascii "template" file
 */
errr emit_r_info_index(FILE *fp, header *head, int i)
{
	int n;

	/* Used to determine 'free' colours */
	u32b metallics = 0;
	u32b chromatics = 0;
	bool clear = FALSE;
	bool multi = FALSE;
	
	/* Current entry */
	monster_race *r_ptr = (monster_race*)head->info_ptr + i;
	
	/* Output 'N' for "New/Number/Name" */
	fprintf(fp, "N:%d:%s\n", i,head->name_ptr + r_ptr->name);

	/* Output 'G' for "Graphics" (one line only) */
	fprintf(fp, "G:%c:%c\n",r_ptr->d_char,color_attr_to_char(r_ptr->d_attr));
	
	/* Show other monsters with same appearance */
	for (n = 1; n < z_info->r_max; n++)
	{
		monster_race *r2_ptr = (monster_race*)head->info_ptr + n;
		
		if (r_ptr->d_char != r2_ptr->d_char) continue;
		
		if (r2_ptr->flags1 & (RF1_ATTR_CLEAR)) clear = TRUE;
		else if (r2_ptr->flags1 & (RF1_ATTR_MULTI)) multi = TRUE;
		else if (r2_ptr->flags9 & (RF9_ATTR_METAL)) metallics |= 1 << (r2_ptr->d_attr);
		else chromatics |= 1 << (r2_ptr->d_attr);
		
		if (n == i) continue;
		
		/* Town monsters allowed to match dungeon monsters */
		if ((!r2_ptr->level) && (r_ptr->level)) continue;
		if ((!r_ptr->level) && (r2_ptr->level)) continue;
		
		if (((r_ptr->flags1 & (RF1_ATTR_CLEAR)) == 
			(r2_ptr->flags1 & (RF1_ATTR_CLEAR))) &&
			((r_ptr->flags1 & (RF1_ATTR_CLEAR)) ||
					(r2_ptr->flags1 & (RF1_ATTR_CLEAR))))
		{
			fprintf(fp, "#$ Matches %s\n", head->name_ptr + r2_ptr->name);
		}
		else if (((r_ptr->flags1 & (RF1_ATTR_MULTI)) == 
			(r2_ptr->flags1 & (RF1_ATTR_MULTI))) &&
			((r_ptr->flags1 & (RF1_ATTR_MULTI)) ||
					(r2_ptr->flags1 & (RF1_ATTR_MULTI))))
		{
			fprintf(fp, "#$ Matches %s\n", head->name_ptr + r2_ptr->name);
		}
		else if ((r_ptr->d_attr == r2_ptr->d_attr) &&
				((r_ptr->flags9 & (RF9_ATTR_METAL)) == 
								(r2_ptr->flags9 & (RF9_ATTR_METAL))) &&
								((r_ptr->flags1 & (RF1_ATTR_MULTI)) == 
											(r2_ptr->flags1 & (RF1_ATTR_MULTI))) &&
											((r_ptr->flags1 & (RF1_ATTR_CLEAR)) == 
														(r2_ptr->flags1 & (RF1_ATTR_CLEAR))))
		{
			fprintf(fp, "#$ Matches %s\n", head->name_ptr + r2_ptr->name);
		}
	}
	
	/* Display free colours */
	if (!clear) fprintf(fp, "#$ Clear available\n");
	if (!multi) fprintf(fp, "#$ Multihued available\n");
	
	fprintf(fp, "#$ Metallics available:");
	for (i = 0; i < MAX_COLORS; i++)
	{
		if ((metallics & (1 << i)) == 0) fprintf(fp, "%c",color_table[i].index_char);
	}
	fprintf(fp, "\n");
	
	fprintf(fp, "#$ Colors available:");
	for (i = 0; i < MAX_COLORS; i++)
	{
		if ((chromatics & (1 << i)) == 0) fprintf(fp, "%c",color_table[i].index_char);
	}
	fprintf(fp, "\n");
	
	/* Output 'I' for "Info" (one line only) */
	fprintf(fp, "I:%d:%dd%d:%d:%d:%d\n",r_ptr->speed,r_ptr->hdice,r_ptr->hside,r_ptr->aaf,r_ptr->ac,r_ptr->sleep);

	/* Output 'W' for "More Info" (one line only) */
	fprintf(fp,"W:%d:%d:%d:%ld\n",r_ptr->level, r_ptr->rarity, r_ptr->grp_idx, r_ptr->mexp);

	/* Output 'M' for "Magic Info" (one line only) */
	fprintf(fp, "M:%d:%d:%d:%d\n",r_ptr->freq_innate, r_ptr->freq_spell, r_ptr->spell_power, r_ptr->mana);

	/* Output 'B' for "Blows" (up to four lines) */
	for (n = 0; n < 4; n++)
	{
		/* End of blows */
		if (!r_ptr->blow[n].method) break;

		/* Output blow method */		
		fprintf(fp, "B:%s", r_info_blow_method[r_ptr->blow[n].method]);
		
		/* Output blow effect */
		if (r_ptr->blow[n].effect)
		{
			fprintf(fp, ":%s", r_info_blow_effect[r_ptr->blow[n].effect]);
			
			/* Output blow damage if required */
			if ((r_ptr->blow[n].d_dice) && (r_ptr->blow[n].d_side))
			{
				fprintf(fp, ":%dd%d", r_ptr->blow[n].d_dice, r_ptr->blow[n].d_side);
			}
		}
		
		/* End line */
		fprintf(fp, "\n");
	}

	/* Output 'F' for "Flags" */
	emit_flags_32(fp, "F:", r_ptr->flags1, r_info_flags1);
	emit_flags_32(fp, "F:", r_ptr->flags2, r_info_flags2);
	emit_flags_32(fp, "F:", r_ptr->flags3, r_info_flags3);
	emit_flags_32(fp, "F:", r_ptr->flags8, r_info_flags8);
	emit_flags_32(fp, "F:", r_ptr->flags9, r_info_flags9);

	/* Output 'S' for "Spell Flags" (multiple lines) */
	emit_flags_32(fp, "S:", r_ptr->flags4, r_info_flags4);
	emit_flags_32(fp, "S:", r_ptr->flags5, r_info_flags5);
	emit_flags_32(fp, "S:", r_ptr->flags6, r_info_flags6);
	emit_flags_32(fp, "S:", r_ptr->flags7, r_info_flags7);

	/* Output 'D' for "Description" */
	emit_desc(fp, "D:", head->text_ptr + r_ptr->text);

	fprintf(fp,"\n");

	/* Success */
	return (0);	
}


/*
 * Emit the "f_info" array into an ascii "template" file
 */
errr emit_f_info_index(FILE *fp, header *head, int i)
{
	int n;

	/* Current entry */
	feature_type *f_ptr = (feature_type*)head->info_ptr + i;
	feature_type *f2_ptr;
	
	/* Output 'N' for "New/Number/Name" */
	fprintf(fp, "N:%d:%s\n", i,head->name_ptr + f_ptr->name);

	/* Output 'G' for "Graphics" (one line only) */
	fprintf(fp, "G:%c:%c\n",f_ptr->d_char,color_attr_to_char(f_ptr->d_attr));

	/* Output 'M' for "Mimic" (one line only) */
	if (f_ptr->mimic != i)
	{
		fprintf(fp, "M:%d\n", f_ptr->mimic);
		f2_ptr = (feature_type*)head->info_ptr + f_ptr->mimic;
		fprintf(fp, "#$ %s\n",head->name_ptr + f2_ptr->name);
	}

	/* Output 'U' for "Unseen" (one line only) */
	if (f_ptr->unseen != i)
	{
		fprintf(fp, "U:%d\n",f_ptr->unseen);
		f2_ptr = (feature_type*)head->info_ptr + f_ptr->unseen;
		fprintf(fp, "#$ %s\n",head->name_ptr + f2_ptr->name);
	}

	/* Output 'O' for "Object" (one line only) */
	fprintf(fp, "O:%d\n",f_ptr->k_idx);

	/* Output 'W' for "More Info" (one line only) */
	fprintf(fp,"W:%d:%d:%d:%d\n",f_ptr->level, f_ptr->rarity, f_ptr->priority, f_ptr->edge);
		f2_ptr = (feature_type*)head->info_ptr + f_ptr->edge;
		if (f_ptr->edge) fprintf(fp, "#$ %s\n",head->name_ptr + f2_ptr->name);

	/* Output 'T' for "Trap" (one line only) */
	if (f_ptr->blow.method)
	{
		/* Output blow method */		
		fprintf(fp, "T:%s", r_info_blow_method[f_ptr->blow.method]);
		
		/* Output blow effect */
		if (f_ptr->blow.effect)
		{
			fprintf(fp, ":%s", r_info_blow_effect[f_ptr->blow.effect]);
			
			/* Output blow damage if required */
			if ((f_ptr->blow.d_dice) && (f_ptr->blow.d_side))
			{
				fprintf(fp, ":%dd%d", f_ptr->blow.d_dice, f_ptr->blow.d_side);
			}
		}
		
		/* End line */
		fprintf(fp, "\n");
	}

	/* Output 'S' for "Spell" (one line only) */
	if (f_ptr->spell)
	{
		fprintf(fp,"S:%s\n", f_ptr->spell < 33 ? r_info_flags4[f_ptr->spell - 1] :
							 (f_ptr->spell < 65 ? r_info_flags5[f_ptr->spell - 33] :
							 (f_ptr->spell < 129 ? r_info_flags6[f_ptr->spell - 65] :
							 (f_ptr->spell < 161 ? r_info_flags7[f_ptr->spell - 129] : "Error"))));		
	}

	/* Output 'K' for "Transitions" (up to 8 lines, plus default) */
	if (f_ptr->defaults)
	{
		fprintf(fp,"K:DEFAULT:%d:%d\n",f_ptr->defaults, f_ptr->power);
		f2_ptr = (feature_type*)head->info_ptr + f_ptr->mimic;
		if (f_ptr->defaults != i) fprintf(fp, "#$ %s\n",head->name_ptr + f2_ptr->name);
	}

	for (n = 0; n < MAX_FEAT_STATES; n++)
	{
 		if (f_ptr->state[n].action == FS_FLAGS_END) break;
 		if (f_ptr->state[n].result == i) continue;

		fprintf(fp,"K:%s:%d:%d\n", f_ptr->state[n].action < 32 ? f_info_flags1[f_ptr->state[n].action] :
							 (f_ptr->state[n].action < 64 ? f_info_flags2[f_ptr->state[n].action - 32] :
							 (f_ptr->state[n].action < 128 ? f_info_flags3[f_ptr->state[n].action - 64] : "Error"))
							 ,f_ptr->state[n].result, f_ptr->power);
		f2_ptr = (feature_type*)head->info_ptr + f_ptr->state[n].result;
		fprintf(fp, "#$ %s\n",head->name_ptr + f2_ptr->name);
	}

	/* Output 'F' for "Flags" */
	emit_flags_32(fp, "F:", f_ptr->flags1, f_info_flags1);
	emit_flags_32(fp, "F:", f_ptr->flags2, f_info_flags2);
	emit_flags_32(fp, "F:", f_ptr->flags3, f_info_flags3);

	/* Output 'D' for "Description" */
	emit_desc(fp, "D:", head->text_ptr + f_ptr->text);

	fprintf(fp,"\n");

	/* Success */
	return (0);	
}


/*
 * Emit the "k_info" array into an ascii "template" file
 */
errr emit_k_info_index(FILE *fp, header *head, int i)
{
	int n;
	bool introduced = FALSE;

	/* Current entry */
	object_kind *k_ptr = (object_kind*)head->info_ptr + i;
	
	/* Output 'N' for "New/Number/Name" */
	fprintf(fp, "N:%d:%s\n", i,head->name_ptr + k_ptr->name);

	/* Output 'G' for "Graphics" (one line only) */
	fprintf(fp, "G:%c:%c\n",k_ptr->d_char,color_attr_to_char(k_ptr->d_attr));

	/* Output 'I' for "Info" (one line only) */
	fprintf(fp, "I:%d:%d:%d\n",k_ptr->tval,k_ptr->sval,k_ptr->pval);

	/* Output 'W' for "More Info" (one line only) */
	fprintf(fp,"W:%d:%d:%d:%ld\n",k_ptr->level, k_ptr->charges, k_ptr->weight, k_ptr->cost);

	/* Output allocation */
	for (n = 0; n < 4; n++)
	{
		if (!k_ptr->chance[n]) continue;
		
		if (!introduced) fprintf(fp, "A");
		fprintf(fp, ":%d/%d", k_ptr->locale[n], k_ptr->chance[n]);
		introduced = TRUE;
	}
	
	/* Terminate */
	if (introduced) fprintf(fp,"\n");
	
	/* Output 'P' for "Power" (one line only) */
	fprintf(fp,"P:%d:%dd%d:%d:%d:%d\n",k_ptr->ac, k_ptr->dd, k_ptr->ds, k_ptr->to_h, k_ptr->to_d, k_ptr->to_a);

	/* Output 'Y' for "Runes" (up to one line only) */
	if (k_ptr->runest) fprintf(fp, "Y:%d:%d\n",k_ptr->runest, k_ptr->runesc);
	else if (k_ptr->flavor) fprintf(fp, "# Runes needed\n");

	/* Output 'F' for "Flags" */
	emit_flags_32(fp, "F:", k_ptr->flags1, k_info_flags1);
	emit_flags_32(fp, "F:", k_ptr->flags2, k_info_flags2);
	emit_flags_32(fp, "F:", k_ptr->flags3, k_info_flags3);
	emit_flags_32(fp, "F:", k_ptr->flags4, k_info_flags4);

	/* Output 'D' for "Description" */
	emit_desc(fp, "D:", head->text_ptr + k_ptr->text);

	fprintf(fp,"\n");

	/* Success */
	return (0);	
}


/*
 * Emit the "a_info" array into an ascii "template" file
 */
errr emit_a_info_index(FILE *fp, header *head, int i)
{
	/* Current entry */
	artifact_type *a_ptr = (artifact_type*)head->info_ptr + i;
	
	/* Output 'N' for "New/Number/Name" */
	fprintf(fp, "N:%d:%s\n", i,head->name_ptr + a_ptr->name);

	/* Output 'I' for "Info" (one line only) */
	fprintf(fp, "I:%d:%d:%d\n",a_ptr->tval,a_ptr->sval,a_ptr->pval);

	/* Output 'W' for "More Info" (one line only) */
	fprintf(fp,"W:%d:%d:%d:%ld\n",a_ptr->level, a_ptr->rarity, a_ptr->weight, a_ptr->cost);
	
	/* Output 'P' for "Power" (one line only) */
	fprintf(fp,"P:%d:%dd%d:%d:%d:%d\n",a_ptr->ac, a_ptr->dd, a_ptr->ds, a_ptr->to_h, a_ptr->to_d, a_ptr->to_a);

	/* Output 'A' for "Activation" (one line only) */
	if (a_ptr->activation) fprintf(fp, "A:%d:%d:%d\n",a_ptr->activation,a_ptr->time,a_ptr->randtime);

	/* Output 'F' for "Flags" */
	emit_flags_32(fp, "F:", a_ptr->flags1, k_info_flags1);
	emit_flags_32(fp, "F:", a_ptr->flags2, k_info_flags2);
	emit_flags_32(fp, "F:", a_ptr->flags3, k_info_flags3);
	emit_flags_32(fp, "F:", a_ptr->flags4, k_info_flags4);

	/* Output 'D' for "Description" */
	emit_desc(fp, "D:", head->text_ptr + a_ptr->text);

	fprintf(fp,"\n");

	/* Success */
	return (0);	
}


/*
 * Emit the "e_info" array into an ascii "template" file
 */
errr emit_e_info_index(FILE *fp, header *head, int i)
{
	int n;
	/* Current entry */
	ego_item_type *e_ptr = (ego_item_type*)head->info_ptr + i;
	
	/* Output 'N' for "New/Number/Name" */
	fprintf(fp, "N:%d:%s\n", i,head->name_ptr + e_ptr->name);

	/* Output 'W' for "More Info" (one line only) */
	fprintf(fp,"W:%d:%d:0:%ld\n",e_ptr->level, e_ptr->rarity, e_ptr->cost);

	/* Output 'X' for "Extra Info" (one line only) */
	fprintf(fp,"X:%d:%d:%d\n",e_ptr->slot, e_ptr->rating, e_ptr->xtra);

	/* Output allocation */
	for (n = 0; n < 3; n++)
	{
		if (!e_ptr->tval[n]) continue;

		fprintf(fp, "T:%d:%d:%d\n", e_ptr->tval[n], e_ptr->min_sval[n], e_ptr->max_sval[n]);
	}
	
	/* Output 'C' for "Creation" (one line only) */
	fprintf(fp,"C:%d:%d:%d:%d\n",e_ptr->max_to_h, e_ptr->max_to_d, e_ptr->max_to_a, e_ptr->max_pval);

	/* Output 'A' for "Activation" (one line only) */
	if (e_ptr->activation) fprintf(fp, "A:%d:%d:%d\n",e_ptr->activation,e_ptr->time,e_ptr->randtime);

	/* Output 'Y' for "Runes" (up to one line only) */
	if (e_ptr->runest) fprintf(fp, "Y:%d:%d\n",e_ptr->runest, e_ptr->runesc);
	else fprintf(fp, "# Runes needed\n");

	/* Output 'F' for "Flags" */
	emit_flags_32(fp, "F:", e_ptr->flags1, k_info_flags1);
	emit_flags_32(fp, "F:", e_ptr->flags2, k_info_flags2);
	emit_flags_32(fp, "F:", e_ptr->flags3, k_info_flags3);
	emit_flags_32(fp, "F:", e_ptr->flags4, k_info_flags4);

	/* Output 'O' for "Obvious Flags" */
	emit_flags_32(fp, "O:", e_ptr->obv_flags1, k_info_flags1);
	emit_flags_32(fp, "O:", e_ptr->obv_flags2, k_info_flags2);
	emit_flags_32(fp, "O:", e_ptr->obv_flags3, k_info_flags3);
	emit_flags_32(fp, "O:", e_ptr->obv_flags4, k_info_flags4);
	
	/* Output 'D' for "Description" */
	/*emit_desc(fp, "D:", head->text_ptr + e_ptr->text);*/

	fprintf(fp,"\n");

	/* Success */
	return (0);	
}


/*
 * Emit the "x_info" array into an ascii "template" file
 */
errr emit_x_info_index(FILE *fp, header *head, int i)
{
	/* Current entry */
	flavor_type *x_ptr = (flavor_type*)head->info_ptr + i;
	
	/* Output 'N' for "New/Number/Name" */
	fprintf(fp, "N:%d:%d%s\n", i,x_ptr->tval, x_ptr->sval == 255 ? "" : format(":%d", x_ptr->sval));

	/* Output 'G' for "Graphics" (one line only) */
	fprintf(fp, "G:%c:%c\n",x_ptr->d_char,color_attr_to_char(x_ptr->d_attr));

	/* Output 'D' for "Description" */
	emit_desc(fp, "D:", head->text_ptr + x_ptr->text);

	fprintf(fp,"\n");

	/* Success */
	return (0);	
}


/*
 * Emit the "p_info" array into an ascii "template" file
 */
errr emit_p_info_index(FILE *fp, header *head, int i)
{
	int n;
	bool introduced = FALSE;
	bool output = FALSE;
	
	/* Current entry */
	player_race *pr_ptr = (player_race*)head->info_ptr + i;
	
	/* Output 'N' for "New/Number/Name" */
	fprintf(fp, "N:%d:%s\n", i,head->name_ptr + pr_ptr->name);

	/* Output 'D' for "Description" */
	emit_desc(fp, "D:", head->text_ptr + pr_ptr->text);

	/* Start stats output */
	fprintf(fp, "S");
	
	/* Output stats */
	for (n = 0; n < A_MAX; n++)
	{
		fprintf(fp, ":%d", pr_ptr->r_adj[n]);
	}
	
	/* Finish stats output */
	fprintf(fp, "\n");
	
	/* Output 'R' for "Racial Skills" (one line only) */
	fprintf(fp, "R:%d:%d:%d:%d:%d:%d:%d:%d:%d\n",
		    pr_ptr->r_dis, pr_ptr->r_dev, pr_ptr->r_sav, pr_ptr->r_stl,
		    pr_ptr->r_srh, pr_ptr->r_dig, pr_ptr->r_tht, pr_ptr->r_thn, pr_ptr->r_thb);
	
	/* Output 'X' for "Extra Info" (one line only) */
	if (pr_ptr->r_exp || pr_ptr->infra || pr_ptr->r_idx || pr_ptr->slots[END_EQUIPMENT - INVEN_WIELD])
	{
		fprintf(fp, "X:%d:%d:%d:%d\n", pr_ptr->r_exp, pr_ptr->infra, pr_ptr->r_idx, pr_ptr->slots[END_EQUIPMENT - INVEN_WIELD]);
	}

	/* Output 'I' for "Info" (one line only) */	
	if (pr_ptr->hist || pr_ptr->b_age || pr_ptr->m_age || pr_ptr->home)
	{
		fprintf(fp, "I:%d:%d:%d:%d\n", pr_ptr->hist, pr_ptr->b_age, pr_ptr->m_age, pr_ptr->home);
	}

	/* Output 'H' for "Height" (one line only) */
	if  (pr_ptr->m_b_ht || pr_ptr->m_m_ht || pr_ptr->f_b_ht || pr_ptr->f_m_ht)
	{
		fprintf(fp, "H:%d:%d:%d:%d\n", pr_ptr->m_b_ht, pr_ptr->m_m_ht, pr_ptr->f_b_ht, pr_ptr->f_m_ht);
	}
		
	/* Output 'W' for "Weight" (one line only) */
	if (pr_ptr->m_b_wt || pr_ptr->f_b_wt)
	{
		fprintf(fp, "W:%d:%d\n", pr_ptr->m_b_wt, pr_ptr->f_b_wt);
	}

	/* Output 'F' for "Flags" */
	emit_flags_32(fp, "F:", pr_ptr->flags1, k_info_flags1);
	emit_flags_32(fp, "F:", pr_ptr->flags2, k_info_flags2);
	emit_flags_32(fp, "F:", pr_ptr->flags3, k_info_flags3);
	emit_flags_32(fp, "F:", pr_ptr->flags4, k_info_flags4);

	/* Check if 'O' required */
	for (n = 0; n < END_EQUIPMENT - INVEN_WIELD; n++)
	{
		if (pr_ptr->slots[n]) output = TRUE;
	}
	
	if (output)
	{
		/* Start object slots output */
		fprintf(fp, "O");
		
		/* Output stats */
		for (n = 0; n < END_EQUIPMENT - INVEN_WIELD; n++)
		{
			fprintf(fp, ":%d", pr_ptr->slots[n]);
		}
		
		/* Finish object slots output */
		fprintf(fp, "\n");
	}

	/* Only output classes for starting races */
	if (i< z_info->g_max)
	{
		/* Start class choices output */
		fprintf(fp, "C:");
		
		/* Output class choices */
		for (n = 0; n < z_info->c_max; n++)
		{			
			if (pr_ptr->choice & (1 << n))
			{
				if (introduced) fprintf(fp, "|");
				fprintf(fp, "%d", n);
				introduced = TRUE;
			}
		}
		
		/* Finish class choices output */
		fprintf(fp, "\n");

	}
	
	/* Finish output */
	fprintf(fp, "\n");

	/* Success */
	return (0);	
}



/*
 * Emit the "c_info" array into an ascii "template" file
 */
errr emit_c_info_index(FILE *fp, header *head, int i)
{
	int n;

	/* Current entry */
	player_class *pc_ptr = (player_class*)head->info_ptr + i;
	
	/* Output 'N' for "New/Number/Name" */
	fprintf(fp, "N:%d:%s\n", i,head->name_ptr + pc_ptr->name);

	/* Start stats output */
	fprintf(fp, "S");
	
	/* Output stats */
	for (n = 0; n < A_MAX; n++)
	{
		fprintf(fp, ":%d", pc_ptr->c_adj[n]);
	}
	
	/* Finish stats output */
	fprintf(fp, "\n");
	
	/* Output 'C' for "Class Skills" (one line only) */
	fprintf(fp, "C:%d:%d:%d:%d:%d:%d:%d:%d:%d\n",
		    pc_ptr->c_dis, pc_ptr->c_dev, pc_ptr->c_sav, pc_ptr->c_stl,
		    pc_ptr->c_srh, pc_ptr->c_dig, pc_ptr->c_tht, pc_ptr->c_thn, pc_ptr->c_thb);

	/* Output 'X' for "eXtra Skills" (one line only) */
	fprintf(fp, "X:%d:%d:%d:%d:%d:%d:%d:%d:%d\n",
		    pc_ptr->x_dis, pc_ptr->x_dev, pc_ptr->x_sav, pc_ptr->x_stl,
		    pc_ptr->x_srh, pc_ptr->x_dig, pc_ptr->x_tht, pc_ptr->x_thn, pc_ptr->x_thb);
	
	/* Output 'I' for "Info" (one line only) */
	fprintf(fp, "I:%d:%ld:%d:%d:%d\n", pc_ptr->c_exp, pc_ptr->sense_base, pc_ptr->sense_div, pc_ptr->sense_type, pc_ptr->sense_squared);

	/* Output 'A' for "Attack Info" (one line only) */
	fprintf(fp, "A:%d:%d:%d:%d\n", pc_ptr->max_attacks, pc_ptr->min_weight, pc_ptr->att_multiply, pc_ptr->chg_weight);

	/* Output 'M' for "Magic Info" (one line only) */
	fprintf(fp, "M:%d:%d:%d:%d:%d:%d:%d\n",
		pc_ptr->spell_book, pc_ptr->spell_stat_study, pc_ptr->spell_stat_mana, pc_ptr->spell_stat_fail,
		pc_ptr->spell_first, pc_ptr->spell_weight, pc_ptr->spell_power);

	/* Output titles */
	for (n = 0; n < PY_MAX_LEVEL / 5; n++)
	{
		fprintf(fp, "T:%s\n", head->text_ptr + pc_ptr->title[n]);
	}
	
	/* Output starting equipment */
	for (n = 0; n < MAX_CLASS_ITEMS; n++)
	{
		if (!pc_ptr->start_items[n].tval) continue;
		
		fprintf(fp, "E:%d:%d:%d:%d:%d:%d:%d:%d\n",
			    pc_ptr->start_items[n].tval, pc_ptr->start_items[n].sval,
			    pc_ptr->start_items[n].number_min, pc_ptr->start_items[n].number_max,
			    pc_ptr->start_items[n].charge_min, pc_ptr->start_items[n].charge_max,
			    pc_ptr->start_items[n].social_min, pc_ptr->start_items[n].social_max);
	}

	fprintf(fp,"\n");
	
	/* Success */
	return (0);
}


/*
 * Emit the "s_info" array into an ascii "template" file
 */
errr emit_s_info_index(FILE *fp, header *head, int i)
{
	int n, l;
	bool mage_spell = FALSE;
	bool priest_spell = FALSE;

	/* Current entry */
	spell_type *s_ptr = (spell_type*)head->info_ptr + i;
	
	/* Output 'N' for "New/Number/Name" */
	fprintf(fp, "N:%d:%s\n", i,head->name_ptr + s_ptr->name);

	/* Output appears in */
	for (n = 0; n < MAX_SPELL_APPEARS; n++)
	{
		if (!s_ptr->appears[n].tval) continue;

		fprintf(fp, "A:%d:%d:%d\n", s_ptr->appears[n].tval, s_ptr->appears[n].sval,
				s_ptr->appears[n].slot);
	}

	/* Reorder spells in class order */
	for (l = 0; l < z_info->c_max; l++)
	{
		bool class_spell, note_spell;
		
		class_spell = FALSE;
		note_spell = FALSE;
		
		/* Output casters */
		for (n = 0; n < MAX_SPELL_CASTERS; n++)
		{
			if (!s_ptr->cast[n].class) continue;
			
			if (s_ptr->cast[n].class != l) continue;
	
			fprintf(fp, "C:%d:%d:%d:%d:%d\n",
					s_ptr->cast[n].class,s_ptr->cast[n].level,s_ptr->cast[n].mana,s_ptr->cast[n].fail,s_ptr->cast[n].min);
			
			class_spell = TRUE;
			
			if (s_ptr->cast[n].class == 1) mage_spell = TRUE;
			if (s_ptr->cast[n].class == 2) priest_spell = TRUE;
		}
		
		/* Hack -- note when sub-classes cannot cast spell */
		if (!class_spell)
		{
			switch (l)
			{
				case 3: case 4: case 10: note_spell = mage_spell; break;
				case 5: note_spell = priest_spell; break;
				case 8: case 9: case 11: note_spell = (s_ptr->cast[0].class != 0); break;
			}
#if 0 /* This causes dangerous memory access on some OS'es and needs p_class initialised first. */			
			if (note_spell)
			{
				fprintf(fp, "#$ %s cannot cast this spell.\n", c_name + c_info[l].name);
			}
#endif
		}
	}
	
	/* Output 'P' for "Pre-requisites" (one line only) */
	fprintf(fp,"P:%d:%d\n",s_ptr->preq[0], s_ptr->preq[1]);

	/* Output blows */
	for (n = 0; n < 4; n++)
	{
		if (!s_ptr->blow[n].method) continue;

		fprintf(fp, "B:%s", r_info_blow_method[s_ptr->blow[n].method]);
		
		if (s_ptr->blow[n].effect)
		{
			fprintf(fp, ":%s", r_info_blow_effect[s_ptr->blow[n].effect]);

			if ((s_ptr->blow[n].d_dice != 0) || (s_ptr->blow[n].d_side != 0))
			{
				fprintf(fp, ":%dd%d", s_ptr->blow[n].d_dice, s_ptr->blow[n].d_side);
			}

			if ((s_ptr->blow[n].d_plus != 0) || (s_ptr->blow[n].l_side != 0) || (s_ptr->blow[n].l_dice != 0)
					|| (s_ptr->blow[n].levels != 0))
			{
				if ((s_ptr->blow[n].d_dice == 0) && (s_ptr->blow[n].d_side == 0))
				{
					fprintf(fp, ":");
				}
				else if (s_ptr->blow[n].d_plus)
				{
					fprintf(fp, "+");
				}

				if (s_ptr->blow[n].d_plus)
				{
					fprintf(fp, "%d", s_ptr->blow[n].d_plus);
				}

				if (((s_ptr->blow[n].d_dice != 0) || (s_ptr->blow[n].d_side != 0) || (s_ptr->blow[n].d_plus))
					&& ((s_ptr->blow[n].l_dice != 0) || (s_ptr->blow[n].l_side != 0) || (s_ptr->blow[n].l_plus)))
				{
					fprintf(fp, "+");
				}
				
				if (((s_ptr->blow[n].d_dice != 0) || (s_ptr->blow[n].d_side != 0)) && ((s_ptr->blow[n].l_dice != 0) || (s_ptr->blow[n].l_side != 0)))
				{
					fprintf(fp, "(");
				}
				
				if ((s_ptr->blow[n].l_dice != 0) || (s_ptr->blow[n].l_side != 0))
				{
					fprintf(fp, "%dd%d", s_ptr->blow[n].l_dice, s_ptr->blow[n].l_side);
					
					if (s_ptr->blow[n].l_plus != 0)
					{
						fprintf(fp, "+");
					}
				}
				
				if (s_ptr->blow[n].l_plus != 0)
				{
					fprintf(fp, "%d", s_ptr->blow[n].l_plus);
				}				
				
				if (((s_ptr->blow[n].d_dice != 0) || (s_ptr->blow[n].d_side != 0)) && ((s_ptr->blow[n].l_dice != 0) || (s_ptr->blow[n].l_side != 0)))
				{
					fprintf(fp, ")");
				}

				if (s_ptr->blow[n].levels != 0)
				{
					fprintf(fp, "/%d", s_ptr->blow[n].levels);
				}
			}			
		}
		
		fprintf(fp, "\n");
	}

	/* Output 'F' for "Flags" */
	emit_flags_32(fp, "F:", s_ptr->flags1, s_info_flags1);
	emit_flags_32(fp, "F:", s_ptr->flags2, s_info_flags2);
	emit_flags_32(fp, "F:", s_ptr->flags3, s_info_flags3);

	/* Output 'S' for 'Spell types' */
	if (s_ptr->type)
	{
		fprintf(fp, "S:%s:%d\n", s_info_types[s_ptr->type], s_ptr->param);
	}
	
	/* Output 'L' for lasts */
	if (s_ptr->l_dice || s_ptr->l_side || s_ptr->l_plus)
	{
		fprintf(fp, "L:%dd%d+%d\n", 
				s_ptr->l_dice, s_ptr->l_side, s_ptr->l_plus);
	}
	
	/* Output 'D' for "Description" */
	emit_desc(fp, "D:", head->text_ptr + s_ptr->text);

	fprintf(fp,"\n");

	/* Success */
	return (0);	
}



#endif /* ALLOW_TEMPLATES_OUTPUT */



#else	/* ALLOW_TEMPLATES */

#ifdef MACINTOSH
static int i = 0;
#endif

#endif	/* ALLOW_TEMPLATES */
