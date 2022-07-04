// Default editor label defintions
// These arrays are used to create default labels in the event that there's no native LBLS file available to copy.
// These correspond to the counts of related data in the original game.

#pragma once

#include <afx.h>

namespace Editorlabels
{

	const CString editordefs_patterntables[] = {  // BATTLEPATTERNTABLE_COUNT
		"Imp - Wolf - Iguana - Giant",
		"Sahag - Pirate - Shark - BigEye",
		"Bone - Creep - Hyena - Ogre",
		"Asp - Lobster - Bull - Troll",
		"Image - Geist - Worm - Eye",
		"Medusa - Catman - Pede - Tiger",
		"Vamp - Gargoyle - Earth - Dragon 1",
		"Slime - Spider - Manticor - Ankylo",
		"Mummy - Coctric - Wyvern - Tyro",
		"Caribe - Gator - Ocho - Hydra",
		"Sentry - Water - Naga - Chimera",
		"Wizard - Garland - Dragon 2 - Golem",
		"Badman - Astos - Madpony - Warmech",
		"Kary -- Lich",
		"Kraken -- Tiamat",
		" ---- Chaos ----" };

	const CString editordefs_shoptable[] = {  // SHOP_COUNT
		"Wep-Coneria","Wep-Provoka","Wep-Elfland","Wep-Melmond","Wep-Crescent Lake","Wep-Gaia","Wep-1","Wep-2","Wep-3","Wep-4",
		"Arm-Coneria","Arm-Provoka","Arm-Elfland","Arm-Melmond","Arm-Crescent Lake","Arm-Gaia","Arm-1","Arm-2","Arm-3","Arm-4",
		"Wht-Coneria","Wht-Provoka","Wht-Elfland","Wht-Melmond","Wht-Crescent Lake","Wht-Elfland 2","Wht-Gaia","Wht-Gaia 2","Wht-Onrac","Wht-Lifein",
		"Blk-Coneria","Blk-Provoka","Blk-Elfland","Blk-Melmond","Blk-Crescent Lake","Blk-Elfland 2","Blk-Gaia","Blk-Gaia 2","Blk-Onrac","Blk-Lifein",
		"Cnc-Coneria","Cnc-Elfland","Cnc-Crescent Lake","Cnc-Gaia","Cnc-Onrac","Cnc-Provoka","Cnc-1","Cnc-2","Cnc-3","Cnc-4",
		"Inn-Coneria","Inn-Provoka","Inn-Elfland","Inn-Melmond","Inn-Crescent Lake","Inn-Gaia","Inn-Onrac","Inn-1","Inn-2","Inn-3",
		"Itm-Coneria","Itm-Provoka","Itm-Elfland","Itm-Crescent Lake","Itm-Gaia","Itm-Onrac","Itm-1","Itm-2","Itm-3","Caravan" };

	const CString editordefs_wepmagtable[] = {  // WEAPONMAGICGRAPHIC_COUNT
		"Sword 1","Sword 2","Sword 3","Sword 4","Sword 5","Hammer","Knife","Axe","Staff 1","Staff 2","Nunchuck","Bl Belt Hands",
		"Bar of Light","Bar of Light-2","4 Sparkles","4 Sparkles-2","Stars","Stars-2",
		"Energy Beam","Energy Beam-2","Energy Flare","Energy Flare-2","Glowing Ball",
		"Glowing Ball-2","Large Sparkle","Large Sparkle-2","Sparkling Hand","Sparkling Hand-2" };

	const CString editordefs_aitable[] = {  // AI_COUNT
		"FrWolf","Agama","Medusa","Oddeye","Bigeye","Cerebus","WzOgre","Sand W","Eye","Phantom",
		"Mancat","Vampire","WzVamp","R.Goyle","Frost D","Red D","Perelisk","R. Hydra","Naga","GrNaga",
		"Chimera","Jimera","Sorcerer","Gas D","Blue D","MudGol","RockGol","IronGol","Evilman","Mage",
		"Fighter","Nitemare","WarMech","Manticor","Lich","Lich 2","Kary","Kary 2","Kraken","Kraken 2",
		"Tiamat","Tiamat 2","Chaos","Astos" };

	const CString editordefs_backdroptables[] = {  // BACKDROP_COUNT
		"Grass","Marsh Cave","Cave","Water","Forest","ToF","Desert","Marsh","Earth Cave",
		"Castle","River","Tower","Sea Shrine","Ice Cave","Volcano","Waterfall" };

	const CString editordefs_onteleporttables[] = {  // ONTELEPORT_COUNT
		"Cardia - 1","Coneria","Provoka","Elfland","Melmond","Crescent Lake","Gaia","Onrac",
		"Leifen","Coneria Castle","Elfland Castle","Northwest Castle","Castle of Ordeals",
		"Temple of Fiends","Earth Cave","Gurgu Volcano","Ice Cave","Cardia - 2",
		"Bahamut's Room","Waterfall","Dwarf Cave","Matoya's Cave","Sarda's Cave","Marsh Cave",
		"Mirage Tower","Titan's Tunnel - E","Titan's Tunnel - W","Cardia - 3","Cardia - 4",
		"Cardia - 5","Not Used","Not Used" };

	const CString editordefs_nnteleporttables[] = {  // NNTELEPORT_COUNT
		"Coneria Castle 1","Time Warp","Marsh Cave 1","Marsh Cave 2","Marsh Cave 3","Earth Cave 1","Earth Cave 2","Earth Cave 3","Earth Cave 4","Gurgu 1","Gurgu 2","Gurgu 3","Gurgu 4","Gurgu 5","Gurgu 6","Ice Cave 1","Ice Cave 2","Ice Cave 3","Ice Cave 4","Ice Cave 5",
		"Ice Cave 6","Ice Cave 7","Castle Ordeals 1","Castle Ordeals 2","Castle Ordeals 3","Bahamut's Room","Castle Ordeal 4","Castle Ordeal 5","Castle Ordeal 6","Castle Ordeal 7","Castle Ordeal 8","Castle Ordeal 9","Sea Shrine 1","Sea Shrine 2","Sea Shrine 3","Sea Shrine 4","Sea Shrine 5","Sea Shrine 6","Sea Shrine 7","Sea Shrine 8","Sea Shrine 9",
		"Mirage Tower 1","Mirage Tower 2","Sky Palace 1","Sky Palace 2","Sky Palace 3","Sky Palace 4","Sky Palace 5","ToF 1","ToF 2","ToF 3","ToF 4","ToF 5","ToF 6","ToF 7","ToF 8","ToF 9","ToF 10","Castle Ordeal 10","Castle Ordeal 11","Castle Ordeal 12","Castle Ordeal 13",
		"Coneria Castle","Rescue Princess" };

	const CString editordefs_noteleporttables[] = {  // NOTELEPORT_COUNT
		"Titan E","Titan W","Ice Cave","Castle Ordeals","Castle Coneria","Earth Cave",
		"Gurgu Volcano","Sea Shrine","Sky Castle","Not used","Not used","Not used",
		"Not used","Not used","Not used","Not used" };

	const CString editordefs_tilesettables[] = {  // TILESET_COUNT
		"Town","Castle","Earth Cave","Ice Cave","Tower","Shrine","Sky Castle","ToF" };

	const CString editordefs_maptables[] = {  // MAP_COUNT
		"Coneria","Provoka","Elfland","Melmond","Cresent Lake","Gaia","Onrac","Leifen","Coneria Castle 1F","Elfland Castle",
		"Northwest Castle","Castle of Ordeals 1F","Temple of Fiends","Earth Cave B1","Gurgu Volcano B1","Ice Cave B1","Cardia",
		"Bahamut's Room B1","Waterfall","Dwarf Cave","Matoya's Cave","Sarda's Cave","Marsh Cave B1","Mirage Tower 1F","Coneria Castle 2F",
		"Castle of Ordeal 2F","Castle of Ordeal 3F","Marsh Cave B2","Marsh Cave B3","Earth Cave B2","Earth Cave B3","Earth Cave B4","Earth Cave B5",
		"Gurgu Volcano B2","Gurgu Volcano B3","Gurgu Volcano B4","Gurgu Volcano B5","Ice Cave B2","Ice Cave B3","Bahamut's Room B2",
		"Mirage Tower 2F","Mirage Tower 3F","Sea Shrine B5","Sea Shrine B4","Sea Shrine B3","Sea Shrine B2","Sea Shrine B1",
		"Sky Palace 1F","Sky Palace 2F","Sky Palace 3F","Sky Palace 4F","Sky Palace 5F","Temple of Fiends 1F","Temple of Fiends 2F",
		"Temple Fiends 3F","Temple Fiends-Earth","Temple Fiends-Fire","Temple Fiends-Water","Temple Fiends-Air",
		"Temple Fiends-Chaos","Titan's Tunnel" };

	const CString editordefs_treasuretables[] = {  // TREASURE_COUNT
		"Unused","Coneria 1","Coneria 2","Coneria 3","Coneria 4","Coneria 5","Coneria 6","Temple of Fiends 1","Temple of Fiends 2","Temple of Fiends 3","Temple of Fiends 4","Temple of Fiends 5",
		"Temple of Fiends 6","Elfland 1","Elfland 2","Elfland 3","Elfland 4","NorthWest Castle 1","NorthWest Castle 2","NorthWest Castle 3","Marsh Cave 1","Marsh Cave 2","Marsh Cave 3","Marsh Cave 4",
		"Marsh Cave 5","Marsh Cave 6","Marsh Cave 7","Marsh Cave 8","Marsh Cave 9","Marsh Cave 10","Marsh Cave 11","Marsh Cave 12","Marsh Cave 13","Dwarf Cave 1","Dwarf Cave 2","Dwarf Cave 3",
		"Dwarf Cave 4","Dwarf Cave 5","Dwarf Cave 6","Dwarf Cave 7","Dwarf Cave 8","Dwarf Cave 9","Dwarf Cave 10","Matoya's Cave 1","Matoya's Cave 2","Matoya's Cave 3","Earth Cave 1","Earth Cave 2",
		"Earth Cave 3","Earth Cave 4","Earth Cave 5","Earth Cave 6","Earth Cave 7","Earth Cave 8","Earth Cave 9","Earth Cave 10","Earth Cave 11","Earth Cave 12","Earth Cave 13","Earth Cave 14",
		"Earth Cave 15","Earth Cave 16","Earth Cave 17","Earth Cave 18","Earth Cave 19","Earth Cave 20","Earth Cave 21","Earth Cave 22","Earth Cave 23","Earth Cave 24","Titan's Tunnel 1","Titan's Tunnel 2",
		"Titan's Tunnel 3","Titan's Tunnel 4","Gurgu Volcano 1","Gurgu Volcano 2","Gurgu Volcano 3","Gurgu Volcano 4","Gurgu Volcano 5","Gurgu Volcano 6","Gurgu Volcano 7","Gurgu Volcano 8",
		"Gurgu Volcano 9","Gurgu Volcano 10","Gurgu Volcano 11","Gurgu Volcano 12","Gurgu Volcano 13","Gurgu Volcano 14","Gurgu Volcano 15","Gurgu Volcano 16","Gurgu Volcano 17","Gurgu Volcano 18",
		"Gurgu Volcano 19","Gurgu Volcano 20","Gurgu Volcano 21","Gurgu Volcano 22","Gurgu Volcano 23","Gurgu Volcano 24","Gurgu Volcano 25","Gurgu Volcano 26","Gurgu Volcano 27","Gurgu Volcano 28",
		"Gurgu Volcano 29","Gurgu Volcano 30","Gurgu Volcano 31","Gurgu Volcano 32","Gurgu Volcano 33","Ice Cave 1","Ice Cave 2","Ice Cave 3","Ice Cave 4","Ice Cave 5","Ice Cave 6","Ice Cave 7","Ice Cave 8",
		"Ice Cave 9","Ice Cave 10","Ice Cave 11","Ice Cave 12","Ice Cave 13","Ice Cave 14","Ice Cave 15","Ice Cave 16","Castle of Ordeal 1","Castle of Ordeal 2","Castle of Ordeal 3","Castle of Ordeal 4",
		"Castle of Ordeal 5","Castle of Ordeal 6","Castle of Ordeal 7","Castle of Ordeal 8","Castle of Ordeal 9","Cardia 1","Cardia 2","Cardia 3","Cardia 4","Cardia 5","Cardia 6","Cardia 7","Cardia 8",
		"Cardia 9","Cardia 10","Cardia 11","Cardia 12","Cardia 13","Not Used 1","Not Used 2","Not Used 3","Not Used 4","Sea Shrine 1","Sea Shrine 2","Sea Shrine 3","Sea Shrine 4","Sea Shrine 5","Sea Shrine 6",
		"Sea Shrine 7","Sea Shrine 8","Sea Shrine 9","Sea Shrine 10","Sea Shrine 11","Sea Shrine 12","Sea Shrine 13","Sea Shrine 14","Sea Shrine 15","Sea Shrine 16","Sea Shrine 17","Sea Shrine 18","Sea Shrine 19",
		"Sea Shrine 20","Sea Shrine 21","Sea Shrine 22","Sea Shrine 23","Sea Shrine 24","Sea Shrine 25","Sea Shrine 26","Sea Shrine 27","Sea Shrine 28","Sea Shrine 29","Sea Shrine 30","Sea Shrine 31",
		"Sea Shrine 32","Waterfall 1","Waterfall 2","Waterfall 3","Waterfall 4","Waterfall 5","Waterfall 6","Not Used 5","Not Used 6","Not Used 7","Not Used 8","Not Used 9","Not Used 10","Not Used 11",
		"Not Used 12","Not Used 13","Mirage Tower 1","Mirage Tower 2","Mirage Tower 3","Mirage Tower 4","Mirage Tower 5","Mirage Tower 6","Mirage Tower 7","Mirage Tower 8","Mirage Tower 9","Mirage Tower 10",
		"Mirage Tower 11","Mirage Tower 12","Mirage Tower 13","Mirage Tower 14","Mirage Tower 15","Mirage Tower 16","Mirage Tower 17","Mirage Tower 18","Sky Palace 1","Sky Palace 2","Sky Palace 3",
		"Sky Palace 4","Sky Palace 5","Sky Palace 6","Sky Palace 7","Sky Palace 8","Sky Palace 9","Sky Palace 10","Sky Palace 11","Sky Palace 12","Sky Palace 13","Sky Palace 14","Sky Palace 15",
		"Sky Palace 16","Sky Palace 17","Sky Palace 18","Sky Palace 19","Sky Palace 20","Sky Palace 21","Sky Palace 22","Sky Palace 23","Sky Palace 24","Sky Palace 25","Sky Palace 26","Sky Palace 27",
		"Sky Palace 28","Sky Palace 29","Sky Palace 30","Sky Palace 31","Sky Palace 32","Sky Palace 33","Sky Palace 34","ToF Revisited 1","ToF Revisited 2",
		"ToF Revisited 3","ToF Revisited 4","ToF Revisited 5","ToF Revisited 6","ToF Revisited 7","Unused" };

	const CString editordefs_spritetables[] = {  // SPRITE_COUNT
		"No Sprite","King of Coneria","Garland","Kidnapped Princess","Bikke the Pirate","Elf Prince's Doctor","Elf Prince","Astos","Nerrick","Dwarven Blacksmith","Matoya",
		"Dr. Unne","Vampire","Sadra","Bahamut","Unknown","Submarine Engineer","Waterfall Robot","Rescued Princess","Fairy in Bottle",
		"Titan","Canoe Sage","Earth Plate","Fiends' Plate","Garland 1","Garland 2","Garland 3","ORB of the Earth","ORB of Fire","ORB of Water",
		"ORB of Wind","Unknown","Coneria Castle 1F - 1","Unknown","Coneria Castle 1F - 2","Coneria Castle 1F - 3","Unknown","Coneria Castle 1F - 4","Coneria Castle 1F - 5",
		"Unknown","Unknown","Coneria Castle 1F - 6","Coneria Castle 1F - 7","Coneria Castle 2F - 1","Coneria Castle 1F - 8","Unknown","Coneria Castle 1F - 9","Unknown","Coneria Castle 2F - 2",
		"Coneria - 1","Coneria - 2","Unknown","Coneria - 3","Coneria - 4","Coneria - 5","Coneria - 6","Coneria - 7","Coneria - 8","Sky Warrior Bat - 1","Sky Warrior Bat - 2",
		"Sky Warrior Bat - 3","Sky Warrior Bat - 4","Sky Warrior Bat - 5","Provoka - 1","Provoka - 2","Provoka - 3","Provoka - 4","Provoka - 5","Unknown","Castle of Elf - 1",
		"Castle of Elf - 2","Castle of Elf - 3","Castle of Elf - 4","Castle of Elf - 5","Castle of Elf - 6","Unknown","Unknown","Elfland - 1",
		"Elfland - 2","Elfland - 3","Elfland - 4","Elfland - 5","Elfland - 6","Elfland - 7","Elfland - 8","Unknown","Unknown","Bat","Dwarf Cave - 1","Dwarf Cave - 2",
		"Dwarf Cave - 3","Dwarf Cave - 4","Dwarf Cave - 5","Dwarf Cave - 6","Dwarf Cave - 7","Dwarf Cave - 8","Dwarf Cave - 9","Dwarf Cave - 10","Unknown","Dwarf Cave - 11",
		"Matoya's Broom - 1","Matoya's Broom - 2","Matoya's Broom - 3","Matoya's Broom - 4","Melmond - 1","Melmond - 2","Melmond - 3","Melmond - 4","Melmond - 5","Melmond - 6",
		"Melmond - 7","Melmond - 8","Melmond - 9","Melmond - 10","Melmond - 11","Melmond - 12","Melmond - 13","Unknown","Unknown",
		"Crescent Lake - 1","Crescent Lake - 2","Crescent Lake - 3","Crescent Lake - 4","Crescent Lake - 5","Crescent Lake - 6","Crescent Lake - 7","Crescent Lake - 8",
		"Crescent Lake - 9","Crescent Lake - 10","Crescent Lake - 11","Crescent Lake - 12","Crescent Lake - 13","Castle of Ordeals 1F","Cardian Dragon - 1","Cardian Dragon - 2",
		"Cardian Dragon - 3","Cardian Dragon - 4","Cardian Dragon - 5","Cardian Dragon - 6","Cardian Dragon - 7","Cardian Dragon - 8","Cardian Dragon - 9","Cardian Dragon - 10",
		"Cardian Dragon - 11","Cardian Dragon - 12","Unknown","Unknown","Onrac - 1","Onrac - 2","Onrac - 3","Onrac - 4","Onrac - 5","Onrac - 6","Onrac - 7",
		"Onrac - 8","Onrac - 9","Onrac - 10","Onrac - 11","Onrac - 12","Onrac - 13","Onrac - 14","Unknown","Bad Cube Robot","Mermaid - 1",
		"Mermaid - 2","Mermaid - 3","Mermaid - 4","Mermaid - 5","Mermaid - 6","Mermaid - 7","Mermaid - 8","Mermaid - 9","Mermaid - 10","Unknown",
		"Gaia - 1","Gaia - 2","Gaia - 3","Gaia - 4","Gaia - 5","Gaia - 6","Gaia - 7","Gaia - 8","Gaia - 9","Gaia - 10","Gaia - 11","Gaia - 12","Gaia - 13",
		"Lifein - 1","Lifein - 2","Lifein - 3","Lifein - 4","Lifein - 5","Lifein - 6","Lifein - 7","Lifein - 8","Lifein - 9",
		"Lifein - 10","Lifein - 11","Lifein - 12","Unknown","Lifein - 13","Lifein - 14","Black ORB!","Unknown",
		"Robot 2","Robot 3","Robot 1","Robot 4" };

	const CString editordefs_spritegraphictables[] = {  // MAPSPRITEGRAPHIC_COUNT
		"Princess","Woman","Fatty","Dancer","Orb","Witch","Prince","Soldier","Scholar","Mohawk",
		"Boy","Sage","Dwarf","Mermaid","Liefen","King","Broom","Bat","Garland","Pirate",
		"Fairy","Robot","Dragon","Bahamut","Female Elf","Elf","Elf Prince","Plate","Titan","Vampire" };

	const CString editordefs_specialtext[] = {  // SPECIAL_COUNT
		"Door - Open Rooms",
		"Door - Close Rooms",
		"Door - Locked Door",
		"Move - Need Crown",
		"Move - Need Cube",
		"Move - Need 4 Orbs",
		"Orb - Earth Orb",
		"Orb - Fire Orb",
		"Orb - Water Orb",
		"Orb - Air Orb",
		"Misc - Lower HP",
		"Misc - Use Lute",
		"Misc - Use Rod" };

	const CString editordefs_misccoordtext[] = {  // MISCCOORDOFFSET_COUNT
		"Starting","Ship","Airship","Bridge","Canal" };

	const CString editordefs_elements[] = {
		"Status", "Poi/Stone", "Time/Space", "Death", "Fire", "Ice", "Lit", "Earth"
	};

	const CString editordefs_aileffects[] = {
		"Death", "Stone", "Poison", "Dark", "Stun", "Sleep", "Mute", "Conf"
	};

	const CString editordefs_enemycategories[] = {
		"Special", "Dragon", "Giant", "Undead", "Were", "Water", "Mage", "Regenerative"
	};

} // end namespace Editorlabels
