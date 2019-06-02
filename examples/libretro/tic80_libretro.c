#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <tic.h>
#include "libretro.h"

#define TIC_FRAMERATE 60
#define TIC_FREQUENCY 44100

static uint32_t *frame_buf;
static struct retro_log_callback logging;
static retro_log_printf_t log_cb;
tic80* tic;
static struct
{
	bool quit;
} state =
{
	.quit = false,
};

/**
 * TIC-80 callback; Requests the content to exit.
 */
static void tic80_libretro_exit()
{
	state.quit = true;
}

/** 
 * TIC-80 callback; Report an error from the TIC-80 cart.
 */
static void tic80_libretro_error(const char* info) {
	log_cb(RETRO_LOG_ERROR, info);
}

/**
 * TIC-80 callback; Report a trace log from the TIC-80 cart.
 */
static void tic80_libretro_trace(const char* text, u8 color) {
	log_cb(RETRO_LOG_INFO, text);
}

static void tic80_libretro_fallback_log(enum retro_log_level level, const char *fmt, ...)
{
	(void)level;
	va_list va;
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
}

/**
 * libretro callback; Global initialization.
 */
void retro_init(void)
{
	frame_buf = calloc(320 * 240, sizeof(uint32_t));
}

/**
 * libretro callback; Global deinitialization.
 */
void retro_deinit(void)
{
	// Try to force another unload of the game.
	retro_unload_game();

	// Clear out the frame buffer.
	free(frame_buf);
	frame_buf = NULL;
}

/**
 * libretro callback; Retrieves the internal libretro API version.
 */
unsigned retro_api_version(void)
{
	return RETRO_API_VERSION;
}

/**
 * libretro callback; Reports device changes.
 */
void retro_set_controller_port_device(unsigned port, unsigned device)
{
	log_cb(RETRO_LOG_INFO, "[TIC-80] Plugging device %u into port %u.\n", device, port);
}

/**
 * libretro callback; Retrieves information about the core.
 */
void retro_get_system_info(struct retro_system_info *info)
{
	memset(info, 0, sizeof(*info));
	info->library_name     = TIC_NAME;
	info->library_version  = TIC_VERSION_LABEL;
	info->need_fullpath    = false;
	info->valid_extensions = "tic";
}

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

/**
 * libretro callback; Get information about the desired audio and video.
 */
void retro_get_system_av_info(struct retro_system_av_info *info)
{
	info->timing = (struct retro_system_timing) {
		.fps = TIC_FRAMERATE,
		.sample_rate = TIC_FREQUENCY,
	};

	info->geometry = (struct retro_game_geometry) {
		.base_width   = TIC80_FULLWIDTH,
		.base_height  = TIC80_FULLHEIGHT,
		.max_width    = TIC80_FULLWIDTH,
		.max_height   = TIC80_FULLHEIGHT,
		.aspect_ratio = (float)TIC80_WIDTH / (float)TIC80_HEIGHT,
	};
}

/**
 * libretro callback; Sets up the environment callback.
 */
void retro_set_environment(retro_environment_t cb)
{
	// Update the environment callback to make environment calls.
	environ_cb = cb;

	// TIC-80 requires a cartridge.
	bool no_content = false;
	cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_content);

	// Set up the logging interface.
	if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging)) {
		log_cb = logging.log;
	}
	else {
		log_cb = tic80_libretro_fallback_log;
	}
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
	audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
	audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
	input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
	input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
	video_cb = cb;
}

void retro_reset(void)
{
	// TODO: Allow reseting the cartridge.
}

/**
 * libretro callback; Load the labels for the input buttons.
 */
void tic80_libretro_input_descriptors() {
	struct retro_input_descriptor desc[] = {
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "D-Pad Left" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "D-Pad Up" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "D-Pad Down" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "A" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "B" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "Y" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "X" },

		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "D-Pad Left" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "D-Pad Up" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "D-Pad Down" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B, "A" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "B" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X, "Y" },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "X" },

		{ 0 },
	};

	environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);
}

/**
 * Retrieve gamepad information from libretro.
 */
static void tic80_libretro_update_gamepad(tic80_gamepad* gamepad, int player) {
	gamepad->up = input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
	gamepad->down = input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
	gamepad->left = input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
	gamepad->right = input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);

	// A/B and X/Y are switched in TIC-80
	gamepad->a = input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
	gamepad->b = input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
	gamepad->x = input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y);
	gamepad->y = input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X);
}

/**
 * Retrieve gamepad information from libretro.
 */
static void tic80_libretro_update_mouse(tic80_mouse* mouse) {
	// TODO: Add mouse X and Y
	mouse->left = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
	mouse->right = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
	mouse->middle = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE);
}

static void tic80_libretro_update(retro_usec_t usec)
{
	// Make sure we only act when a TIC-80 environment is available.
	if (tic) {
		// Let libretro know that we need updated input states.
		input_poll_cb();

		// Port the libretro inputs to TIC-80.
		tic80_input input;
		tic80_libretro_update_gamepad(&input.gamepads.first, 0);
		tic80_libretro_update_gamepad(&input.gamepads.second, 1);
		//tic80_libretro_update_gamepad(&input.gamepads.third, 2);
		//tic80_libretro_update_gamepad(&input.gamepads.fourth, 3);
		tic80_libretro_update_mouse(&input.mouse);
		// TODO: Add keyboard support

		// Update the game state.
		tic80_tick(tic, input);
	}
}

/**
 * Convert between argb8888 and abgr8888.
 *
 * TODO: Use directly from libretro-common instead?
 */
void tic80_libretro_conv_argb8888_abgr8888(void *output_, const void *input_,
      int width, int height,
      int out_stride, int in_stride)
{
	int h, w;
	const uint32_t *input = (const uint32_t*)input_;
	uint32_t *output = (uint32_t*)output_;

	for (h = 0; h < height; h++, output += out_stride >> 2, input += in_stride >> 2) {
		for (w = 0; w < width; w++) {
			uint32_t col = input[w];
			output[w] = ((col << 16) & 0xff0000) | 
				((col >> 16) & 0xff) |
				(col & 0xff00ff00);
		}
	}
}

/**
 * Draw the screen.
 */
static void tic80_libretro_draw(void)
{
	// Check if there is a screen to render.
	if (tic && tic->screen) {
		// TIC-80 uses ABGR8888, so we need to convert it.
		tic80_libretro_conv_argb8888_abgr8888(frame_buf, tic->screen, TIC80_FULLWIDTH, TIC80_FULLHEIGHT, TIC80_FULLWIDTH << 2, TIC80_FULLWIDTH << 2);

		// Render to the screen.
		video_cb(frame_buf, TIC80_FULLWIDTH, TIC80_FULLHEIGHT, TIC80_FULLWIDTH << 2);
	}
}

/**
 * libretro callback; Play the audio.
 */
void tic80_libretro_audio() {
	// Only render audio if there is a TIC-80 environment.
	if (tic) {
		// Tell libretro about the samples.
		audio_batch_cb(tic->sound.samples, tic->sound.count / 2);
	}
}

/**
 * libretro callback; Render the screen and play the audio.
 */
void retro_run(void)
{
	// Check if we are to quit.
	if (state.quit) {
		// If the game is still running, ask it to shut it down.
		if (tic) {
			environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
			retro_unload_game();
		}
		return;
	}

	// Render the screen.
	tic80_libretro_draw();

	// Play the audio.
	tic80_libretro_audio();
}

/**
 * libretro callback; Set the current state of the audio.
 */
void tic80_libretro_audio_set_state(bool enabled) {
	if (enabled) {
		log_cb(RETRO_LOG_INFO, "[TIC-80] Audio: Enabled\n");
	}
	else {
		log_cb(RETRO_LOG_INFO, "[TIC-80] Audio: Disabled\n");
	}
}

/**
 * libretro callback; Load a game.
 */
bool retro_load_game(const struct retro_game_info *info)
{
	// Pixel format.
	enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
	if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt)) {
		log_cb(RETRO_LOG_INFO, "[TIC-80] RETRO_PIXEL_FORMAT_XRGB8888 is not supported.\n");
		return false;
	}

	// Frame timer.
	struct retro_frame_time_callback frame_cb = {
		tic80_libretro_update,
		1000000 / TIC_FRAMERATE
	};
	if (!environ_cb(RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK, &frame_cb)) {
		log_cb(RETRO_LOG_INFO, "[TIC-80] Failed to set frame time callback.\n");
		return false;
	}

	// Update the input button descriptions.
	tic80_libretro_input_descriptors();

	// Check for the content.
	if (!info) {
		log_cb(RETRO_LOG_ERROR, "[TIC-80] No content information provided.\n");
		return false;
	}
	if (info->data == NULL) {
		log_cb(RETRO_LOG_ERROR, "[TIC-80] No content data provided.\n");
		return false;
	}

	// Ensure a game is not loaded before loading.
	retro_unload_game();

	// Set up the TIC-80 environment.
	tic = tic80_create(TIC_FREQUENCY);
	state.quit = false;
	tic->callback.exit = tic80_libretro_exit;
	tic->callback.error = tic80_libretro_error;
	tic->callback.trace = tic80_libretro_trace;

	// Load the content.
	tic80_load(tic, (void*)(info->data), info->size);
	if (!tic) {
		log_cb(RETRO_LOG_ERROR, "[TIC-80] Content loaded, but failed to load game.\n");
		return false;
	}

	return true;
}

void retro_unload_game(void)
{
	// Close the game if it's loaded.
	state.quit = true;
	if (tic != NULL) {
		tic80_delete(tic);
		tic = NULL;
	}
}

unsigned retro_get_region(void)
{
	return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
	return retro_load_game(info);
}

size_t retro_serialize_size(void)
{
	return 0;
}

bool retro_serialize(void *data_, size_t size)
{
	return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
	return false;
}

void *retro_get_memory_data(unsigned id)
{
	(void)id;
	return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
	(void)id;
	return 0;
}

void retro_cheat_reset(void)
{
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
	(void)index;
	(void)enabled;
	(void)code;
}
