#include <pebble.h>

#define KEY_MESSAGE 0

static Window *s_window;
static TextLayer *s_time_layer;
static TextLayer *s_message_layer;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // TODO
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void update_time() {

  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer containing hh:mm
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // calculate centered - horizontally/vertically in top half - rect based on text/font
  GPoint left_center = GPoint(0, bounds.size.h / 4);
  GRect bounding_box = GRect(left_center.x, left_center.y, bounds.size.w, 42 + 16);

  GFont time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  GTextOverflowMode time_overflow = GTextOverflowModeTrailingEllipsis;
  GTextAlignment time_alignment = GTextAlignmentCenter;

  GSize optimal_size = graphics_text_layout_get_content_size("00:00", time_font, bounding_box, time_overflow, time_alignment);
  GPoint optimal_point = GPoint(left_center.x + ((bounds.size.w - optimal_size.w) / 2), left_center.y - (optimal_size.h / 2));
  optimal_point.y += PBL_IF_ROUND_ELSE(6, -6); // fudge factor due to weird font rendering; tweak vertical alignment for Round

  // Create and add time TextLayer
  s_time_layer = text_layer_create(GRect(optimal_point.x, optimal_point.y, optimal_size.w, optimal_size.h));
  //text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, time_font);
  
  text_layer_set_overflow_mode(s_time_layer, time_overflow);
  text_layer_set_text_alignment(s_time_layer, time_alignment);

  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // Create and add message TextLayer filling bottom half of screen
  GFont message_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_message_layer = text_layer_create(GRect(0, bounds.size.h / 2, bounds.size.w, bounds.size.h / 2));
  text_layer_set_text(s_message_layer, "Loading...");
  text_layer_set_font(s_message_layer, message_font);

  text_layer_set_overflow_mode(s_message_layer, time_overflow);
  text_layer_set_text_alignment(s_message_layer, time_alignment);

  text_layer_set_background_color(s_message_layer, GColorBlack);
  text_layer_set_text_color(s_message_layer, GColorWhite);

  layer_add_child(window_layer, text_layer_get_layer(s_message_layer));

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

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService (MINUTE_UNIT in order to save on battery usage)
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
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
  