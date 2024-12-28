package main

import "base:runtime"
import "core:flags"
import "core:log"
import "core:mem"
import "core:os"
import "core:strings"
import gl "vendor:OpenGL"
import "vendor:glfw"
import nvg "vendor:nanovg"
import nvgl "vendor:nanovg/gl"
import "vendor:x11/xlib"

custom_flag_checker :: proc(
	model: rawptr,
	name: string,
	value: any,
	args_tag: string,
) -> (
	error: string,
) {
	if name == "window_type" {
		value := value.(string)
		if value != "docked" && value != "floating" && value != "normal" {
			error = "Window type can be only of the following: docked,floating,normal"
		}
	} else if name == "docked_location" {
		value := value.(string)
		if value != "top" && value != "bottom" {
			error = "Docked location can be only of the following: top,bottom"
		}
	}

	return
}

WORKSPACE_NONE :: 0xFFFFFFFF

window_size := [2]i32{800, 600}

logger: log.Logger

Options :: struct {
	window_type:     string `args:"required" usage:"Window type: docked,floating,normal"`,
	docked_location: string `usage:"If window type is docked: top,bottom"`,
	workspace_id:    int `usage:"Desired worspace index"`,
}

main :: proc() {
	logger = log.create_console_logger()
	defer log.destroy_console_logger(logger)
	context.logger = logger

	when ODIN_DEBUG {
		track: mem.Tracking_Allocator
		mem.tracking_allocator_init(&track, context.allocator)
		context.allocator = mem.tracking_allocator(&track)

		defer {
			if len(track.allocation_map) > 0 {
				log.errorf("=== %v allocations not freed: ===", len(track.allocation_map))
				for _, entry in track.allocation_map {
					log.errorf("- %v bytes @ %v", entry.size, entry.location)
				}
			}
			if len(track.bad_free_array) > 0 {
				log.errorf("=== %v incorrect frees: ===", len(track.bad_free_array))
				for entry in track.bad_free_array {
					log.errorf("- %p @ %v", entry.memory, entry.location)
				}
			}
			mem.tracking_allocator_destroy(&track)
		}
	}
	opt: Options
	flags.register_flag_checker(custom_flag_checker)
	flags.parse_or_exit(&opt, os.args, .Odin)

	if opt.workspace_id == 0 {
		opt.workspace_id = WORKSPACE_NONE
	}

	log.debugf("%#v", opt)

	if opt.window_type == "docked" {
		window_size = [2]i32{5120, 25}
	}

	if glfw.Init() != true {
		log.error("Failed to init GLFW.")
		return
	}
	defer glfw.Terminate()

	glfw.WindowHint(glfw.CONTEXT_VERSION_MAJOR, 3)
	glfw.WindowHint(glfw.CONTEXT_VERSION_MINOR, 3)
	glfw.WindowHint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
	glfw.WindowHint(glfw.PLATFORM_X11, true)
	glfw.WindowHint(glfw.TRANSPARENT_FRAMEBUFFER, true)
	glfw.WindowHint(glfw.VISIBLE, false)

	window_name := "Example X11 Normal window"
	if opt.window_type == "floating" {
		window_name = "Example X11 Floating window"
	} else if opt.window_type == "docked" {
		window_name = "Example X11 Docked window"
	}
	window_name_cstr := strings.clone_to_cstring(window_name)

	window := glfw.CreateWindow(window_size.x, window_size.y, window_name_cstr, nil, nil)
	if window == nil {
		log.error("Failed to create GLFW window.")
		return
	}
	defer glfw.DestroyWindow(window)

	glfw.MakeContextCurrent(window)
	gl.load_up_to(3, 3, glfw.gl_set_proc_address)
	gl.Viewport(0, 0, window_size.x, window_size.y)
	glfw.SwapInterval(1)

	glfw.SetFramebufferSizeCallback(window, FramebufferSizeCallback)

	set_x11_attributes(opt, window)

	sb := strings.builder_make()
	defer strings.builder_destroy(&sb)
	strings.write_string(&sb, "Example ")
	if opt.window_type == "docked" {
		if opt.workspace_id == WORKSPACE_NONE {
			strings.write_string(&sb, "globally")
		} else {
			strings.write_string(&sb, "at workspace ")
			strings.write_int(&sb, opt.workspace_id)
		}
		strings.write_string(&sb, " docked window located ")
		if opt.docked_location == "top" {
			strings.write_string(&sb, "top center")
		} else if opt.docked_location == "bottom" {
			strings.write_string(&sb, "bottom center")
		}
	} else if opt.window_type == "floating" {
		strings.write_string(&sb, "floating window")
	} else if opt.window_type == "normal" {
		strings.write_string(&sb, "normal window")
	}
	text := strings.to_string(sb)

	nvg_ctx := nvgl.Create({.ANTI_ALIAS, .STENCIL_STROKES, .DEBUG})
	if nvg_ctx == nil {
		log.error("Failed to initialize NanoVG.")
		return
	}
	defer nvg.DeleteInternal(nvg_ctx)

	roboto_nerd_font_data := #load("../fonts/RobotoMonoNerdFont-Regular.ttf")
	roboto_nerd_font_id := nvg.CreateFontMem(
		nvg_ctx,
		"RobotoMonoNerdFont-Regular",
		roboto_nerd_font_data,
		false,
	)

	for glfw.WindowShouldClose(window) != true {
		gl.ClearColor(0.0, 0.0, 0.0, 0.0)
		gl.Clear(gl.COLOR_BUFFER_BIT | gl.STENCIL_BUFFER_BIT)

		nvg.BeginFrame(nvg_ctx, auto_cast window_size.x, auto_cast window_size.y, 1.0)

		element_width := f32(window_size.x) * 2 / 3
		element_height := f32(window_size.y) * 2 / 3
		start_x := f32(window_size.x) / 2 - element_width / 2
		start_y := f32(window_size.y) / 2 - element_height / 2

		nvg.BeginPath(nvg_ctx)
		nvg.Rect(nvg_ctx, start_x, start_y + 5, element_width, element_height)
		nvg.FillColor(nvg_ctx, nvg.RGBA(0, 151, 167, 255))
		nvg.Fill(nvg_ctx)

		text_height: f32 = 18
		text_width := f32(10 * len(text))
		nvg.FontSize(nvg_ctx, 18)
		nvg.FontFaceId(nvg_ctx, roboto_nerd_font_id)
		nvg.TextAlign(nvg_ctx, .CENTER, .MIDDLE)
		nvg.FillColor(nvg_ctx, nvg.RGB(0, 0, 0))
		nvg.Text(
			nvg_ctx,
			start_x + text_width / 2 + (element_width - text_width) / 2,
			start_y + text_height / 2 + (element_height - text_height) / 2,
			text,
		)

		nvg.EndFrame(nvg_ctx)

		glfw.SwapBuffers(window)
		glfw.PollEvents()
	}
}

FramebufferSizeCallback :: proc "c" (window: glfw.WindowHandle, width, height: i32) {
	window_size.x = width
	window_size.y = height
	gl.Viewport(0, 0, window_size.x, window_size.y)

	context = runtime.default_context()
	context.logger = logger
	log.debugf("new window size: %v", window_size)
}

set_x11_attributes :: proc(opt: Options, window: glfw.WindowHandle) {
	x11_display := glfw.GetX11Display()
	x11_window := glfw.GetX11Window(window)
	atom_window_type := xlib.InternAtom(x11_display, "_NET_WM_WINDOW_TYPE", false)

	if opt.window_type == "docked" {
		set_window_docked(x11_display, x11_window, opt.docked_location, opt.workspace_id)
	} else if opt.window_type == "floating" {
		atom_window_type_dialog := xlib.InternAtom(
			x11_display,
			"_NET_WM_WINDOW_TYPE_DIALOG",
			false,
		)
		xlib.ChangeProperty(
			x11_display,
			x11_window,
			atom_window_type,
			xlib.XA_ATOM,
			32,
			xlib.PropModeReplace,
			cast(^u8)&atom_window_type_dialog,
			1,
		)
	} else if opt.window_type == "normal" {
		atom_window_type_normal := xlib.InternAtom(
			x11_display,
			"_NET_WM_WINDOW_TYPE_NORMAL",
			false,
		)
		xlib.ChangeProperty(
			x11_display,
			x11_window,
			atom_window_type,
			xlib.XA_ATOM,
			32,
			xlib.PropModeReplace,
			cast(^u8)&atom_window_type_normal,
			1,
		)
	}

	if opt.workspace_id != WORKSPACE_NONE {
		atom_desktop := xlib.InternAtom(x11_display, "_NET_WM_DESKTOP", false)
		workspace_id := opt.workspace_id
		xlib.ChangeProperty(
			x11_display,
			x11_window,
			atom_desktop,
			xlib.Atom(6),
			32,
			xlib.PropModeReplace,
			cast(^u8)&workspace_id,
			1,
		)
	}

	xlib.MapWindow(x11_display, x11_window)
	xlib.Flush(x11_display)
}

set_window_docked :: proc(
	x11_display: ^xlib.Display,
	x11_window: xlib.Window,
	docked_location: string,
	workspace_id: int,
) {
	atom_strut_partial := xlib.InternAtom(x11_display, "_NET_WM_STRUT_PARTIAL", false)
	atom_window_type := xlib.InternAtom(x11_display, "_NET_WM_WINDOW_TYPE", false)
	atom_dock := xlib.InternAtom(x11_display, "_NET_WM_WINDOW_TYPE_DOCK", false)
	atom_state := xlib.InternAtom(x11_display, "_NET_WM_STATE", false)
	atom_state_sticky := xlib.InternAtom(x11_display, "_NET_WM_STATE_STICKY", false)

	strut := [12]i64 {
		0, // left
		0, // right
		25, // top
		0, // bottom
		0, // left_start_y
		0, // left_end_y 
		0, // right_start_y
		0, // right_end_y 
		0, // top_start_x
		0, // top_end_x (spanning the entire width)
		0, // bottom_start_x
		5120, // bottom_end_x 
	}
	if docked_location == "bottom" {
		strut = [12]i64 {
			0, // left
			0, // right
			0, // top
			25, // bottom
			0, // left_start_y
			0, // left_end_y 
			0, // right_start_y
			0, // right_end_y 
			0, // top_start_x
			0, // top_end_x (spanning the entire width)
			0, // bottom_start_x
			5120, // bottom_end_x 
		}
	}
	xlib.ChangeProperty(
		x11_display,
		x11_window,
		atom_strut_partial,
		xlib.Atom(6),
		32,
		xlib.PropModeReplace,
		cast(^u8)raw_data(strut[:]),
		12,
	)
	xlib.ChangeProperty(
		x11_display,
		x11_window,
		atom_window_type,
		xlib.XA_ATOM,
		32,
		xlib.PropModeReplace,
		cast(^u8)&atom_dock,
		1,
	)

	if workspace_id == WORKSPACE_NONE {
		xlib.ChangeProperty(
			x11_display,
			x11_window,
			atom_state,
			xlib.XA_ATOM,
			32,
			xlib.PropModeReplace,
			cast(^u8)&atom_state_sticky,
			1,
		)
	}
}
