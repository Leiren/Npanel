#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "log.h"

#include <vector>
/*
	window class manages the visibility of a window, and also checks for error when being created.
*/

struct Vec2
{
	float x, y;
	constexpr Vec2(float _x, float _y) : x(_x), y(_y) {}
	explicit operator ImVec2() const { return *(ImVec2 *)(this); }

	constexpr Vec2(const Vec2 &oldvec) : x(oldvec.x), y(oldvec.y) {}
	Vec2 &operator=(const Vec2 &oldvec)
	{
		x = oldvec.x;
		y = oldvec.y;
		return *this;
	}
};
enum class DockDir
{
	none,
	left,
	right,
	main,
	down
};

struct WindowProps
{

	static constexpr const char *DEFAULT_WINDOW_TITLE = "UNTITLED WINDOW";
	static constexpr const float DEFAULT_WINDOW_X = 0.0f;
	static constexpr const float DEFAULT_WINDOW_Y = 0.0f;
	static constexpr const float DEFAULT_WINDOW_WIDTH = 200.0f;
	static constexpr const float DEFAULT_WINDOW_HEIGHT = 600.0f;

	const char *Title;
	Vec2 Position;
	Vec2 Size;
	ImGuiWindowFlags WindowFlags;
	ImGuiCond PositionFlags;
	ImGuiCond SizeFlags;
	DockDir dock_dir;

	constexpr WindowProps(const char *title) : WindowProps(
												   title, ImGuiWindowFlags_None, ImGuiCond_FirstUseEver, ImGuiCond_FirstUseEver) {}

	constexpr WindowProps(
		const char *title,
		ImGuiWindowFlags windowflags,
		ImGuiCond positionflags,
		ImGuiCond sizeflags)
		: WindowProps(title, DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
					  windowflags, positionflags, sizeflags, DockDir::none)
	{
	}

	constexpr WindowProps(
		const char *title,
		float x,
		float y,
		float width,
		float height,
		ImGuiWindowFlags windowflags,
		ImGuiCond positionflags,
		ImGuiCond sizeflags,
		DockDir dir)
		: Title(title), Position(x, y), Size(width, height), WindowFlags(windowflags), PositionFlags(positionflags), SizeFlags(sizeflags), dock_dir(dir)
	{
	}
	constexpr WindowProps(const WindowProps &winargs) : Title(winargs.Title), Position(winargs.Position), Size(winargs.Size), WindowFlags(winargs.WindowFlags),
														PositionFlags(winargs.PositionFlags), SizeFlags(winargs.SizeFlags), dock_dir(winargs.dock_dir)
	{
	}
};

class Window
{

public:
	static ImGuiID dock_main_id;
	static ImGuiID dock_id_left;
	static ImGuiID dock_id_right;
	static ImGuiID dock_id_bottom;
	bool re_dock;
	Window(WindowProps winargs);
	inline void tick()
	{
		if (visible_ptr != nullptr && !(*visible_ptr))
			return;

		if (window_properties.dock_dir != DockDir::none && re_dock)
		{
			re_dock = false;
			switch (window_properties.dock_dir)
			{
			case DockDir::left:
				ImGui::DockBuilderDockWindow(window_properties.Title, dock_id_left);
				break;
			case DockDir::right:
				ImGui::DockBuilderDockWindow(window_properties.Title, dock_id_right);
				break;
			case DockDir::main:
				ImGui::DockBuilderDockWindow(window_properties.Title, dock_main_id);
				break;
			case DockDir::down:
				ImGui::DockBuilderDockWindow(window_properties.Title, dock_id_bottom);
				break;
			default:
				break;
			}
		}

		ImGui::SetNextWindowPos((ImVec2)(window_properties.Position), window_properties.PositionFlags);
		ImGui::SetNextWindowSize((ImVec2)(window_properties.Size), window_properties.SizeFlags);
		int styles_to_pop = onStyle();
		if (!ImGui::Begin(window_properties.Title, visible_ptr, window_properties.WindowFlags))
		{
			// when window is collapsed
			ImGui::End();
			ImGui::PopStyleVar(styles_to_pop);
			return;
		}
		onFrame();
		ImGui::End();
		ImGui::PopStyleVar(styles_to_pop);
	}
	inline const char *title() const
	{
		return window_properties.Title;
	}

protected:
	WindowProps window_properties;

	// Each Frame this gets called if visible
	virtual void onFrame() = 0;

	virtual int onStyle() { return 0; }

private:
	bool *visible_ptr;

	bool _visible; // true

public:
	inline void hide()
	{
		_visible = false;
	}

	inline void show()
	{
		_visible = true;
	}
	inline bool visible() { return _visible; }

	inline void toggleVisibility()
	{
		_visible = !_visible;
	}

	inline void hideCloseButton()
	{
		visible_ptr = NULL;
	}

	inline void dock(DockDir dir)
	{
		window_properties.dock_dir = dir;
		re_dock = true;
	}
};