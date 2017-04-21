#include <pebble.h>
#include "splash_win.h"

Window *splashWindow;
GBitmap *splashImage;
BitmapLayer *splashImageLayer;
TextLayer *splashTextLayer, *developerTextLayer;  
//  static const int splashText = 5;

void timer_callback(void *data) {
  window_stack_pop(true);
}
void splash_window_load(Window *window){
  Layer *windowLayer = window_get_root_layer(window);
  //GRect bounds = layer_get_frame(windowLayer);

  splashImage = gbitmap_create_with_resource(RESOURCE_ID_BUS_LOGO);
  splashImageLayer = bitmap_layer_create(GRect(0, 0, 144, 120));
  bitmap_layer_set_bitmap(splashImageLayer, splashImage);
  bitmap_layer_set_compositing_mode(splashImageLayer, GCompOpSet);
  layer_add_child(windowLayer, bitmap_layer_get_layer(splashImageLayer));
  
  splashTextLayer = text_layer_create(GRect(0, 120, 144, 50));
  text_layer_set_text(splashTextLayer, "BUS APP");
  text_layer_set_text_alignment(splashTextLayer, GTextAlignmentCenter);
  text_layer_set_font(splashTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_color(splashTextLayer, GColorCobaltBlue);
  layer_add_child(windowLayer, text_layer_get_layer(splashTextLayer));
  
  developerTextLayer = text_layer_create(GRect(0, 146, 144, 50));
  text_layer_set_text(developerTextLayer, "by minodev");
  text_layer_set_text_alignment(developerTextLayer, GTextAlignmentRight);
  text_layer_set_font(developerTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_color(developerTextLayer, GColorBlack);
  layer_add_child(windowLayer, text_layer_get_layer(developerTextLayer));
  
  app_timer_register(2000, (AppTimerCallback) timer_callback, NULL);

}

void splash_window_unload(Window *window){
  gbitmap_destroy(splashImage);
  bitmap_layer_destroy(splashImageLayer);
  text_layer_destroy(developerTextLayer);
  text_layer_destroy(splashTextLayer);  
}

void splash_window_destroy(){
  window_destroy(splashWindow);
}
  
Window *get_splash_window(){
  return splashWindow;
}
  
void splash_window_create(){
  splashWindow = window_create();
  window_set_window_handlers(splashWindow, (WindowHandlers){
    .load = splash_window_load,
    .unload = splash_window_unload
  });
}