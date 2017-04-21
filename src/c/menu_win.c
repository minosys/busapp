#include <pebble.h>
#include "menu_win.h"
#include "buslist_win.h"
#include "busdata.h"
#include "src/c/dialog_message_window.h"

#define NUM_MENU_SECTIONS 2
#define NUM_MENU_ICONS 3

#define NUM_BUS_PICTOS 7

#define NUM_FIRST_MENU_ITEMS 2
#define NUM_SECOND_MENU_ITEMS 2
#define MAX_LENGTH 100

static Window *s_main_window;
static MenuLayer *s_menu_layer;


// Array of different stops
Station busStops[][2] = {
  {
    {0, "Restau U", "vers cesson", true},
    {1, "Danzelot", "vers cesson", true},
  },
  {
    {2, "Champs Blancs", "vers centre", false},
    {3, "Chateau vaux", "vers centre", false},
  }
};


static char busString_buffer[MAX_LENGTH];

static char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}
static void sendMessage(int selected){ 
  DictionaryIterator *out_iter;  
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  if(result == APP_MSG_OK) {
    dict_write_int(out_iter, BUS_STOP_KEY, &selected, sizeof(int), true);
    dict_write_end(out_iter);
    result = app_message_outbox_send();
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}
static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  Tuple *busString_tuple = dict_find(iter, MESSAGE_KEY_BUS_STRING_KEY);
  Tuple *error_tuple = dict_find(iter, ERROR_MESSAGE_KEY);

  if(busString_tuple) {
    char *busString = busString_tuple->value->cstring;
    snprintf(busString_buffer, sizeof(busString_buffer), "%s", busString);
    window_set_user_data(get_bus_list_window(), busString_buffer);
    window_stack_push(get_bus_list_window(), false);
  }
  else if(error_tuple) {
    char *errorMessage = error_tuple->value->cstring;
    static char msg_buffer[50];
    snprintf(msg_buffer, sizeof(msg_buffer), "%s", errorMessage);
    dialog_message_window_push(msg_buffer);
  }
    
}
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %s",translate_error(reason));
  dialog_message_window_push(translate_error(reason));
}
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    sendMessage(busStops[cell_index->section][cell_index->row].id);
}
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    case 1:
      return NUM_SECOND_MENU_ITEMS;
    default:
      return 0;
  }
}
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
        // Draw title text in the section header
        menu_cell_basic_header_draw(ctx, cell_layer, "Aller au job");
      break;
    case 1:
        // Draw title text in the section header
        menu_cell_basic_header_draw(ctx, cell_layer, "Retour du job");
      break;
  }
}
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in    
  menu_cell_basic_draw(ctx, 
                       cell_layer, 
                       busStops[cell_index->section][cell_index->row].name, 
                       busStops[cell_index->section][cell_index->row].desc, 
                       NULL);
}

//begin:  main windows (menu) window functions
static void main_window_load(Window *window) { 

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = PBL_IF_RECT_ELSE(menu_get_header_height_callback, NULL),
    .draw_header = PBL_IF_RECT_ELSE(menu_draw_header_callback, NULL),
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
    .get_cell_height = PBL_IF_ROUND_ELSE(get_cell_height_callback, NULL),
  });

  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
  
}
static void main_window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
}
void main_window_create() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
  // Largest expected inbox and outbox message sizes
  const uint32_t inbox_size = 256;
  const uint32_t outbox_size = 64;

  // Open AppMessage
  app_message_open(inbox_size, outbox_size);
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);

}
void main_window_destroy() {
  window_destroy(s_main_window);
}
Window *get_menu_window(){
  return s_main_window;
}
//end:  main windows (menu) window functions
