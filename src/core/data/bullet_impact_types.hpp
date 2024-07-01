#pragma once

inline std::unordered_map<eExplosionTag, const char*> BULLET_IMPACTS = {
    {eExplosionTag::DONTCARE, "DEFAULT_BULLETS"},
    {eExplosionTag::MOLOTOV, "MOLOTOV"},
    {eExplosionTag::DIR_STEAM, "DIR_STEAM"},
    {eExplosionTag::DIR_FLAME, "DIR_FLAME"},
    {eExplosionTag::DIR_WATER_HYDRANT, "DIR_WATER_HYDRANT"},
    {eExplosionTag::FLARE, "FLARE"},
    {eExplosionTag::EXP_TAG_FIREWORK, "FIREWORK"},
    {eExplosionTag::EXP_TAG_SNOWBALL, "SNOWBALL"},
    {eExplosionTag::EXP_TAG_MORTAR_KINETIC, "MORTAR_KINETIC"},
    {eExplosionTag::EXP_TAG_VEHICLEMINE_KINETIC, "VEHICLEMINE_KINETIC"},
    {eExplosionTag::EXP_TAG_EMPLAUNCHER_EMP, "EMPLAUNCHER_EMP"},
};
