#include "backend/looped/looped.hpp"
#include "core/data/ptfx_effects.hpp"
#include "gta/enums.hpp"
#include "natives.hpp"
#include "script.hpp"

namespace big
{
	static PedBones ptfx_ped_bones[5] = {PedBones::SKEL_Head, PedBones::SKEL_L_Hand, PedBones::SKEL_R_Hand, PedBones::SKEL_L_Foot, PedBones::SKEL_R_Foot};
	static const char* ptfx_vehicle_pos[4] = {"wheel_lf", "wheel_lr", "wheel_rf", "wheel_rr"};

	static inline void show_player_ptfx_effect()
	{
		if (g_ptfx_effects.center)
		{
			GRAPHICS::USE_PARTICLE_FX_ASSET(g_ptfx_effects.asset);
			GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_ON_PED_BONE(g_ptfx_effects.effect,
			    self::ped,
			    0.f,
			    0.f,
			    0.f,
			    0.f,
			    0.f,
			    0.f,
			    (int)PedBones::SKEL_Pelvis1,
			    g_ptfx_effects.size,
			    1,
			    1,
			    1);
		}
		else
			for (const auto& ptfx_bone : ptfx_ped_bones)
			{
				GRAPHICS::USE_PARTICLE_FX_ASSET(g_ptfx_effects.asset);
				GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_ON_PED_BONE(g_ptfx_effects.effect,
				    self::ped,
				    0.f,
				    0.f,
				    0.f,
				    0.f,
				    0.f,
				    0.f,
				    (int)ptfx_bone,
				    g_ptfx_effects.size,
				    1,
				    1,
				    1);
			}
	}

	static inline void show_vehicle_ptfx_effect()
	{
		for (const auto& ptfx_bone : ptfx_vehicle_pos)
		{
			Vector3 bone_pos = ENTITY::GET_WORLD_POSITION_OF_ENTITY_BONE(self::veh, ENTITY::GET_ENTITY_BONE_INDEX_BY_NAME(self::veh, ptfx_bone));
			GRAPHICS::USE_PARTICLE_FX_ASSET(g_ptfx_effects.asset);
			GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD(g_ptfx_effects.effect,
			    bone_pos.x,
			    bone_pos.y,
			    bone_pos.z,
			    0.f,
			    0.f,
			    0.f,
			    g_ptfx_effects.size,
			    0.f,
			    0.f,
			    0.f,
			    0);
		}
	}

	void looped::ptfx_loop()
	{
		static bool disabled               = false;
		static const char* current_fx_name = nullptr;

		if (g_ptfx_effects.show)
		{
			if (disabled)
				disabled = false;

			if (current_fx_name != g_ptfx_effects.asset)
			{
				STREAMING::REMOVE_PTFX_ASSET();
				current_fx_name = g_ptfx_effects.asset;
			}

			if (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED(g_ptfx_effects.asset))
				STREAMING::REQUEST_NAMED_PTFX_ASSET(g_ptfx_effects.asset);
			else
			{
				if (self::veh == 0)
					show_player_ptfx_effect();
				else
					show_vehicle_ptfx_effect();
			}

			if (g_ptfx_effects.delay)
				script::get_current()->yield(std::chrono::milliseconds::duration(g_ptfx_effects.delay));
		}
		else if (!disabled)
		{
			disabled = true;
			STREAMING::REMOVE_PTFX_ASSET();
		}
	}
}