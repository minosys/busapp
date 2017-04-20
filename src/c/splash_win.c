#include <pebble.h>
#include "splash_win.h"

Window *splashWindow;
GBitmap *splashImage;
BitmapLayer *splashImageLayer;
TextLayer *splashTextLayer;  
//  static const int splashText = 5;

void splash_window_load(Window *window){
  Layer *windowLayer = window_get_root_layer(window);
  
  splashImage = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON);
  splashImageLayer = bitmap_layer_create(GRect(0, 0, 144, 120));
  bitmap_layer_set_bitmap(splashImageLayer, splashImage);
  bitmap_layer_set_compositing_mode(splashImageLayer, GCompOpSet);
  layer_add_child(windowLayer, bitmap_layer_get_layer(splashImageLayer));
  
  splashTextLayer = text_layer_create(GRect(0, 120, 144, 50));
  text_layer_set_text(splashTextLayer, "SARA KELBA");
  text_layer_set_text_alignment(splashTextLayer, GTextAlignmentCenter);
  text_layer_set_font(splashTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(splashTextLayer, GColorLiberty);
  text_layer_set_size(splashTextLayer, GSize(10, 10));
  layer_add_child(windowLayer, text_layer_get_layer(splashTextLayer));
}

void splash_window_unload(Window *window){
  gbitmap_destroy(splashImage);
  bitmap_layer_destroy(splashImageLayer);
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