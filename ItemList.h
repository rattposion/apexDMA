#pragma once

#include <vector>
#include <string>

inline std::vector<std::tuple<int, std::string, std::string>> MedList = {
	{221, "Phoenix",""},
	{222, "Med-Kit",""},
	{223, "Syringe",""},
	{225, "Shield Cell",""},
	{224, "Battery",""},
	{392, "AMP Armor",""},
	{389, "AMP BATTERIES",""},
	{390, "AMP HEALER",""},
	{388, "AMP AMMO",""},
	{391, "AMP BOOSTER",""},
	{333, "Respawn",""},
};

inline std::vector<std::tuple<int, std::string, std::string>> LightGunList = {
	{49, "Alternator",""},
	{87, "R-301",""},
	{82, "Spit-fire",""},
	{43, "G7",""},
	{169, "Ammo",""},
	{55, "R-99",""}
};

inline std::vector<std::tuple<int, std::string, std::string>> EnergyGunList = {
	{20, "Devotion",""},
	{8, "L-STAR",""},
	{26, "Triple Take",""},
	{164, "Nemesis",""},
	{170, "Ammo",""},
	{67, "VOLT",""},
	{14, "Havoc",""},
	{136, "RE-45",""},
};

inline std::vector<std::tuple<int, std::string, std::string>> HeavyGunList = {
	{32, "FlatLine",""},
	{37, "Hemlock",""},
	{61, "Prowler",""},
	{159, "30-30",""},
	{176, "Rampage",""},
	{172, "Ammo",""}
};

inline std::vector<std::tuple<int, std::string, std::string>> SniperGunList = {
	{117, "WingMan",""},
	{72, "Long Bow",""},
	{77, "Charge Rifle",""},
	{145, "Sentinel",""},
	{152, "Bow",""},
	{173, "Ammo",""},
	{174, "Arrow",""},
};

inline std::vector<std::tuple<int, std::string, std::string>> ShotGunlist = {
	{104, "Mozambique",""},
	{92, "EVA",""},
	{98, "PeaceKeeper",""},
	{171, "Ammo",""},
	{2, "Mastiff",""},
	{4, "Gold Mastiff",""}
};

inline std::vector<std::tuple<int, std::string, std::string>> GrenadeList = {
	{257, "Thermite",""},
	{258, "Grenade",""},
	{259, "Arc_Star",""},
};

inline std::vector<std::tuple<int, std::string, std::string>> RedGunlist = {
	{1, "Kraber",""},
	{129, "P2020",""},
	{183, "C.A.R",""},
	{256, "Knife",""},
};
//lv4
inline std::vector<std::tuple<int, std::string, std::string>> LvL4Bodylist = {
	{255, "Backpack",""},
	{246, "Shield",""},
	{367, "EVO",""},
	{347, "NESSIE",""},
};
//lv3
inline std::vector<std::tuple<int, std::string, std::string>> LvL3Bodylist = {
	{250, "Knockdown Shield",""},
	{254, "Backpack","n"},
	{245, "Shield","l"},
};
//lv2
inline std::vector<std::tuple<int, std::string, std::string>> LvL2Bodylist = {
	{249, "Knockdown Shield",""},
	{253, "Backpack","}"},
	{244, "Shield","l"},
};
//lv1
inline std::vector<std::tuple<int, std::string, std::string>> LvL1Bodylist = {
	{248, "Knockdown Shield",""},
	{252, "Backpack",""},
	{243, "Shield",""},
};
//lv4
inline std::vector<std::tuple<int, std::string, std::string>> Partslvl4list = {
	{276, "Laser sight",""},
	{269, "4X-10X",""},
	{272, "Suppressor",""}, //lv3
	{280, "Light Mag",""},
	{284, "Heavy Mag",""},
	{288, "Energy Mag",""},
	{292, "Sniper Mag",""},
	{296, "Bolt",""},
	{303, "Sniper Stock",""},

};
//lv3
inline std::vector<std::tuple<int, std::string, std::string>> Partslvl3list = {
	{265, "3x",""},
	{266, "2x-4x",""},
	{268, "4x-8x",""},
	{275, "Laser sight",""},
	{272, "Suppressor",""},
	{279, "Light Mag",""},
	{283, "Heavy Mag",""},
	{287, "Energy Mag",""},
	{291, "Sniper Mag",""},
	{295, "Bolt",""},
	{299, "Stock",""},
	{301, "Sniper Stock",""},
	{220, "Accelerant",""},
};
//lv2
inline std::vector<std::tuple<int, std::string, std::string>> Partslvl2list = {
	{261, "2x",""},
	{263, "1x-2x",""},
	{274, "Laser",""},
	{267, "6x",""},
	{271, "Suppressor",""},
	{278, "Light Mag",""},
	{282, "Heavy Mag",""},
	{286, "Energy Mag",""},
	{290, "Sniper Mag",""},
	{294, "Bolt",""},
	{298, "Stock",""},
	{302, "Sniper Stock",""},
	{342, "Evac",""}
};
//lv1
inline std::vector<std::tuple<int, std::string, std::string>> Partslvl1list = {
	{260, "1x",""},
	{273, "Laser",""},
	{270, "Suppressor",""},
	{277, "Light Mag",""},
	{281, "Heavy Mag",""},
	{285, "Energy Mag",""},
	{289, "Sniper Mag",""},
	{293, "Bolt",""},
	{297, "Stock",""},
	{300, "Sniper Stock",""}
};
inline auto Lists =
{
	MedList,
	LightGunList,
	ShotGunlist,
	EnergyGunList,
	HeavyGunList,
	SniperGunList,
	Partslvl1list,
	Partslvl2list,
	Partslvl3list,
	Partslvl4list,
	LvL1Bodylist,
	LvL2Bodylist,
	LvL3Bodylist,
	LvL4Bodylist,
	RedGunlist,
	GrenadeList,
};