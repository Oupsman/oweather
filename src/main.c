#include <pebble.h>
  
#define KEY_CONDITIONS 0
#define KEY_TEMPERATURE 1
#define KEY_WINDCHILL 2
#define KEY_TOWN 3
#define KEY_UPDATETIME 4
  
static GBitmap *s_bitmap_no_image;
static GBitmap *s_bitmap_sunny;
static GBitmap *s_bitmap_cloudy;
static GBitmap *s_bitmap_foggy;
static GBitmap *s_bitmap_rainy;
static GBitmap *s_bitmap_snowy;
static GBitmap *s_bitmap_windy;
static GBitmap *s_bitmap_partly;
static GBitmap *s_bitmap_fair_night;
static GBitmap *s_bitmap_partly_night;

static GBitmap *s_bluetooth;
static GBitmap *s_no_bluetooth;

static GBitmap *s_battery_level_100;
static GBitmap *s_battery_level_80;
static GBitmap *s_battery_level_60;
static GBitmap *s_battery_level_40;
static GBitmap *s_battery_level_20;
static GBitmap *s_battery_level_0;

static Window *s_main_window;

static TextLayer *s_time_layer;
static TextLayer *s_weekday_layer;
static TextLayer *s_temperature_layer;
static TextLayer *s_windchill_layer;
static TextLayer *s_static_temp_layer;
static TextLayer *s_town_layer;
static TextLayer *s_update_layer;

static BitmapLayer *s_conditions_layer;
static BitmapLayer *s_bt_layer;
static BitmapLayer *s_battery_level;

static uint8_t temperature;
static uint8_t windchill;
static uint8_t conditions;
static char time_update [] = "11h11";
static char town[129] = "";

static void load_datas (void) {
  temperature = persist_exists(KEY_TEMPERATURE) ? persist_read_int(KEY_TEMPERATURE) : 0;
  windchill = persist_exists(KEY_WINDCHILL) ? persist_read_int(KEY_WINDCHILL) : 0;
  conditions = persist_exists(KEY_CONDITIONS) ? persist_read_int(KEY_CONDITIONS) : 0;
  if (persist_exists (KEY_UPDATETIME)) {
    persist_read_string (KEY_UPDATETIME,time_update,sizeof(time_update));
  }
  if (persist_exists (KEY_TOWN)){
    persist_read_string (KEY_TOWN,town,sizeof(town));
  } 
  
  APP_LOG (APP_LOG_LEVEL_INFO,"Loading datas from storage %d %d %d %s %s",temperature,windchill,conditions,time_update,town);
}

static void save_datas(void) {
  APP_LOG (APP_LOG_LEVEL_INFO,"Writing datas to storage %d %d %d %s %s",temperature,windchill,conditions,time_update,town);

  persist_write_int (KEY_TEMPERATURE, temperature);
  persist_write_int (KEY_WINDCHILL, windchill);
  persist_write_int (KEY_CONDITIONS, conditions);
  persist_write_string (KEY_UPDATETIME, time_update);
  persist_write_string (KEY_TOWN,town);
  
}

static void update_battery_indicator () {
  
  static char log_buffer[128] = "Updating battery indicator" ;

  BatteryChargeState charge_state = battery_state_service_peek();

  APP_LOG (APP_LOG_LEVEL_INFO, "%s", log_buffer);
  
  if (charge_state.charge_percent == 100 ) {
    bitmap_layer_set_bitmap (s_battery_level, s_battery_level_100);  
  } else if (charge_state.charge_percent >= 80 ) {
    bitmap_layer_set_bitmap (s_battery_level, s_battery_level_80);  
  } else if (charge_state.charge_percent >= 60 ) {
    bitmap_layer_set_bitmap (s_battery_level, s_battery_level_60);  
  } else if (charge_state.charge_percent >= 40) {
    bitmap_layer_set_bitmap (s_battery_level, s_battery_level_40);   
  } else if (charge_state.charge_percent >= 20) {
    bitmap_layer_set_bitmap (s_battery_level, s_battery_level_20);  
  } else {
    bitmap_layer_set_bitmap (s_battery_level, s_battery_level_0);     
  }
} 

static void update_bluetooth_indicator (bool connected) {
  
  if (connected) {
    vibes_short_pulse();
    bitmap_layer_set_bitmap(s_bt_layer, s_bluetooth);    
  } else {    
    vibes_double_pulse();
    bitmap_layer_set_bitmap(s_bt_layer, s_no_bluetooth);
  } 
  
}

static void init_bt_indicator (void) {
  
  static char log_buffer[128] = "Initializing BT indicator" ;

  APP_LOG (APP_LOG_LEVEL_INFO, "%s", log_buffer);
  
  bitmap_layer_set_bitmap (s_bt_layer, s_bluetooth);
  
}

static void update_conditions (uint8_t weather_code) {

  static char log_buffer[128];


      snprintf (log_buffer, sizeof (log_buffer), "Weather code I've got from the JS : %u", weather_code);
      APP_LOG (APP_LOG_LEVEL_INFO, "%s", log_buffer);
   
      switch (weather_code){
        case 20:
        case 21:
          bitmap_layer_set_bitmap(s_conditions_layer, s_bitmap_foggy);
        break;
        case 26:
          bitmap_layer_set_bitmap(s_conditions_layer, s_bitmap_cloudy);
        break;
        
        case 27:
        case 29:
          bitmap_layer_set_bitmap(s_conditions_layer, s_bitmap_partly_night);
        break;
        
        case 28:
        case 30:
          bitmap_layer_set_bitmap(s_conditions_layer, s_bitmap_partly);
        break;
        
        case 32:
        case 34:
          bitmap_layer_set_bitmap(s_conditions_layer, s_bitmap_sunny);
        break;
        
        case 33:
          bitmap_layer_set_bitmap(s_conditions_layer, s_bitmap_fair_night);
        break;
        
        default:
          APP_LOG(APP_LOG_LEVEL_INFO, "Oops shouldn't be here !");
          bitmap_layer_set_bitmap(s_conditions_layer, s_bitmap_no_image);
        break;
      }
     
      conditions = weather_code;

}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  static char buffer_weekday [] = "Wednesday 31 december";
  
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 24h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  strftime(buffer_weekday, sizeof ("Wednesday December 31"), "%A %B %d", tick_time);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_weekday_layer, buffer_weekday);
}

static void main_window_load(Window *window) {
  
  char temp_buffer[32] = "";
  
  window_set_background_color(s_main_window, GColorBlack);
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 20, 144, 50));
  s_weekday_layer = text_layer_create (GRect(0,8,144,16));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");

  text_layer_set_background_color(s_weekday_layer, GColorBlack);
  text_layer_set_text_color(s_weekday_layer, GColorWhite);
  text_layer_set_text(s_weekday_layer, "Wednesday December 31");

  s_static_temp_layer = text_layer_create(GRect(72, 72, 26, 52));
  text_layer_set_background_color(s_static_temp_layer, GColorBlack);
  text_layer_set_text_color(s_static_temp_layer, GColorWhite);
  text_layer_set_text_alignment(s_static_temp_layer, GTextAlignmentCenter);
  text_layer_set_text(s_static_temp_layer, "T°");
  
  // Create temperature Layer
  s_temperature_layer = text_layer_create(GRect(98, 72, 46, 26));
  text_layer_set_background_color(s_temperature_layer, GColorBlack);
  text_layer_set_text_color(s_temperature_layer, GColorWhite);
  text_layer_set_text_alignment(s_temperature_layer, GTextAlignmentRight);
  //text_layer_set_text(s_temperature_layer, "-xx °C");
  snprintf(temp_buffer, sizeof(temp_buffer), "%d °C", temperature);
  APP_LOG (APP_LOG_LEVEL_INFO, "Temperature : %s", temp_buffer);
  text_layer_set_text(s_temperature_layer, temp_buffer);
  
  s_windchill_layer = text_layer_create(GRect(98, 98, 46, 26));
  text_layer_set_background_color(s_windchill_layer, GColorBlack);
  text_layer_set_text_color(s_windchill_layer, GColorWhite);
  text_layer_set_text_alignment(s_windchill_layer, GTextAlignmentRight);
  //text_layer_set_text(s_windchill_layer, "-xx °C");

  snprintf(temp_buffer, sizeof(temp_buffer), "%d °C", windchill);
  text_layer_set_text(s_temperature_layer, temp_buffer);
  
  s_town_layer = text_layer_create (GRect(0,134,144,16));
  text_layer_set_background_color (s_town_layer,GColorBlack);
  text_layer_set_text_color (s_town_layer,GColorWhite);
  text_layer_set_text_alignment(s_town_layer, GTextAlignmentCenter);
  text_layer_set_text(s_town_layer, town);
  
  s_update_layer = text_layer_create (GRect(0,148,144,16));
  text_layer_set_background_color (s_update_layer,GColorBlack);
  text_layer_set_text_color (s_update_layer,GColorWhite);
  text_layer_set_text_alignment(s_update_layer, GTextAlignmentRight);
  text_layer_set_text(s_update_layer, time_update);
  
  s_conditions_layer = bitmap_layer_create(GRect (0,64,72,72));
  bitmap_layer_set_bitmap (s_conditions_layer, s_bitmap_no_image);

  s_battery_level  = bitmap_layer_create (GRect (110,0,25,10));
 
  s_bt_layer = bitmap_layer_create (GRect (137,0,7,10));
  
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);

  text_layer_set_font(s_weekday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  text_layer_set_font (s_temperature_layer,fonts_get_system_font (FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment (s_temperature_layer,GTextAlignmentLeft);
                       
  text_layer_set_font (s_windchill_layer,fonts_get_system_font (FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment (s_windchill_layer,GTextAlignmentLeft);
                              
  text_layer_set_font(s_static_temp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_static_temp_layer, GTextAlignmentLeft);
  
  text_layer_set_font(s_town_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_update_layer, GTextAlignmentLeft);
  
  //text_layer_set_text_alignment(s_static_temp_layer, GTextAlignmentLeft);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weekday_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_temperature_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_windchill_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_static_temp_layer));         
      
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_town_layer));     
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_update_layer));    
  
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_conditions_layer));     
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_battery_level));         
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bt_layer));  
  
  // Make sure the time is displayed from the start
  
  update_conditions (conditions);
  
  update_time();
  
  init_bt_indicator ();
  
  update_battery_indicator();
  
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_windchill_layer);
  text_layer_destroy(s_temperature_layer);
  text_layer_destroy(s_town_layer);
  text_layer_destroy(s_static_temp_layer);
  text_layer_destroy(s_update_layer);

  bitmap_layer_destroy(s_conditions_layer);
  bitmap_layer_destroy(s_battery_level);
  bitmap_layer_destroy(s_bt_layer);
  
  gbitmap_destroy (s_bitmap_no_image);
  gbitmap_destroy (s_bitmap_sunny);
  gbitmap_destroy (s_bitmap_partly);
  gbitmap_destroy (s_bitmap_partly_night);
  gbitmap_destroy (s_bitmap_fair_night);
  gbitmap_destroy (s_bitmap_cloudy);
  gbitmap_destroy (s_bitmap_foggy);
  
  gbitmap_destroy (s_bluetooth);
  gbitmap_destroy (s_no_bluetooth);
  
  gbitmap_destroy (s_battery_level_100);
  gbitmap_destroy (s_battery_level_80);
  gbitmap_destroy (s_battery_level_60);
  gbitmap_destroy (s_battery_level_40);
  gbitmap_destroy (s_battery_level_20);
  gbitmap_destroy (s_battery_level_0);
  
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every ten minutes
  if(tick_time->tm_min % 10 == 0) {
    APP_LOG (APP_LOG_LEVEL_INFO, "Updating weather");
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

  static unsigned int weather_code;
  // Store incoming information
  static char temperature_buffer[8];
  static char feelslike_buffer[8];
  static char town_buffer[64];
  time_t temp = time(NULL); 

  struct tm *tick_time = localtime(&temp);
  //static char conditions_buffer[32];
  //static char weather_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);
  
  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
      case KEY_TEMPERATURE:
        snprintf(temperature_buffer, sizeof(temperature_buffer), "%d °C", (int)t->value->int32);
        text_layer_set_text(s_temperature_layer, temperature_buffer);
        temperature = t->value->int32;
      
      break;
      case KEY_CONDITIONS:
        weather_code = (unsigned int)t->value->uint8;
        update_conditions (weather_code);      
        strftime(time_update, sizeof("00:00"), "%H:%M", tick_time);
        text_layer_set_text(s_update_layer, time_update);
      break;
      case KEY_WINDCHILL:
        snprintf(feelslike_buffer, sizeof(feelslike_buffer), "%d °C", (int)t->value->int32);
        text_layer_set_text(s_windchill_layer, feelslike_buffer);
        windchill = t->value->int32;
      
      break;
      case KEY_TOWN:
        snprintf(town_buffer, sizeof(town_buffer), "%s", t->value->cstring);
        text_layer_set_text (s_town_layer,town_buffer);
        strcpy (town,t->value->cstring);
      break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }
    // Look for next item
    t = dict_read_next(iterator);
  }

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

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // Load saved datas 
  
  load_datas();
  
  // Load all images
  s_bitmap_no_image = gbitmap_create_with_resource(RESOURCE_ID_IMG_NO_IMAGE);
  s_bitmap_sunny = gbitmap_create_with_resource(RESOURCE_ID_IMG_SUNNY);
  s_bitmap_partly = gbitmap_create_with_resource(RESOURCE_ID_IMG_PARTLY);
  s_bitmap_fair_night = gbitmap_create_with_resource(RESOURCE_ID_IMG_FAIR_NIGHT);
  s_bitmap_partly_night = gbitmap_create_with_resource(RESOURCE_ID_IMG_CLOUD_NIGHT);
  s_bitmap_cloudy = gbitmap_create_with_resource(RESOURCE_ID_IMG_CLOUDY);
  s_bitmap_foggy = gbitmap_create_with_resource(RESOURCE_ID_IMG_FOGGY);
  s_bluetooth = gbitmap_create_with_resource (RESOURCE_ID_IMG_BTSIGN);
  s_no_bluetooth = gbitmap_create_with_resource (RESOURCE_ID_IMG_NO_BT);
  s_battery_level_100 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_100);
  s_battery_level_80 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_80);
  s_battery_level_60 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_60);
  s_battery_level_40 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_40);
  s_battery_level_20 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_20);
  s_battery_level_0 = gbitmap_create_with_resource (RESOURCE_ID_IMG_BATTERY_0);
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register battery state change service
  
  battery_state_service_subscribe(&update_battery_indicator);
  bluetooth_connection_service_subscribe(&update_bluetooth_indicator);
  
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe(); 
  
  save_datas();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
