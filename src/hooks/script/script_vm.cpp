#include "core/data/script_vm.hpp"

#include "hooking/hooking.hpp"
#include "services/script_patcher/script_patcher_service.hpp"

#include <script/scrProgram.hpp>

namespace big
{
	class script_vm_guard
	{
		rage::scrProgram* m_program;
		uint8_t** m_orig_bytecode;

	public:
		script_vm_guard(rage::scrProgram* program) :
		    m_program(program)
		{
			m_orig_bytecode = program->m_code_blocks;
	
			if (auto bytecode = g_script_patcher_service->get_script_bytecode(program->m_name_hash))
				program->m_code_blocks = bytecode;

			g_in_script_vm = true;
		}

		~script_vm_guard()
		{
			m_program->m_code_blocks = m_orig_bytecode;

			g_in_script_vm = false;
		}
	};

	rage::eThreadState hooks::script_vm(uint64_t* start_stack, uint64_t** scr_globals, rage::scrProgram* program, rage::scrThreadContext* ctx)
	{
		script_vm_guard guard(program);

		return g_hooking->get_original<hooks::script_vm>()(start_stack, scr_globals, program, ctx);
	}
}