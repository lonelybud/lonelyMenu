#include "renderer.hpp"

#include "core/settings/window.hpp"
#include "file_manager.hpp"
#include "fonts/fonts.hpp"
#include "gui.hpp"
#include "pointers.hpp"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>
#include <imgui_internal.h>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace big
{
	bool renderer::init()
	{
		if (!g_pointers->m_gta.m_swapchain || !*g_pointers->m_gta.m_swapchain)
		{
			LOG(FATAL) << "Invalid swapchain ptr";
			return false;
		}

		m_dxgi_swapchain = *g_pointers->m_gta.m_swapchain;

		if (m_dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&m_d3d_device)) < 0)
		{
			LOG(FATAL) << "Failed to get D3D device.";
			return false;
		}

		m_d3d_device->GetImmediateContext(&m_d3d_device_context);

		auto file_path = g_file_manager.get_project_file("./imgui.ini").get_path();

		ImGuiContext* ctx = ImGui::CreateContext();

		static std::string path = file_path.make_preferred().string();
		ctx->IO.IniFilename     = path.c_str();

		ImGui_ImplDX11_Init(m_d3d_device, m_d3d_device_context);
		ImGui_ImplWin32_Init(g_pointers->m_hwnd);

		// initialize font here
		auto& io = ImGui::GetIO();

		{
			ImFontConfig fnt_cfg{};
			fnt_cfg.FontDataOwnedByAtlas = false;
			strcpy(fnt_cfg.Name, "Fnt20px");

			io.Fonts->AddFontFromMemoryTTF(const_cast<uint8_t*>(font_storopia),
			    sizeof(font_storopia),
			    20.f,
			    &fnt_cfg,
			    io.Fonts->GetGlyphRangesDefault());
			io.Fonts->Build();
		}

		{
			ImFontConfig fnt_cfg{};
			fnt_cfg.FontDataOwnedByAtlas = false;
			strcpy(fnt_cfg.Name, "Fnt28px");

			g_window.font_title = io.Fonts->AddFontFromMemoryTTF(const_cast<uint8_t*>(font_storopia), sizeof(font_storopia), 28.f, &fnt_cfg);
			io.Fonts->Build();
		}

		{
			ImFontConfig fnt_cfg{};
			fnt_cfg.FontDataOwnedByAtlas = false;
			strcpy(fnt_cfg.Name, "Fnt24px");

			g_window.font_sub_title = io.Fonts->AddFontFromMemoryTTF(const_cast<uint8_t*>(font_storopia), sizeof(font_storopia), 24.f, &fnt_cfg);
			io.Fonts->Build();
		}

		{
			ImFontConfig fnt_cfg{};
			fnt_cfg.FontDataOwnedByAtlas = false;
			strcpy(fnt_cfg.Name, "Fnt18px");

			g_window.font_small = io.Fonts->AddFontFromMemoryTTF(const_cast<uint8_t*>(font_storopia), sizeof(font_storopia), 18.f, &fnt_cfg);
			io.Fonts->Build();
		}

		{
			ImFontConfig font_icons_cfg{};
			font_icons_cfg.FontDataOwnedByAtlas = false;
			std::strcpy(font_icons_cfg.Name, "Icons");
			g_window.font_icon = io.Fonts->AddFontFromMemoryTTF(const_cast<uint8_t*>(font_icons), sizeof(font_icons), 24.f, &font_icons_cfg);
		}
		//

		return true;
	}

	void renderer::destroy()
	{
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();
	}

	bool renderer::add_dx_callback(dx_callback callback, uint32_t priority)
	{
		if (!m_dx_callbacks.insert({priority, callback}).second)
		{
			LOG(WARNING) << "Duplicate priority given on DX Callback!";

			return false;
		}
		return true;
	}

	void renderer::add_wndproc_callback(wndproc_callback callback)
	{
		m_wndproc_callbacks.emplace_back(callback);
	}

	void renderer::on_present()
	{
		new_frame();
		for (const auto& cb : m_dx_callbacks | std::views::values)
			cb();
		end_frame();
	}

	void renderer::rescale(float rel_size)
	{
		pre_reset();
		g_gui->restore_default_style();

		if (rel_size != 1.0f)
			ImGui::GetStyle().ScaleAllSizes(rel_size);

		ImGui::GetStyle().MouseCursorScale = 1.0f;
		ImGui::GetIO().FontGlobalScale     = rel_size;
		post_reset();
	}

	void renderer::pre_reset()
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}

	void renderer::post_reset()
	{
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	void renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		for (const auto& cb : m_wndproc_callbacks)
			cb(hwnd, msg, wparam, lparam);


		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	}

	void renderer::new_frame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void renderer::end_frame()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}
