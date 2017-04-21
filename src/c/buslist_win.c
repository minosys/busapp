#include <pebble.h>
#include "buslist_win.h"
#include "busdata.h"
#include <string.h>

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 3
#define NUM_OF_BUS_FIELDS 3

#define ID_INDEX 0
#define BUS_NAME_INDEX 1
#define BUS_TIME_INDEX 2

static int num_bus = 0;
static Window *s_bus_list_window;
static MenuLayer *s_bus_list_menu_layer;

static RawBusData* rawdata = NULL;
static Bus* busdata = NULL;
  

static const uint32_t BUS_PICTOS[] = {
  RESOURCE_ID_PICTO_ERR_S,
  RESOURCE_ID_PICTO_C1_S,
  RESOURCE_ID_PICTO_C4_S,
  RESOURCE_ID_PICTO_64_S,
  RESOURCE_ID_PICTO_14_S,
  RESOURCE_ID_PICTO_40_S,
  RESOURCE_ID_PICTO_41_S,
  RESOURCE_ID_PICTO_35_S,
  RESOURCE_ID_PICTO_50_S,
  RESOURCE_ID_PICTO_32_S
};
static GBitmap *s_picto_bitmap = NULL;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0: //return NUM_OF_BUS_FIELDS;
        if(num_bus==0) return 1;
        return num_bus ;
    default:
      return 0;
  }
}
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {  
  switch (section_index) {
    case 0:
      menu_cell_basic_header_draw(ctx, cell_layer, "liste des bus");
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  if(num_bus == 0){
    menu_cell_basic_draw(ctx, cell_layer, "NO ENTRIE RETURNED !", NULL, NULL);
    return;
  }  
  Bus* busdata = ((Bus*)data);
  if(NULL != busdata ){
      if (s_picto_bitmap) {
        gbitmap_destroy(s_picto_bitmap);
      }
      int id = atoi( busdata[cell_index->row].id );
      s_picto_bitmap = gbitmap_create_with_resource(BUS_PICTOS[id]);
      menu_cell_basic_draw(ctx, cell_layer, busdata[cell_index->row].name, busdata[cell_index->row].time, s_picto_bitmap);
  }  
}
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
//      APP_LOG(APP_LOG_LEVEL_DEBUG, "Bus Click");
}

//begin:  main windows (menu) window functions
static void bus_list_window_load(Window *window) { 
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);  
  
  //reception des bus   
  char* buffer = (char*) window_get_user_data(s_bus_list_window);
  char delimiter = '|';
  char subdelimiter = ',';
  
  rawdata = bus_raw_data_create(buffer, delimiter, subdelimiter);
  busdata = bus_list_create(rawdata);
  get_bus_list(rawdata, busdata);
  num_bus = rawdata->numberOfBus;
  
  s_bus_list_menu_layer = menu_layer_create(bounds);
  
  menu_layer_set_callbacks(s_bus_list_menu_layer, busdata, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = PBL_IF_RECT_ELSE(menu_get_header_height_callback, NULL),
    .draw_header = PBL_IF_RECT_ELSE(menu_draw_header_callback, NULL),
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
    .get_cell_height = PBL_IF_ROUND_ELSE(get_cell_height_callback, NULL),
  });

  menu_layer_set_click_config_onto_window(s_bus_list_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_bus_list_menu_layer));

}
static void bus_list_window_unload(Window *window) {
//   if (s_picto_bitmap) {
//     gbitmap_destroy(s_picto_bitmap);
//   }
  bus_destroy(rawdata, busdata);
  menu_layer_destroy(s_bus_list_menu_layer);  
}

void bus_list_window_create() {
  s_bus_list_window = window_create();
  window_set_window_handlers(s_bus_list_window, (WindowHandlers) {
    .load = bus_list_window_load,
    .unload = bus_list_window_unload,
  });
}
void bus_list_window_destroy() {
  window_destroy(s_bus_list_window);
}
Window *get_bus_list_window(){
  return s_bus_list_window;
}
//end:  main windows (menu) window functions