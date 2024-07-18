#pragma once
#include "core/vars.hpp"
#include "entity.hpp"
#include "natives.hpp"
#include "services/ped_animations/ped_animations_service.hpp"
#include "util/animations.hpp"

namespace big::ped
{
	inline void kill_ped(const Ped ped)
	{
		Hash weaponHash;
		WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weaponHash, 1);

		if (!ENTITY::IS_ENTITY_DEAD(ped, 0) && entity::take_control_of(ped))
			ENTITY::SET_ENTITY_HEALTH(ped, 0, self::ped, weaponHash);
	}

	inline bool load_animation_dict(const char* dict)
	{
		if (STREAMING::HAS_ANIM_DICT_LOADED(dict))
			return true;

		for (uint8_t i = 0; !STREAMING::HAS_ANIM_DICT_LOADED(dict) && i < 35; i++)
		{
			STREAMING::REQUEST_ANIM_DICT(dict);
			script::get_current()->yield();
		}

		return STREAMING::HAS_ANIM_DICT_LOADED(dict);
	}

	inline void ped_play_animation(Ped ped, const std::string_view& animDict, const std::string_view& animName, float speed = 4.f, float speedMultiplier = -4.f, int duration = -1, int flag = 0, float playbackRate = 0, bool lockPos = false, Vector3 pos = {}, Vector3 rot = {}, int ik_flags = 0)
	{
		if (load_animation_dict(animDict.data()))
			if (pos.x == 0 && pos.y == 0 && pos.z == 0)
				TASK::TASK_PLAY_ANIM(ped, animDict.data(), animName.data(), speed, speedMultiplier, duration, flag, playbackRate, lockPos, lockPos, lockPos);
			else
				TASK::TASK_PLAY_ANIM_ADVANCED(ped, animDict.data(), animName.data(), pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, speed, speedMultiplier, duration, flag, playbackRate, lockPos, ik_flags);
	}

	static void get_anim(Entity e)
	{
		auto c     = 0;
		auto found = false;

		if (!animations::all_anims.size())
		{
			animations::fetch_all_anims();
			return;
		}

		for (auto& p : animations::all_anims)
		{
			if (c % 10000 == 0)
				script::get_current()->yield();

			if (PED::IS_SCRIPTED_SCENARIO_PED_USING_CONDITIONAL_ANIM(e, p.first.c_str(), p.second.c_str())
			    || ENTITY::IS_ENTITY_PLAYING_ANIM(e, p.first.c_str(), p.second.c_str(), 3))
			{
				LOG(VERBOSE) << "Dict: " << p.first << " Anim: " << p.second;
				found                                          = true;
				g_ped_animation_service.current_animation.dict = p.first;
				g_ped_animation_service.current_animation.anim = p.second;
				break;
			}

			++c;
		}

		if (!found)
			LOG(VERBOSE) << "Not able to find the animation " << c;
	}
}
