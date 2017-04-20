#include <pebble.h>
#include "menu_win.h"

#define NUM_MENU_SECTIONS 2
#define NUM_MENU_ICONS 3

#define NUM_BUS_PICTOS 7

#define NUM_FIRST_MENU_ITEMS 2
#define NUM_SECOND_MENU_ITEMS 2

static Window *s_main_window, *bus_list_window;
static MenuLayer *s_menu_layer;
static GBitmap *s_menu_icons[NUM_MENU_ICONS];
static GBitmap *s_background_bitmap;

static BitmapLayer *s_picto_layer;
static GBitmap *s_picto_bitmap = NULL;

static const uint32_t BUS_PICTOS[] = {
  RESOURCE_ID_PICTO_ERR,
  RESOURCE_ID_PICTO_C1,
  RESOURCE_ID_PICTO_C4,
  RESOURCE_ID_PICTO_64,
  RESOURCE_ID_PICTO_14,
  RESOURCE_ID_PICTO_40,
  RESOURCE_ID_PICTO_41,
  RESOURCE_ID_PICTO_35
};

static TextLayer  *s_bus_name_text_layer, *s_arrive_time_text_layer;

typedef struct {
  int id;
  char name[16];  // Name of bus
  char desc[16];       // description of bus
  bool sens;
} Station;
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

enum BusKey {
  BUS_ICON_KEY = 0x0,         // TUPLE_INT
  BUS_NAME_KEY = 0x1,  // TUPLE_CSTRING
  ARRIVE_TIME_KEY = 0x2,         // TUPLE_CSTRING
};
static AppSync s_sync;
static uint8_t s_sync_buffer[64];

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}
static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  switch (key) {
    case BUS_ICON_KEY:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "bus icon id : %d", new_tuple->value->uint8);
      //TODO : add images 
      if (s_picto_bitmap) {
        gbitmap_destroy(s_picto_bitmap);
      }

      s_picto_bitmap = gbitmap_create_with_resource(BUS_PICTOS[new_tuple->value->uint8]);
      bitmap_layer_set_compositing_mode(s_picto_layer, GCompOpSet);
      bitmap_layer_set_bitmap(s_picto_layer, s_picto_bitmap);
      break;

    case BUS_NAME_KEY:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "bus name : %s", new_tuple->value->cstring);
      text_layer_set_text(s_bus_name_text_layer, new_tuple->value->cstring);
      break;

    case ARRIVE_TIME_KEY:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "bus time : %s", new_tuple->value->cstring);
      text_layer_set_text(s_arrive_time_text_layer, new_tuple->value->cstring);
      break;
  }
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
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action

      APP_LOG(APP_LOG_LEVEL_DEBUG, busStops[cell_index->section][cell_index->row].name);
      window_set_user_data(bus_list_window, &busStops[cell_index->section][cell_index->row]);
      window_stack_push(bus_list_window, false);

}

static void countdown_back_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop(true);
}
static void countdown_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_BACK, countdown_back_handler);
 }

static void sendMessage(void){
  
  DictionaryIterator *out_iter;  
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  if(result == APP_MSG_OK) {
    int value = 1;
    dict_write_int(out_iter, 0, &value, sizeof(int), true);
    dict_write_end(out_iter);
    result = app_message_outbox_send();
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }

}

//*******************************************************************
//begin:  bus list window functions
static void bus_info_window_load(Window *window){
    Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  Station* selectedStation = window_get_user_data(bus_list_window);
  APP_LOG(APP_LOG_LEVEL_DEBUG, 
          "id : %c, name : %s", 
          '0' + selectedStation->id, 
          selectedStation->name
         );
  
  window_set_click_config_provider(window, countdown_click_config_provider);
  
  s_picto_layer = bitmap_layer_create(GRect(0, 10, bounds.size.w, 80));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_picto_layer));
  
  // hadi dial texte lfo9ani 
  s_bus_name_text_layer = text_layer_create(GRect(0, 90, bounds.size.w, 32));
  text_layer_set_text(s_bus_name_text_layer, "Bus Name");
  text_layer_set_text_alignment(s_bus_name_text_layer, GTextAlignmentCenter);
  //layer_add_child(window_layer, text_layer_get_layer(s_bus_name_text_layer));
  //layer_insert_below_sibling(text_layer_get_layer(s_bus_name_text_layer), bitmap_layer_get_layer(s_picto_layer));

  s_arrive_time_text_layer = text_layer_create(GRect(0, 90, bounds.size.w, 32));
  text_layer_set_text(s_arrive_time_text_layer, "Arrive Time");
  text_layer_set_text_alignment(s_arrive_time_text_layer, GTextAlignmentCenter);
  //layer_insert_below_sibling(text_layer_get_layer(s_arrive_time_text_layer), text_layer_get_layer(s_bus_name_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_arrive_time_text_layer));

  
  Tuplet initial_values[] = {
    TupletInteger(BUS_ICON_KEY, (uint8_t) 0),
    TupletCString(BUS_NAME_KEY, "Bus mok"),
    TupletCString(ARRIVE_TIME_KEY, "17:45"),
  };
  
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer),
      initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);
  
  sendMessage();
}
static void bus_info_window_unload(Window *window){}
void bus_info_window_create() {
  bus_list_window = window_create();
  window_set_window_handlers(bus_list_window, (WindowHandlers) {
    .load = bus_info_window_load,
    .unload = bus_info_window_unload,
  });
  //open msg service
  app_message_open(64, 64);
}
void bus_info_window_destroy() {
  window_destroy(bus_list_window);
  app_sync_deinit(&s_sync);
}
Window *get_bus_info_window(){
  return bus_list_window;
}
//end: bus list window functions


//begin:  main windows (menu) window functions
static void main_window_load(Window *window) { 
  // Here we load the bitmap assets
  s_menu_icons[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_BIG_WATCH);
  s_menu_icons[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_SECTOR_WATCH);
  s_menu_icons[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_BINARY_WATCH);

  // And also load the background
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_BRAINS);

  // Now we prepare to initialize the menu layer
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

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}
static void main_window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  for (int i = 0; i < NUM_MENU_ICONS; i++) {
    gbitmap_destroy(s_menu_icons[i]);
  }
  gbitmap_destroy(s_background_bitmap);
}
void main_window_create() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
}
void main_window_destroy() {
  window_destroy(s_main_window);
}
Window *get_menu_window(){
  return s_main_window;
}
//end:  main windows (menu) window functions
