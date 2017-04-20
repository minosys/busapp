#include <pebble.h>
#include "test.h"
#include "busdata.h"
#include <string.h>

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 3
#define NUM_OF_BUS_FIELDS 3

#define ID_INDEX 0
#define BUS_NAME_INDEX 1
#define BUS_TIME_INDEX 2

static Bus* bus;

static int num_bus = 0;
static Window *s_bus_list_window;
static MenuLayer *s_bus_list_menu_layer;

char * strtok(s, delim)	register char *s;	register const char *delim;{
	register char *spanp;
	register int c, sc;
	char *tok;
	static char *last;

	if (s == NULL && (s = last) == NULL)
		return (NULL);
	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		last = NULL;
		return (NULL);
	}
	tok = s - 1;
	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

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
// static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
//   if(num_bus == 0){
//     menu_cell_basic_draw(ctx, cell_layer, "NO ENTRIE RETURNED !", NULL, NULL);
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "nombre des bus : %d", num_bus);
//     return;
//   }  
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "nombre des bus : %d", num_bus);
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "row index : %d", cell_index->row);


//   char* busInfos = data_processor_get_string(data_processor_get_global());
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "before split : busInfos=[%s]\n", busInfos);
//   //prints : before split : busInfos=[1,amine,17:55]
//   if(strlen(busInfos) == 0 ) return;
  
//   char* id = strtok(busInfos,",");
//   char* busName = strtok(NULL,",");
//   char* busTime = strtok(NULL,",");
    
// //   char* id = "test";
// //   char* busName = "test";
// //   char* busTime = "test";
  
//   if (id!=NULL && busName!=NULL && busTime!=NULL)
//     {
//         APP_LOG(APP_LOG_LEVEL_DEBUG, "after split :");
//         APP_LOG(APP_LOG_LEVEL_DEBUG, "id = [%s]\n",  id);
//         APP_LOG(APP_LOG_LEVEL_DEBUG, "busName = [%s]\n",  busName);
//         APP_LOG(APP_LOG_LEVEL_DEBUG, "busTime = [%s]\n",  busTime);
        
//         menu_cell_basic_draw(ctx, 
//                        cell_layer, 
//                        busName, 
//                        busTime, 
//                        NULL);
//     }
//   else{
//         menu_cell_basic_draw(ctx, 
//                              cell_layer, 
//                              "NaN", 
//                              "NaN", 
//                              NULL);
//   }
  
// }
  
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  //Bus* buses = data;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "bus[%d].name = '%s'.", 0, buses[0].name);
  if(num_bus == 0){
    menu_cell_basic_draw(ctx, cell_layer, "NO ENTRIE RETURNED !", NULL, NULL);
    return;
  }  
  
  Bus* buses = ((Bus*)data);
  if(NULL != buses ){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "bus[%d].name = '%s'.", cell_index->row, buses[0].name); // bus[cell_index->row].name, bus[cell_index->row].time); 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "bus[%d].time = '%s'.", cell_index->row, buses[0].time);
  }  
}
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Bus Click");
}

//begin:  main windows (menu) window functions
static void bus_list_window_load(Window *window) { 
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);  
  
  //reception des bus   
  char buf[] ="1,amine,17:55|88,salim,18:44|3,karim,19:00|88,salim,18:44|3,karim,19:00"; //window_get_user_data(s_bus_list_window); 
  bus_data_init(buf, '|');
  num_bus = bus_data_count(bus_data_get_global());
  printf("number of buses: %d", num_bus);
  print_bus_list(bus_data_get_buses());
  
  //window_set_user_data(s_bus_list_window, bus_data_get_buses());  
  s_bus_list_menu_layer = menu_layer_create(bounds);
  
  menu_layer_set_callbacks(s_bus_list_menu_layer, bus_data_get_buses()  , (MenuLayerCallbacks){
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
  menu_layer_destroy(s_bus_list_menu_layer);
  bus_data_deinit();
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