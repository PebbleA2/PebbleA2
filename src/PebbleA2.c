#include <pebble.h>

static Window *s_window;
static TextLayer *s_time_layer;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // calculate centered rect based on text/font
  GPoint left_center = GPoint(0, bounds.size.h / 2);
  GRect bounding_box = GRect(left_center.x, left_center.y, bounds.size.w, 42 + 16);

  GFont time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  GTextOverflowMode time_overflow = GTextOverflowModeTrailingEllipsis;
  GTextAlignment time_alignment = GTextAlignmentCenter;

  GSize optimal_size = graphics_text_layout_get_content_size("00:00", time_font, bounding_box, time_overflow, time_alignment);
  GPoint optimal_point = GPoint(left_center.x + ((bounds.size.w - optimal_size.w) / 2), left_center.y - (optimal_size.h / 2));
  optimal_point.y -= 6; // fudge factor due to weird font rendering

  s_time_layer = text_layer_create(GRect(optimal_point.x, optimal_point.y, optimal_size.w, optimal_size.h));
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, time_font);
  
  text_layer_set_overflow_mode(s_time_layer, time_overflow);
  text_layer_set_text_alignment(s_time_layer, time_alignment);

  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
}

static void init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  deinit();
}
  