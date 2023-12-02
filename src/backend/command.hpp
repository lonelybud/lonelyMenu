#pragma once
#include "fiber_pool.hpp"
#include "gta/joaat.hpp"

namespace big
{
	class command;

	inline std::unordered_map<rage::joaat_t, command*> g_commands;

	class command
	{
	protected:
		std::string m_name;
		std::string m_label;
		std::string m_description;
		bool m_fiber_pool;

	public:
		command(const std::string& name, const std::string& label, const std::string& description, bool fiber_pool = true) :
		    m_name(name),
		    m_label(label),
		    m_description(description),
		    m_fiber_pool(fiber_pool)
		{
			g_commands[rage::joaat(name)] = this;
		}

		inline const std::string& get_name()
		{
			return m_name;
		}
		inline const std::string get_label()
		{
			return m_label;
		}
		inline const std::string get_description()
		{
			return m_description;
		}

		static command* get(rage::joaat_t command)
		{
			return g_commands[command];
		}

		virtual void execute()
		{
			LOG(WARNING) << "Command execute method called. This one should not be called.";
		}

		virtual void call()
		{
			if (m_fiber_pool)
				g_fiber_pool->queue_job([this] {
					execute();
				});
			else
				execute();
		}
	};
}