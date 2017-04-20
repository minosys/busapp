#include <pebble.h>
#include "splash_win.h"
#include "menu_win.h"
#include "test.h"

static void init() {
  // splash_window_create();
  // window_stack_push(get_splash_window(),true);
  // main_window_create();
  bus_list_window_create();
  window_stack_push(get_bus_list_window(), true);
}

static void deinit() {
  // splash_window_destroy();
  // main_window_destroy();
  bus_list_window_destroy();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}