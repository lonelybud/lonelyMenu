#pragma once

#include "gta/joaat.hpp"
#include "natives.hpp"

enum Boats : Hash
{
	VEHICLE_DINGHY       = "DINGHY"_J,
	VEHICLE_DINGHY2      = "DINGHY2"_J,
	VEHICLE_DINGHY3      = "DINGHY3"_J,
	VEHICLE_DINGHY4      = "DINGHY4"_J,
	VEHICLE_JETMAX       = "JETMAX"_J,
	VEHICLE_MARQUIS      = "MARQUIS"_J,
	VEHICLE_SEASHARK     = "SEASHARK"_J,
	VEHICLE_SEASHARK2    = "SEASHARK2"_J,
	VEHICLE_SEASHARK3    = "SEASHARK3"_J,
	VEHICLE_SPEEDER      = "SPEEDER"_J,
	VEHICLE_SPEEDER2     = "SPEEDER2"_J,
	VEHICLE_SQUALO       = "SQUALO"_J,
	VEHICLE_SUBMERSIBLE  = "SUBMERSIBLE"_J,
	VEHICLE_SUBMERSIBLE2 = "SUBMERSIBLE2"_J,
	VEHICLE_SUNTRAP      = "SUNTRAP"_J,
	VEHICLE_TORO         = "TORO"_J,
	VEHICLE_TORO2        = "TORO2"_J,
	VEHICLE_TROPIC       = "TROPIC"_J,
	VEHICLE_TROPIC2      = "TROPIC2"_J,
	VEHICLE_TUG          = "TUG"_J,
};

enum Commericals : Hash
{
	VEHICLE_BENSON    = "BENSON"_J,
	VEHICLE_BIFF      = "BIFF"_J,
	VEHICLE_HAULER    = "HAULER"_J,
	VEHICLE_HAULER2   = "HAULER2"_J,
	VEHICLE_MULE      = "MULE"_J,
	VEHICLE_MULE2     = "MULE2"_J,
	VEHICLE_MULE3     = "MULE3"_J,
	VEHICLE_MULE4     = "MULE4"_J,
	VEHICLE_PACKER    = "PACKER"_J,
	VEHICLE_PHANTOM   = "PHANTOM"_J,
	VEHICLE_PHANTOM2  = "PHANTOM2"_J,
	VEHICLE_PHANTOM3  = "PHANTOM3"_J,
	VEHICLE_POUNDER   = "POUNDER"_J,
	VEHICLE_POUNDER2  = "POUNDER2"_J,
	VEHICLE_STOCKADE  = "STOCKADE"_J,
	VEHICLE_STOCKADE3 = "STOCKADE3"_J,
	VEHICLE_TERBYTE   = "TERBYTE"_J,
};

enum Compacts : Hash
{
	VEHICLE_BLISTA      = "BLISTA"_J,
	VEHICLE_BRIOSO      = "BRIOSO"_J,
	VEHICLE_DILETTANTE  = "DILETTANTE"_J,
	VEHICLE_DILETTANTE2 = "DILETTANTE2"_J,
	VEHICLE_ISSI2       = "ISSI2"_J,
	VEHICLE_ISSI3       = "ISSI3"_J,
	VEHICLE_PANTO       = "PANTO"_J,
	VEHICLE_PRAIRIE     = "PRAIRIE"_J,
	VEHICLE_RHAPSODY    = "RHAPSODY"_J,
};

enum Coupes : Hash
{
	VEHICLE_COGCABRIO = "COGCABRIO"_J,
	VEHICLE_EXEMPLAR  = "EXEMPLAR"_J,
	VEHICLE_F620      = "F620"_J,
	VEHICLE_FELON     = "FELON"_J,
	VEHICLE_FELON2    = "FELON2"_J,
	VEHICLE_JACKAL    = "JACKAL"_J,
	VEHICLE_ORACLE    = "ORACLE"_J,
	VEHICLE_ORACLE2   = "ORACLE2"_J,
	VEHICLE_SENTINEL  = "SENTINEL"_J,
	VEHICLE_SENTINEL2 = "SENTINEL2"_J,
	VEHICLE_WINDSOR   = "WINDSOR"_J,
	VEHICLE_WINDSOR2  = "WINDSOR2"_J,
	VEHICLE_ZION      = "ZION"_J,
	VEHICLE_ZION2     = "ZION2"_J,
};

enum Bikes : Hash
{
	VEHICLE_BMX      = "BMX"_J,
	VEHICLE_CRUISER  = "CRUISER"_J,
	VEHICLE_FIXTER   = "FIXTER"_J,
	VEHICLE_SCORCHER = "SCORCHER"_J,
	VEHICLE_TRIBIKE  = "TRIBIKE"_J,
	VEHICLE_TRIBIKE2 = "TRIBIKE2"_J,
	VEHICLE_TRIBIKE3 = "TRIBIKE3"_J,
};

enum Emergency : Hash
{
	VEHICLE_AMBULANCE  = "AMBULANCE"_J,
	VEHICLE_FBI        = "FBI"_J,
	VEHICLE_FBI2       = "FBI2"_J,
	VEHICLE_FIRETRUK   = "FIRETRUK"_J,
	VEHICLE_LGUARD     = "LGUARD"_J,
	VEHICLE_PBUS       = "PBUS"_J,
	VEHICLE_POLICE     = "POLICE"_J,
	VEHICLE_POLICE2    = "POLICE2"_J,
	VEHICLE_POLICE3    = "POLICE3"_J,
	VEHICLE_POLICE4    = "POLICE4"_J,
	VEHICLE_POLICEB    = "POLICEB"_J,
	VEHICLE_POLMAV     = "POLMAV"_J,
	VEHICLE_POLICEOLD1 = "POLICEOLD1"_J,
	VEHICLE_POLICEOLD2 = "POLICEOLD2"_J,
	VEHICLE_POLICET    = "POLICET"_J,
	VEHICLE_PRANGER    = "PRANGER"_J,
	VEHICLE_PREDATOR   = "PREDATOR"_J,
	VEHICLE_RIOT       = "RIOT"_J,
	VEHICLE_RIOT2      = "RIOT2"_J,
	VEHICLE_SHERIFF    = "SHERIFF"_J,
	VEHICLE_SHERIFF2   = "SHERIFF2"_J,
};

enum Helicopters : Hash
{
	VEHICLE_AKULA        = "AKULA"_J,
	VEHICLE_ANNIHILATOR  = "ANNIHILATOR"_J,
	VEHICLE_BUZZARD      = "BUZZARD"_J,
	VEHICLE_BUZZARD2     = "BUZZARD2"_J,
	VEHICLE_CARGOBOB     = "CARGOBOB"_J,
	VEHICLE_CARGOBOB2    = "CARGOBOB2"_J,
	VEHICLE_CARGOBOB3    = "CARGOBOB3"_J,
	VEHICLE_CARGOBOB4    = "CARGOBOB4"_J,
	VEHICLE_FROGGER      = "FROGGER"_J,
	VEHICLE_FROGGER2     = "FROGGER2"_J,
	VEHICLE_HAVOK        = "HAVOK"_J,
	VEHICLE_HUNTER       = "HUNTER"_J,
	VEHICLE_MAVERICK     = "MAVERICK"_J,
	VEHICLE_SAVAGE       = "SAVAGE"_J,
	VEHICLE_SKYLIFT      = "SKYLIFT"_J,
	VEHICLE_SUPERVOLITO  = "SUPERVOLITO"_J,
	VEHICLE_SUPERVOLITO2 = "SUPERVOLITO2"_J,
	VEHICLE_SWIFT        = "SWIFT"_J,
	VEHICLE_SWIFT2       = "SWIFT2"_J,
	VEHICLE_VALKYRIE     = "VALKYRIE"_J,
	VEHICLE_VALKYRIE2    = "VALKYRIE2"_J,
	VEHICLE_VOLATUS      = "VOLATUS"_J,
};

enum Industrial : Hash
{
	VEHICLE_BULLDOZER = "BULLDOZER"_J,
	VEHICLE_CUTTER    = "CUTTER"_J,
	VEHICLE_DUMP      = "DUMP"_J,
	VEHICLE_FLATBED   = "FLATBED"_J,
	VEHICLE_GUARDIAN  = "GUARDIAN"_J,
	VEHICLE_HANDLER   = "HANDLER"_J,
	VEHICLE_MIXER     = "MIXER"_J,
	VEHICLE_MIXER2    = "MIXER2"_J,
	VEHICLE_RUBBLE    = "RUBBLE"_J,
	VEHICLE_TIPTRUCK  = "TIPTRUCK"_J,
	VEHICLE_TIPTRUCK2 = "TIPTRUCK2"_J,
};

enum Military : Hash
{
	VEHICLE_APC           = "APC"_J,
	VEHICLE_BARRACKS      = "BARRACKS"_J,
	VEHICLE_BARRACKS2     = "BARRACKS2"_J,
	VEHICLE_BARRACKS3     = "BARRACKS3"_J,
	VEHICLE_BARRAGE       = "BARRAGE"_J,
	VEHICLE_CHERNOBOG     = "CHERNOBOG"_J,
	VEHICLE_CRUSADER      = "CRUSADER"_J,
	VEHICLE_HALFTRACK     = "HALFTRACK"_J,
	VEHICLE_KHANJALI      = "KHANJALI"_J,
	VEHICLE_RHINO         = "RHINO"_J,
	VEHICLE_THRUSTER      = "THRUSTER"_J,
	VEHICLE_TRAILERSMALL2 = "TRAILERSMALL2"_J,
};

enum Motorcycles : Hash
{
	VEHICLE_AKUMA       = "AKUMA"_J,
	VEHICLE_AVARUS      = "AVARUS"_J,
	VEHICLE_BAGGER      = "BAGGER"_J,
	VEHICLE_BATI        = "BATI"_J,
	VEHICLE_BATI2       = "BATI2"_J,
	VEHICLE_BF400       = "BF400"_J,
	VEHICLE_CARBONRS    = "CARBONRS"_J,
	VEHICLE_CHIMERA     = "CHIMERA"_J,
	VEHICLE_CLIFFHANGER = "CLIFFHANGER"_J,
	VEHICLE_DAEMON      = "DAEMON"_J,
	VEHICLE_DAEMON2     = "DAEMON2"_J,
	VEHICLE_DEFILER     = "DEFILER"_J,
	VEHICLE_DIABLOUS    = "DIABLOUS"_J,
	VEHICLE_DIABLOUS2   = "DIABLOUS2"_J,
	VEHICLE_DOUBLE      = "DOUBLE"_J,
	VEHICLE_ENDURO      = "ENDURO"_J,
	VEHICLE_ESSKEY      = "ESSKEY"_J,
	VEHICLE_FAGGIO      = "FAGGIO"_J,
	VEHICLE_FAGGIO2     = "FAGGIO2"_J,
	VEHICLE_FAGGIO3     = "FAGGIO3"_J,
	VEHICLE_FCR         = "FCR"_J,
	VEHICLE_FCR2        = "FCR2"_J,
	VEHICLE_GARGOYLE    = "GARGOYLE"_J,
	VEHICLE_HAKUCHOU    = "HAKUCHOU"_J,
	VEHICLE_HAKUCHOU2   = "HAKUCHOU2"_J,
	VEHICLE_HEXER       = "HEXER"_J,
	VEHICLE_INNOVATION  = "INNOVATION"_J,
	VEHICLE_LECTRO      = "LECTRO"_J,
	VEHICLE_MANCHEZ     = "MANCHEZ"_J,
	VEHICLE_NEMESIS     = "NEMESIS"_J,
	VEHICLE_NIGHTBLADE  = "NIGHTBLADE"_J,
	VEHICLE_OPPRESSOR   = "OPPRESSOR"_J,
	VEHICLE_OPPRESSOR2  = "OPPRESSOR2"_J,
	VEHICLE_PCJ         = "PCJ"_J,
	VEHICLE_RATBIKE     = "RATBIKE"_J,
	VEHICLE_RUFFIAN     = "RUFFIAN"_J,
	VEHICLE_SANCHEZ2    = "SANCHEZ2"_J,
	VEHICLE_SANCTUS     = "SANCTUS"_J,
	VEHICLE_SHOTARO     = "SHOTARO"_J,
	VEHICLE_SOVEREIGN   = "SOVEREIGN"_J,
	VEHICLE_THRUST      = "THRUST"_J,
	VEHICLE_VADER       = "VADER"_J,
	VEHICLE_VINDICATOR  = "VINDICATOR"_J,
	VEHICLE_VORTEX      = "VORTEX"_J,
	VEHICLE_WOLFSBANE   = "WOLFSBANE"_J,
	VEHICLE_ZOMBIEA     = "ZOMBIEA"_J,
	VEHICLE_ZOMBIEB     = "ZOMBIEB"_J,
};

enum Muscle : Hash
{
	VEHICLE_BLADE      = "BLADE"_J,
	VEHICLE_BUCCANEER  = "BUCCANEER"_J,
	VEHICLE_BUCCANEER2 = "BUCCANEER2"_J,
	VEHICLE_CHINO      = "CHINO"_J,
	VEHICLE_CHINO2     = "CHINO2"_J,
	VEHICLE_COQUETTE3  = "COQUETTE3"_J,
	VEHICLE_DOMINATOR  = "DOMINATOR"_J,
	VEHICLE_DOMINATOR2 = "DOMINATOR2"_J,
	VEHICLE_DOMINATOR3 = "DOMINATOR3"_J,
	VEHICLE_DUKES      = "DUKES"_J,
	VEHICLE_DUKES2     = "DUKES2"_J,
	VEHICLE_ELLIE      = "ELLIE"_J,
	VEHICLE_FACTION    = "FACTION"_J,
	VEHICLE_FACTION2   = "FACTION2"_J,
	VEHICLE_FACTION3   = "FACTION3"_J,
	VEHICLE_GAUNTLET   = "GAUNTLET"_J,
	VEHICLE_GAUNTLET2  = "GAUNTLET2"_J,
	VEHICLE_HERMES     = "HERMES"_J,
	VEHICLE_HOTKNIFE   = "HOTKNIFE"_J,
	VEHICLE_LURCHER    = "LURCHER"_J,
	VEHICLE_MOONBEAM   = "MOONBEAM"_J,
	VEHICLE_MOONBEAM2  = "MOONBEAM2"_J,
	VEHICLE_NIGHTSHADE = "NIGHTSHADE"_J,
	VEHICLE_PHOENIX    = "PHOENIX"_J,
	VEHICLE_PICADOR    = "PICADOR"_J,
	VEHICLE_RATLOADER  = "RATLOADER"_J,
	VEHICLE_RATLOADER2 = "RATLOADER2"_J,
	VEHICLE_RUINER     = "RUINER"_J,
	VEHICLE_RUINER2    = "RUINER2"_J,
	VEHICLE_RUINER3    = "RUINER3"_J,
	VEHICLE_SABREGT    = "SABREGT"_J,
	VEHICLE_SABREGT2   = "SABREGT2"_J,
	VEHICLE_SLAMVAN    = "SLAMVAN"_J,
	VEHICLE_SLAMVAN2   = "SLAMVAN2"_J,
	VEHICLE_SLAMVAN3   = "SLAMVAN3"_J,
	VEHICLE_STALION    = "STALION"_J,
	VEHICLE_STALION2   = "STALION2"_J,
	VEHICLE_TAMPA      = "TAMPA"_J,
	VEHICLE_TAMPA3     = "TAMPA3"_J,
	VEHICLE_VIGERO     = "VIGERO"_J,
	VEHICLE_VIRGO      = "VIRGO"_J,
	VEHICLE_VIRGO2     = "VIRGO2"_J,
	VEHICLE_VIRGO3     = "VIRGO3"_J,
	VEHICLE_VOODOO     = "VOODOO"_J,
	VEHICLE_VOODOO2    = "VOODOO2"_J,
	VEHICLE_YOSEMITE   = "YOSEMITE"_J,
};

enum OffRoad : Hash
{
	VEHICLE_BFINJECTION  = "BFINJECTION"_J,
	VEHICLE_BIFTA        = "BIFTA"_J,
	VEHICLE_BLAZER       = "BLAZER"_J,
	VEHICLE_BLAZER2      = "BLAZER2"_J,
	VEHICLE_BLAZER3      = "BLAZER3"_J,
	VEHICLE_BLAZER4      = "BLAZER4"_J,
	VEHICLE_BLAZER5      = "BLAZER5"_J,
	VEHICLE_BODHI2       = "BODHI2"_J,
	VEHICLE_BRAWLER      = "BRAWLER"_J,
	VEHICLE_DLOADER      = "DLOADER"_J,
	VEHICLE_DUBSTA3      = "DUBSTA3"_J,
	VEHICLE_DUNE         = "DUNE"_J,
	VEHICLE_DUNE2        = "DUNE2"_J,
	VEHICLE_DUNE3        = "DUNE3"_J,
	VEHICLE_DUNE4        = "DUNE4"_J,
	VEHICLE_DUNE5        = "DUNE5"_J,
	VEHICLE_FREECRAWLER  = "FREECRAWLER"_J,
	VEHICLE_INSURGENT    = "INSURGENT"_J,
	VEHICLE_INSURGENT2   = "INSURGENT2"_J,
	VEHICLE_INSURGENT3   = "INSURGENT3"_J,
	VEHICLE_KALAHARI     = "KALAHARI"_J,
	VEHICLE_KAMACHO      = "KAMACHO"_J,
	VEHICLE_MARSHALL     = "MARSHALL"_J,
	VEHICLE_MESA3        = "MESA3"_J,
	VEHICLE_MONSTER      = "MONSTER"_J,
	VEHICLE_MENACER      = "MENACER"_J,
	VEHICLE_NIGHTSHARK   = "NIGHTSHARK"_J,
	VEHICLE_RANCHERXL    = "RANCHERXL"_J,
	VEHICLE_RANCHERXL2   = "RANCHERXL2"_J,
	VEHICLE_REBEL        = "REBEL"_J,
	VEHICLE_REBEL2       = "REBEL2"_J,
	VEHICLE_RIATA        = "RIATA"_J,
	VEHICLE_SANDKING     = "SANDKING"_J,
	VEHICLE_SANDKING2    = "SANDKING2"_J,
	VEHICLE_TECHNICAL    = "TECHNICAL"_J,
	VEHICLE_TECHNICAL2   = "TECHNICAL2"_J,
	VEHICLE_TECHNICAL3   = "TECHNICAL3"_J,
	VEHICLE_TROPHYTRUCK  = "TROPHYTRUCK"_J,
	VEHICLE_TROPHYTRUCK2 = "TROPHYTRUCK2"_J,
};

enum Planes : Hash
{
	VEHICLE_ALPHAZ1     = "ALPHAZ1"_J,
	VEHICLE_AVENGER     = "AVENGER"_J,
	VEHICLE_BESRA       = "BESRA"_J,
	VEHICLE_BLIMP       = "BLIMP"_J,
	VEHICLE_BLIMP2      = "BLIMP2"_J,
	VEHICLE_BLIMP3      = "BLIMP3"_J,
	VEHICLE_BOMBUSHKA   = "BOMBUSHKA"_J,
	VEHICLE_CARGOPLANE  = "CARGOPLANE"_J,
	VEHICLE_CUBAN800    = "CUBAN800"_J,
	VEHICLE_DODO        = "DODO"_J,
	VEHICLE_DUSTER      = "DUSTER"_J,
	VEHICLE_HOWARD      = "HOWARD"_J,
	VEHICLE_HYDRA       = "HYDRA"_J,
	VEHICLE_JET         = "JET"_J,
	VEHICLE_LAZER       = "LAZER"_J,
	VEHICLE_LUXOR       = "LUXOR"_J,
	VEHICLE_LUXOR2      = "LUXOR2"_J,
	VEHICLE_MAMMATUS    = "MAMMATUS"_J,
	VEHICLE_MICROLIGHT  = "MICROLIGHT"_J,
	VEHICLE_MILJET      = "MILJET"_J,
	VEHICLE_MOGUL       = "MOGUL"_J,
	VEHICLE_MOLOTOK     = "MOLOTOK"_J,
	VEHICLE_NIMBUS      = "NIMBUS"_J,
	VEHICLE_NOKOTA      = "NOKOTA"_J,
	VEHICLE_PYRO        = "PYRO"_J,
	VEHICLE_ROGUE       = "ROGUE"_J,
	VEHICLE_SEABREEZE   = "SEABREEZE"_J,
	VEHICLE_SHAMAL      = "SHAMAL"_J,
	VEHICLE_STARLING    = "STARLING"_J,
	VEHICLE_STRIKEFORCE = "STRIKEFORCE"_J,
	VEHICLE_STUNT       = "STUNT"_J,
	VEHICLE_TITAN       = "TITAN"_J,
	VEHICLE_TULA        = "TULA"_J,
	VEHICLE_VELUM       = "VELUM"_J,
	VEHICLE_VELUM2      = "VELUM2"_J,
	VEHICLE_VESTRA      = "VESTRA"_J,
	VEHICLE_VOLATOL     = "VOLATOL"_J,
};

enum SUVs : Hash
{
	VEHICLE_BALLER      = "BALLER"_J,
	VEHICLE_BALLER2     = "BALLER2"_J,
	VEHICLE_BALLER3     = "BALLER3"_J,
	VEHICLE_BALLER4     = "BALLER4"_J,
	VEHICLE_BALLER5     = "BALLER5"_J,
	VEHICLE_BALLER6     = "BALLER6"_J,
	VEHICLE_BJXL        = "BJXL"_J,
	VEHICLE_CAVALCADE   = "CAVALCADE"_J,
	VEHICLE_CAVALCADE2  = "CAVALCADE2"_J,
	VEHICLE_CONTENDER   = "CONTENDER"_J,
	VEHICLE_DUBSTA      = "DUBSTA"_J,
	VEHICLE_DUBSTA2     = "DUBSTA2"_J,
	VEHICLE_FQ2         = "FQ2"_J,
	VEHICLE_GRANGER     = "GRANGER"_J,
	VEHICLE_GRESLEY     = "GRESLEY"_J,
	VEHICLE_HABANERO    = "HABANERO"_J,
	VEHICLE_HUNTLEY     = "HUNTLEY"_J,
	VEHICLE_LANDSTALKER = "LANDSTALKER"_J,
	VEHICLE_MESA        = "MESA"_J,
	VEHICLE_MESA2       = "MESA2"_J,
	VEHICLE_PATRIOT     = "PATRIOT"_J,
	VEHICLE_PATRIOT2    = "PATRIOT2"_J,
	VEHICLE_RADI        = "RADI"_J,
	VEHICLE_ROCOTO      = "ROCOTO"_J,
	VEHICLE_SEMINOLE    = "SEMINOLE"_J,
	VEHICLE_SERRANO     = "SERRANO"_J,
	VEHICLE_XLS         = "XLS"_J,
	VEHICLE_XLS2        = "XLS2"_J,
};

enum Sedans : Hash
{
	VEHICLE_ASEA         = "ASEA"_J,
	VEHICLE_ASEA2        = "ASEA2"_J,
	VEHICLE_ASTEROPE     = "ASTEROPE"_J,
	VEHICLE_CHEBUREK     = "CHEBUREK"_J,
	VEHICLE_COG55        = "COG55"_J,
	VEHICLE_COG552       = "COG552"_J,
	VEHICLE_COGNOSCENTI  = "COGNOSCENTI"_J,
	VEHICLE_COGNOSCENTI2 = "COGNOSCENTI2"_J,
	VEHICLE_EMPEROR      = "EMPEROR"_J,
	VEHICLE_EMPEROR2     = "EMPEROR2"_J,
	VEHICLE_EMPEROR3     = "EMPEROR3"_J,
	VEHICLE_FUGITIVE     = "FUGITIVE"_J,
	VEHICLE_GLENDALE     = "GLENDALE"_J,
	VEHICLE_INGOT        = "INGOT"_J,
	VEHICLE_INTRUDER     = "INTRUDER"_J,
	VEHICLE_LIMO2        = "LIMO2"_J,
	VEHICLE_PREMIER      = "PREMIER"_J,
	VEHICLE_PRIMO        = "PRIMO"_J,
	VEHICLE_PRIMO2       = "PRIMO2"_J,
	VEHICLE_REGINA       = "REGINA"_J,
	VEHICLE_ROMERO       = "ROMERO"_J,
	VEHICLE_SCHAFTER2    = "SCHAFTER2"_J,
	VEHICLE_SCHAFTER5    = "SCHAFTER5"_J,
	VEHICLE_SCHAFTER6    = "SCHAFTER6"_J,
	VEHICLE_STAFFORD     = "STAFFORD"_J,
	VEHICLE_STANIER      = "STANIER"_J,
	VEHICLE_STRATUM      = "STRATUM"_J,
	VEHICLE_STRETCH      = "STRETCH"_J,
	VEHICLE_SUPERD       = "SUPERD"_J,
	VEHICLE_SURGE        = "SURGE"_J,
	VEHICLE_TAILGATER    = "TAILGATER"_J,
	VEHICLE_WARRENER     = "WARRENER"_J,
	VEHICLE_WASHINGTON   = "WASHINGTON"_J,
};

enum Service : Hash
{
	VEHICLE_AIRBUS      = "AIRBUS"_J,
	VEHICLE_BRICKADE    = "BRICKADE"_J,
	VEHICLE_BUS         = "BUS"_J,
	VEHICLE_COACH       = "COACH"_J,
	VEHICLE_PBUS2       = "PBUS2"_J,
	VEHICLE_RALLYTRUCK  = "RALLYTRUCK"_J,
	VEHICLE_RENTALBUS   = "RENTALBUS"_J,
	VEHICLE_TAXI        = "TAXI"_J,
	VEHICLE_TOURBUS     = "TOURBUS"_J,
	VEHICLE_TRASH       = "TRASH"_J,
	VEHICLE_TRASH2      = "TRASH2"_J,
	VEHICLE_WASTELANDER = "WASTELANDER"_J,
};

enum Sports : Hash
{
	VEHICLE_ALPHA        = "ALPHA"_J,
	VEHICLE_BANSHEE      = "BANSHEE"_J,
	VEHICLE_BESTIAGTS    = "BESTIAGTS"_J,
	VEHICLE_BLISTA2      = "BLISTA2"_J,
	VEHICLE_BLISTA3      = "BLISTA3"_J,
	VEHICLE_BUFFALO      = "BUFFALO"_J,
	VEHICLE_BUFFALO2     = "BUFFALO2"_J,
	VEHICLE_BUFFALO3     = "BUFFALO3"_J,
	VEHICLE_CARBONIZZARE = "CARBONIZZARE"_J,
	VEHICLE_COMET2       = "COMET2"_J,
	VEHICLE_COMET3       = "COMET3"_J,
	VEHICLE_COMET4       = "COMET4"_J,
	VEHICLE_COMET5       = "COMET5"_J,
	VEHICLE_COQUETTE     = "COQUETTE"_J,
	VEHICLE_ELEGY        = "ELEGY"_J,
	VEHICLE_ELEGY2       = "ELEGY2"_J,
	VEHICLE_FELTZER2     = "FELTZER2"_J,
	VEHICLE_FLASHGT      = "FLASHGT"_J,
	VEHICLE_FUROREGT     = "FUROREGT"_J,
	VEHICLE_FUSILADE     = "FUSILADE"_J,
	VEHICLE_FUTO         = "FUTO"_J,
	VEHICLE_GB200        = "GB200"_J,
	VEHICLE_HOTRING      = "HOTRING"_J,
	VEHICLE_JESTER       = "JESTER"_J,
	VEHICLE_JESTER2      = "JESTER2"_J,
	VEHICLE_JESTER3      = "JESTER3"_J,
	VEHICLE_KHAMELION    = "KHAMELION"_J,
	VEHICLE_KURUMA       = "KURUMA"_J,
	VEHICLE_KURUMA2      = "KURUMA2"_J,
	VEHICLE_LYNX2        = "LYNX2"_J,
	VEHICLE_MASSACRO     = "MASSACRO"_J,
	VEHICLE_MASSACRO2    = "MASSACRO2"_J,
	VEHICLE_NEON         = "NEON"_J,
	VEHICLE_NINEF        = "NINEF"_J,
	VEHICLE_NINEF2       = "NINEF2"_J,
	VEHICLE_OMNIS        = "OMNIS"_J,
	VEHICLE_PARIAH       = "PARIAH"_J,
	VEHICLE_PENUMBRA     = "PENUMBRA"_J,
	VEHICLE_RAIDEN       = "RAIDEN"_J,
	VEHICLE_RAPIDGT      = "RAPIDGT"_J,
	VEHICLE_RAPIDGT2     = "RAPIDGT2"_J,
	VEHICLE_RAPTOR       = "RAPTOR"_J,
	VEHICLE_REVOLTER     = "REVOLTER"_J,
	VEHICLE_RUSTON       = "RUSTON"_J,
	VEHICLE_SCHAFTER3    = "SCHAFTER3"_J,
	VEHICLE_SCHAFTER4    = "SCHAFTER4"_J,
	VEHICLE_SCHWARZER    = "SCHWARZER"_J,
	VEHICLE_SENTINEL3    = "SENTINEL3"_J,
	VEHICLE_SEVEN70      = "SEVEN70"_J,
	VEHICLE_SPECTER      = "SPECTER"_J,
	VEHICLE_SPECTER2     = "SPECTER2"_J,
	VEHICLE_STREITER     = "STREITER"_J,
	VEHICLE_SULTAN       = "SULTAN"_J,
	VEHICLE_SURANO       = "SURANO"_J,
	VEHICLE_TAMPA2       = "TAMPA2"_J,
	VEHICLE_TROPOS       = "TROPOS"_J,
	VEHICLE_VERLIERER2   = "VERLIERER2"_J,
};

enum SportsClassic : Hash
{
	VEHICLE_ARDENT    = "ARDENT"_J,
	VEHICLE_BTYPE     = "BTYPE"_J,
	VEHICLE_BTYPE2    = "BTYPE2"_J,
	VEHICLE_BTYPE3    = "BTYPE3"_J,
	VEHICLE_CASCO     = "CASCO"_J,
	VEHICLE_CHEETAH2  = "CHEETAH2"_J,
	VEHICLE_COQUETTE2 = "COQUETTE2"_J,
	VEHICLE_DELUXO    = "DELUXO"_J,
	VEHICLE_FAGALOA   = "FAGALOA"_J,
	VEHICLE_FELTZER3  = "FELTZER3"_J,
	VEHICLE_GT500     = "GT500"_J,
	VEHICLE_HUSTLER   = "HUSTLER"_J,
	VEHICLE_INFERNUS2 = "INFERNUS2"_J,
	VEHICLE_JB700     = "JB700"_J,
	VEHICLE_MAMBA     = "MAMBA"_J,
	VEHICLE_MANANA    = "MANANA"_J,
	VEHICLE_MICHELLI  = "MICHELLI"_J,
	VEHICLE_MONROE    = "MONROE"_J,
	VEHICLE_PEYOTE    = "PEYOTE"_J,
	VEHICLE_PIGALLE   = "PIGALLE"_J,
	VEHICLE_RAPIDGT3  = "RAPIDGT3"_J,
	VEHICLE_RETINUE   = "RETINUE"_J,
	VEHICLE_SAVESTRA  = "SAVESTRA"_J,
	VEHICLE_STINGER   = "STINGER"_J,
	VEHICLE_STINGERGT = "STINGERGT"_J,
	VEHICLE_STROMBERG = "STROMBERG"_J,
	VEHICLE_SWINGER   = "SWINGER"_J,
	VEHICLE_TORERO    = "TORERO"_J,
	VEHICLE_TORNADO   = "TORNADO"_J,
	VEHICLE_TORNADO2  = "TORNADO2"_J,
	VEHICLE_TORNADO3  = "TORNADO3"_J,
	VEHICLE_TORNADO4  = "TORNADO4"_J,
	VEHICLE_TORNADO5  = "TORNADO5"_J,
	VEHICLE_TORNADO6  = "TORNADO6"_J,
	VEHICLE_TURISMO2  = "TURISMO2"_J,
	VEHICLE_VISERIS   = "VISERIS"_J,
	VEHICLE_Z190      = "Z190"_J,
	VEHICLE_ZTYPE     = "ZTYPE"_J,
};

enum Super : Hash
{
	VEHICLE_ADDER      = "ADDER"_J,
	VEHICLE_AUTARCH    = "AUTARCH"_J,
	VEHICLE_BANSHEE2   = "BANSHEE2"_J,
	VEHICLE_BULLET   = "BULLET"_J,
	VEHICLE_CHEETAH    = "CHEETAH"_J,
	VEHICLE_CYCLONE    = "CYCLONE"_J,
	VEHICLE_ENTITYXF   = "ENTITYXF"_J,
	VEHICLE_ENTITY2    = "ENTITY2"_J,
	VEHICLE_FMJ        = "FMJ"_J,
	VEHICLE_GP1        = "GP1"_J,
	VEHICLE_INFERNUS   = "INFERNUS"_J,
	VEHICLE_ITALIGTB   = "ITALIGTB"_J,
	VEHICLE_ITALIGTB2  = "ITALIGTB2"_J,
	VEHICLE_LE7B       = "LE7B"_J,
	VEHICLE_NERO       = "NERO"_J,
	VEHICLE_NERO2      = "NERO2"_J,
	VEHICLE_OSIRIS     = "OSIRIS"_J,
	VEHICLE_PENETRATOR = "PENETRATOR"_J,
	VEHICLE_PFISTER811 = "PFISTER811"_J,
	VEHICLE_PROTOTIPO  = "PROTOTIPO"_J,
	VEHICLE_REAPER     = "REAPER"_J,
	VEHICLE_SC1        = "SC1"_J,
	VEHICLE_SCRAMJET   = "SCRAMJET"_J,
	VEHICLE_SHEAVA     = "SHEAVA"_J,
	VEHICLE_SULTANRS   = "SULTANRS"_J,
	VEHICLE_T20        = "T20"_J,
	VEHICLE_TAIPAN     = "TAIPAN"_J,
	VEHICLE_TEMPESTA   = "TEMPESTA"_J,
	VEHICLE_TEZERACT   = "TEZERACT"_J,
	VEHICLE_TURISMOR   = "TURISMOR"_J,
	VEHICLE_TYRANT     = "TYRANT"_J,
	VEHICLE_TYRUS      = "TYRUS"_J,
	VEHICLE_VACCA      = "VACCA"_J,
	VEHICLE_VAGNER     = "VAGNER"_J,
	VEHICLE_VIGILANTE  = "VIGILANTE"_J,
	VEHICLE_VISIONE    = "VISIONE"_J,
	VEHICLE_VOLTIC     = "VOLTIC"_J,
	VEHICLE_VOLTIC2    = "VOLTIC2"_J,
	VEHICLE_XA21       = "XA21"_J,
	VEHICLE_ZENTORNO   = "ZENTORNO"_J,
};

enum Trailers : Hash
{
	VEHICLE_ARMYTANKER   = "ARMYTANKER"_J,
	VEHICLE_ARMYTRAILER2 = "ARMYTRAILER2"_J,
	VEHICLE_BALETRAILER  = "BALETRAILER"_J,
	VEHICLE_BOATTRAILER  = "BOATTRAILER"_J,
	VEHICLE_CABLECAR     = "CABLECAR"_J,
	VEHICLE_DOCKTRAILER  = "DOCKTRAILER"_J,
	VEHICLE_GRAINTRAILER = "GRAINTRAILER"_J,
	VEHICLE_PROPTRAILER  = "PROPTRAILER"_J,
	VEHICLE_RAKETRAILER  = "RAKETRAILER"_J,
	VEHICLE_TR2          = "TR2"_J,
	VEHICLE_TR3          = "TR3"_J,
	VEHICLE_TR4          = "TR4"_J,
	VEHICLE_TRFLAT       = "TRFLAT"_J,
	VEHICLE_TVTRAILER    = "TVTRAILER"_J,
	VEHICLE_TANKER       = "TANKER"_J,
	VEHICLE_TANKER2      = "TANKER2"_J,
	VEHICLE_TRAILERLOGS  = "TRAILERLOGS"_J,
	VEHICLE_TRAILERSMALL = "TRAILERSMALL"_J,
	VEHICLE_TRAILERS     = "TRAILERS"_J,
	VEHICLE_TRAILERS2    = "TRAILERS2"_J,
	VEHICLE_TRAILERS3    = "TRAILERS3"_J,
};

enum Trains : Hash
{
	VEHICLE_FREIGHT      = "FREIGHT"_J,
	VEHICLE_FREIGHTCAR   = "FREIGHTCAR"_J,
	VEHICLE_FREIGHTCONT1 = "FREIGHTCONT1"_J,
	VEHICLE_FREIGHTCONT2 = "FREIGHTCONT2"_J,
	VEHICLE_FREIGHTGRAIN = "FREIGHTGRAIN"_J,
	VEHICLE_TANKERCAR    = "TANKERCAR"_J,
};

enum Utility : Hash
{
	VEHICLE_AIRTUG       = "AIRTUG"_J,
	VEHICLE_CADDY        = "CADDY"_J,
	VEHICLE_CADDY2       = "CADDY2"_J,
	VEHICLE_CADDY3       = "CADDY3"_J,
	VEHICLE_CARACARA     = "CARACARA"_J,
	VEHICLE_DOCKTUG      = "DOCKTUG"_J,
	VEHICLE_FORKLIFT     = "FORKLIFT"_J,
	VEHICLE_MOWER        = "MOWER"_J,
	VEHICLE_RIPLEY       = "RIPLEY"_J,
	VEHICLE_SADLER       = "SADLER"_J,
	VEHICLE_SADLER2      = "SADLER2"_J,
	VEHICLE_SCRAP        = "SCRAP"_J,
	VEHICLE_TOWTRUCK     = "TOWTRUCK"_J,
	VEHICLE_TOWTRUCK2    = "TOWTRUCK2"_J,
	VEHICLE_TRACTOR      = "TRACTOR"_J,
	VEHICLE_TRACTOR2     = "TRACTOR2"_J,
	VEHICLE_TRACTOR3     = "TRACTOR3"_J,
	VEHICLE_UTILLITRUCK  = "UTILLITRUCK"_J,
	VEHICLE_UTILLITRUCK2 = "UTILLITRUCK2"_J,
	VEHICLE_UTILLITRUCK3 = "UTILLITRUCK3"_J,
};

enum Vans : Hash
{
	VEHICLE_BISON     = "BISON"_J,
	VEHICLE_BISON2    = "BISON2"_J,
	VEHICLE_BISON3    = "BISON3"_J,
	VEHICLE_BOBCATXL  = "BOBCATXL"_J,
	VEHICLE_BOXVILLE  = "BOXVILLE"_J,
	VEHICLE_BOXVILLE2 = "BOXVILLE2"_J,
	VEHICLE_BOXVILLE3 = "BOXVILLE3"_J,
	VEHICLE_BOXVILLE4 = "BOXVILLE4"_J,
	VEHICLE_BOXVILLE5 = "BOXVILLE5"_J,
	VEHICLE_BURRITO   = "BURRITO"_J,
	VEHICLE_BURRITO2  = "BURRITO2"_J,
	VEHICLE_BURRITO3  = "BURRITO3"_J,
	VEHICLE_BURRITO4  = "BURRITO4"_J,
	VEHICLE_BURRITO5  = "BURRITO5"_J,
	VEHICLE_CAMPER    = "CAMPER"_J,
	VEHICLE_GBURRITO  = "GBURRITO"_J,
	VEHICLE_GBURRITO2 = "GBURRITO2"_J,
	VEHICLE_JOURNEY   = "JOURNEY"_J,
	VEHICLE_MINIVAN   = "MINIVAN"_J,
	VEHICLE_MINIVAN2  = "MINIVAN2"_J,
	VEHICLE_PARADISE  = "PARADISE"_J,
	VEHICLE_PONY      = "PONY"_J,
	VEHICLE_PONY2     = "PONY2"_J,
	VEHICLE_RUMPO     = "RUMPO"_J,
	VEHICLE_RUMPO2    = "RUMPO2"_J,
	VEHICLE_RUMPO3    = "RUMPO3"_J,
	VEHICLE_SPEEDO    = "SPEEDO"_J,
	VEHICLE_SPEEDO2   = "SPEEDO2"_J,
	VEHICLE_SURFER    = "SURFER"_J,
	VEHICLE_SURFER2   = "SURFER2"_J,
	VEHICLE_TACO      = "TACO"_J,
	VEHICLE_YOUGA     = "YOUGA"_J,
	VEHICLE_YOUGA2    = "YOUGA2"_J,
};

enum ArenaWar : Hash
{
	VEHICLE_BRUISER    = "BRUISER"_J,
	VEHICLE_BRUISER2   = "BRUISER2"_J,
	VEHICLE_BRUISER3   = "BRUISER3"_J,
	VEHICLE_BRUTUS     = "BRUTUS"_J,
	VEHICLE_BRUTUS2    = "BRUTUS2"_J,
	VEHICLE_BRUTUS3    = "BRUTUS3"_J,
	VEHICLE_CERBERUS   = "CERBERUS"_J,
	VEHICLE_CERBERUS2  = "CERBERUS2"_J,
	VEHICLE_CERBERUS3  = "CERBERUS3"_J,
	VEHICLE_CLIQUE     = "CLIQUE"_J,
	VEHICLE_DEATHBIKE  = "DEATHBIKE"_J,
	VEHICLE_DEATHBIKE2 = "DEATHBIKE2"_J,
	VEHICLE_DEATHBIKE3 = "DEATHBIKE3"_J,
	VEHICLE_DEVESTE    = "DEVESTE"_J,
	VEHICLE_DEVIANT    = "DEVIANT"_J,
	VEHICLE_DOMINATOR4 = "DOMINATOR4"_J,
	VEHICLE_DOMINATOR5 = "DOMINATOR5"_J,
	VEHICLE_DOMINATOR6 = "DOMINATOR6"_J,
	VEHICLE_IMPALER    = "IMPALER"_J,
	VEHICLE_IMPALER2   = "IMPALER2"_J,
	VEHICLE_IMPALER3   = "IMPALER3"_J,
	VEHICLE_IMPALER4   = "IMPALER4"_J,
	VEHICLE_IMPERATOR  = "IMPERATOR"_J,
	VEHICLE_IMPERATOR2 = "IMPERATOR2"_J,
	VEHICLE_IMPERATOR3 = "IMPERATOR3"_J,
	VEHICLE_ISSI4      = "ISSI4"_J,
	VEHICLE_ISSI5      = "ISSI5"_J,
	VEHICLE_ISSI6      = "ISSI6"_J,
	VEHICLE_ITALIGTO   = "ITALIGTO"_J,
	VEHICLE_MONSTER3   = "MONSTER3"_J,
	VEHICLE_MONSTER4   = "MONSTER4"_J,
	VEHICLE_MONSTER5   = "MONSTER5"_J,
	VEHICLE_RCBANDITO  = "RCBANDITO"_J,
	VEHICLE_SCARAB2    = "SCARAB2"_J,
	VEHICLE_SCARAB3    = "SCARAB3"_J,
	VEHICLE_SCHLAGEN   = "SCHLAGEN"_J,
	VEHICLE_SLAMVAN4   = "SLAMVAN4"_J,
	VEHICLE_SLAMVAN5   = "SLAMVAN5"_J,
	VEHICLE_SLAMVAN6   = "SLAMVAN6"_J,
	VEHICLE_TOROS      = "TOROS"_J,
	VEHICLE_TULIP      = "TULIP"_J,
	VEHICLE_VAMOS      = "VAMOS"_J,
	VEHICLE_ZR380      = "ZR380"_J,
	VEHICLE_ZR3802     = "ZR3802"_J,
	VEHICLE_ZR3803     = "ZR3803"_J,
};

enum CasinoUpdate : Hash
{
	VEHICLE_CARACARA2 = "CARACARA2"_J,
	VEHICLE_DRAFTER   = "DRAFTER"_J,
	VEHICLE_DYNASTY   = "DYNASTY"_J,
	VEHICLE_EMERUS    = "EMERUS"_J,
	VEHICLE_GAUNTLET3 = "GAUNTLET3"_J,
	VEHICLE_GAUNTLET4 = "GAUNTLET4"_J,
	VEHICLE_HELLION   = "HELLION"_J,
	VEHICLE_ISSI7     = "ISSI7"_J,
	VEHICLE_JUGULAR   = "JUGULAR"_J,
	VEHICLE_KRIEGER   = "KRIEGER"_J,
	VEHICLE_LOCUST    = "LOCUST"_J,
	VEHICLE_NEBULA    = "NEBULA"_J,
	VEHICLE_NEO       = "NEO"_J,
	VEHICLE_NOVAK     = "NOVAK"_J,
	VEHICLE_PARAGON   = "PARAGON"_J,
	VEHICLE_PARAGON2  = "PARAGON2"_J,
	VEHICLE_PEYOTE2   = "PEYOTE2"_J,
	VEHICLE_RROCKET   = "RROCKET"_J,
	VEHICLE_S80       = "S80"_J,
	VEHICLE_THRAX     = "THRAX"_J,
	VEHICLE_ZION3     = "ZION3"_J,
	VEHICLE_ZORRUSSO  = "ZORRUSSO"_J
};

enum CasinoHeist : Hash
{
	VEHICLE_ASBO      = "ABSO"_J,
	VEHICLE_EVERON    = "EVERON"_J,
	VEHICLE_FORMULA   = "FORMULA"_J,
	VEHICLE_FORMULA2  = "FORMULA2"_J,
	VEHICLE_FURIA     = "FURIA"_J,
	VEHICLE_IMORGON   = "IMORGON"_J,
	VEHICLE_JB7002    = "JB7002"_J,
	VEHICLE_KANJO     = "KANJO"_J,
	VEHICLE_KOMODA    = "KOMODA"_J,
	VEHICLE_MINITANK  = "MINITANK"_J,
	VEHICLE_OUTLAW    = "OUTLAW"_J,
	VEHICLE_REBLA     = "REBLA"_J,
	VEHICLE_RETINUE2  = "RETINUE2"_J,
	VEHICLE_STRYDER   = "STRYDER"_J,
	VEHICLE_SUGOI     = "SUGOI"_J,
	VEHICLE_SULTAN2   = "SULTAN2"_J,
	VEHICLE_VAGRANT   = "VAGRANT"_J,
	VEHICLE_VSTR      = "VSTR"_J,
	VEHICLE_YOSEMITE2 = "YOSEMITE2"_J,
	VEHICLE_ZHABA     = "ZHABA"_J,
};

enum SummerUpdate : Hash
{
	VEHICLE_CLUB         = "CLUB"_J,
	VEHICLE_COQUETTE4    = "VEHICLE_COQUETTE4"_J,
	VEHICLE_DUKES3       = "DUKES3"_J,
	VEHICLE_GAUNTLET5    = "GAUNTLET5"_J,
	VEHICLE_GLENDALE2    = "GLENDALE2"_J,
	VEHICLE_LANDSTALKER2 = "LANDSTALKER2"_J,
	VEHICLE_MANANA2      = "MANANA2"_J,
	VEHICLE_OPENWHEEL1   = "OPENWHEEL1"_J,
	VEHICLE_OPENWHEEL2   = "OPENWHEEL2"_J,
	VEHICLE_PENUMBRA2    = "PENUMBRA2"_J,
	VEHICLE_PEYOTE3      = "PEYOTE3"_J,
	VEHICLE_SEMINOLE2    = "SEMINOLE2"_J,
	VEHICLE_TIGON        = "TIGON"_J,
	VEHICLE_YOSEMITE3    = "YOSEMITE3"_J,
	VEHICLE_YOUGA3       = "YOUGA3"_J
};

enum CayoPericoHeist : Hash
{
	VEHICLE_ALKONOST     = "ALKONOST"_J,
	VEHICLE_ANNIHILATOR2 = "ANNIHILATOR2"_J,
	VEHICLE_AVISA        = "AVISA"_J,
	VEHICLE_BRIOSO2      = "BRIOSO2"_J,
	VEHICLE_DINGHY5      = "DINGHY5"_J,
	VEHICLE_ITALIRSX     = "ITALIRSX"_J,
	VEHICLE_KOSATKA      = "KOSATKA"_J,
	VEHICLE_LONGFIN      = "LONGFIN"_J,
	VEHICLE_MANCHEZE     = "MANCHEZE"_J,
	VEHICLE_PATROLBOAT   = "PATROLBOAT"_J,
	VEHICLE_SEASPARROW2  = "SEASPARROW2"_J,
	VEHICLE_SLAMTRUCK    = "SLAMTRUCK"_J,
	VEHICLE_SQUADDIE     = "SQUADDIE"_J,
	VEHICLE_TOREADOR     = "TOREADOR"_J,
	VEHICLE_VERUS        = "VERUS"_J,
	VEHICLE_VETIR        = "VETIR"_J,
	VEHICLE_VETO         = "VETO"_J,
	VEHICLE_VETO2        = "VETO2"_J,
	VEHICLE_WEEVIL       = "WEEVIL"_J,
	VEHICLE_WINKY        = "WINKY"_J
};

enum LosSantosTuners : Hash
{
	VEHICLE_CALICO      = "CALICO"_J,
	VEHICLE_COMET6      = "COMET6"_J,
	VEHICLE_CYPHER      = "CYPHER"_J,
	VEHICLE_DOMINATOR7  = "DOMINATOR7"_J,
	VEHICLE_DOMINATOR8  = "DOMINATOR8"_J,
	VEHICLE_EUROS       = "EUROS"_J,
	VEHICLE_FREIGHTCAR2 = "FREIGHTCAR2"_J,
	VEHICLE_FUTO2       = "FUTO2"_J,
	VEHICLE_GROWLER     = "GROWLER"_J,
	VEHICLE_JESTER4     = "JESTER4"_J,
	VEHICLE_PREVION     = "PREVION"_J,
	VEHICLE_REMUS       = "REMUS"_J,
	VEHICLE_RT3000      = "RT3000"_J,
	VEHICLE_SULTAN3     = "SULTAN3"_J,
	VEHICLE_TAILGATER2  = "TAILGATER2"_J,
	VEHICLE_VECTRE      = "VECTRE"_J,
	VEHICLE_WARRENER2   = "WARRENER2"_J,
	VEHICLE_ZR350       = "ZR350"_J
};

enum TheContract : Hash
{
	VEHICLE_BUFFALO4   = "BUFFALO4"_J,
	VEHICLE_GRANGER2   = "GRANGER2"_J,
	VEHICLE_IWAGEN     = "IWAGEN"_J,
	VEHICLE_PATRIOT3   = "PATRIOT3"_J,
	VEHICLE_ASTRON     = "ASTRON"_J,
	VEHICLE_DEITY      = "DEITY"_J,
	VEHICLE_IGNUS      = "IGNUS"_J,
	VEHICLE_JUBILEE    = "JUBILEE"_J,
	VEHICLE_REEVER     = "REEVER"_J,
	VEHICLE_SHINOBI    = "SHINOBI"_J,
	VEHICLE_ZENO       = "ZENO"_J,
	VEHICLE_BALLER7    = "BALLER7"_J,
	VEHICLE_CHAMPION   = "CHAMPION"_J,
	VEHICLE_CINQUEMILA = "CINQUEMILA"_J,
	VEHICLE_COMET7     = "COMET7"_J,
	VEHICLE_YOUGA4     = "YOUGA4"_J,
	VEHICLE_MULE5      = "MULE5"_J
};

enum TheCriminalEnterprise : Hash
{
	VEHICLE_TENF      = "TENF"_J,
	VEHICLE_TENF2     = "TENF2"_J,
	VEHICLE_WEEVIL2   = "WEEVIL2"_J,
	VEHICLE_TORERO2   = "TORERO2"_J,
	VEHICLE_SENTINEL4 = "SENTINEL4"_J,
	VEHICLE_POSTLUDE  = "POSTLUDE"_J,
	VEHICLE_DRAUGUR   = "DRAUGUR"_J,
	VEHICLE_RHINEHART = "RHINEHART"_J,
	VEHICLE_VIGERO2   = "VIGERO2"_J,
	VEHICLE_GREENWOOD = "GREENWOOD"_J,
	VEHICLE_OMNISEGT  = "OMNISEGT"_J,
	VEHICLE_RUINER4   = "RUINER4"_J,
	VEHICLE_CORSITA   = "CORSITA"_J,
	VEHICLE_LM87      = "LM87"_J,
	VEHICLE_SM722     = "SM722"_J,
	VEHICLE_CONADA    = "CONADA"_J,
	VEHICLE_BRIOSO3   = "BRIOSO3"_J,
	VEHICLE_KANJOSJ   = "KANJOSJ"_J
};

enum DrugWars : Hash
{
	VEHICLE_BOOR        = "boor"_J,
	VEHICLE_BRICKADE2   = "brickade2"_J,
	VEHICLE_BROADWAY    = "broadway"_J,
	VEHICLE_CARGOPLANE2 = "cargoplane2"_J,
	VEHICLE_ENTITY3     = "entity3"_J,
	VEHICLE_ENDORA      = "eudora"_J,
	VEHICLE_EVERON2     = "everon2"_J,
	VEHICLE_ISSI8       = "issi8"_J,
	VEHICLE_JOURNEY2    = "journey2"_J,
	VEHICLE_MANCHEZ3    = "manchez3"_J,
	VEHICLE_PANTHERE    = "panthere"_J,
	VEHICLE_POWERSURGE  = "powersurge"_J,
	VEHICLE_R300        = "r300"_J,
	VEHICLE_SURFER3     = "surfer3"_J,
	VEHICLE_TAHOMA      = "tahoma"_J,
	VEHICLE_TULIP2      = "tulip2"_J,
	VEHICLE_VIRTUE      = "virtue"_J
};

enum SanAndreasMercenaries : Hash
{
	VEHICLE_AVENGER3    = "avenger3"_J,
	VEHICLE_AVENGER4    = "avenger4"_J,
	VEHICLE_BRIGHAM     = "brigham"_J,
	VEHICLE_BUFFALO5    = "buffalo5"_J,
	VEHICLE_CLIQUE2     = "clique2"_J,
	VEHICLE_CONADA2     = "conada2"_J,
	VEHICLE_COUREUR     = "coureur"_J,
	VEHICLE_GAUNTLET6   = "gauntlet6"_J,
	VEHICLE_INDUCTOR    = "inductor"_J,
	VEHICLE_INDUCTOR2   = "inductor2"_J,
	VEHICLE_L35         = "l35"_J,
	VEHICLE_MONSTROCITI = "monstrociti"_J,
	VEHICLE_RAIJU       = "raiju"_J,
	VEHICLE_RATEL       = "ratel"_J,
	VEHICLE_SPEEDO4     = "speedo4"_J,
	VEHICLE_STINGERTT   = "stingertt"_J,
	VEHICLE_STREAMER216 = "streamer216"_J
};

enum Scrapyard : Hash
{
	VEHICLE_ALEUTIAN      = "ALEUTIAN"_J,
	VEHICLE_ASTEROPE2     = "ASTEROPE2"_J,
	VEHICLE_BALLER8       = "BALLER8"_J,
	VEHICLE_BENSON2       = "BENSON2"_J,
	VEHICLE_BOXVILLE6     = "BOXVILLE6"_J,
	VEHICLE_CAVALCADE3    = "CAVALCADE3"_J,
	VEHICLE_DOMINATOR9    = "DOMINATOR9"_J,
	VEHICLE_DORADO        = "DORADO"_J,
	VEHICLE_DRIFTEUROS    = "DRIFTEUROS"_J,
	VEHICLE_DRIFTFR36     = "DRIFTFR36"_J,
	VEHICLE_DRIFTFUTO     = "DRIFTFUTO"_J,
	VEHICLE_DRIFTJESTER   = "DRIFTJESTER"_J,
	VEHICLE_DRIFTREMUS    = "DRIFTREMUS"_J,
	VEHICLE_DRIFTTAMPA    = "DRIFTTAMPA"_J,
	VEHICLE_DRIFTYOSEMITE = "DRIFTYOSEMITE"_J,
	VEHICLE_DRIFTZR350    = "DRIFTZR350"_J,
	VEHICLE_FR36          = "FR36"_J,
	VEHICLE_IMPALER5      = "IMPALER5"_J,
	VEHICLE_IMPALER6      = "IMPALER6"_J,
	VEHICLE_POLGAUNTLET   = "POLGAUNTLET"_J,
	VEHICLE_POLICE5       = "POLICE5"_J,
	VEHICLE_TERMINUS      = "TERMINUS"_J,
	VEHICLE_TOWTRUCK3     = "TOWTRUCK3"_J,
	VEHICLE_TOWTRUCK4     = "TOWTRUCK4"_J,
	VEHICLE_TURISMO3      = "TURISMO3"_J,
	VEHICLE_VIGERO3       = "VIGERO3"_J,
	VEHICLE_VIVANITE      = "VIVANITE"_J
};