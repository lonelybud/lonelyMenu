#pragma once
#include "command.hpp"
#include "fiber_pool.hpp"

namespace big
{
	class bool_command;

	inline std::vector<bool_command*> g_bool_commands;

	class bool_command : public command
	{
	protected:
		bool& m_toggle;

	public:
		bool_command(const std::string& name, const std::string& label, const std::string& description, bool& toggle) :
		    command(name, label, description),
		    m_toggle(toggle)
		{
			g_bool_commands.push_back(this);
		}

		inline bool& is_enabled()
		{
			return m_toggle;
		}

		virtual void on_enable() {};
		virtual void on_disable() {};

		virtual void refresh()
		{
			g_fiber_pool->execute_on_game_thread([this] {
				m_toggle ? on_enable() : on_disable();
			});
		}
	};
}