#include pebble.h

#define ACCEL_STEP_MS 300  
  
Window *window;	
static AppTimer *timer;
	
// Key values for AppMessage Dictionary
enum {
	STATUS_KEY = 0,	
	MESSAGE_KEY = 1,
  BRI_KEY = 10,
  HUE_KEY = 11,
  SAT_KEY = 12
};

int16_t normalize_int(int16_t data)
{
  //if (data < 0)
  //{
  //  data = - data;
  //}
  if (data > 1023)
  {
    data = 1023;
  }
  if (data < -1024)
  {
    data = -1024;
  }
  //data is between -1024 and 1023
  data += 1024;
  //data is between 0 and 2047
  data /= 8;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", data);
  return data;
}

// Write message to buffer & send
void send_message(AccelData acc){
	DictionaryIterator *iter;
	
  int16_t x = normalize_int(-acc.x);
  int16_t y = normalize_int(acc.y);
  int16_t z = normalize_int(acc.z);
  
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, STATUS_KEY, 0x1);
  dict_write_int16(iter, BRI_KEY, x);
  dict_write_int16(iter, HUE_KEY, y);
  dict_write_int16(iter, SAT_KEY, z);
  
	
	dict_write_end(iter);
  app_message_outbox_send();
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
	
	tuple = dict_find(received, STATUS_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int)tuple->value->uint32); 
	}
	
	tuple = dict_find(received, MESSAGE_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", tuple->value->cstring);
	}}

static void timer_callback(void *data) {
  AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };

  accel_service_peek(&accel);
  
  send_message(accel);
  
  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

void init(void) {
	window = window_create();
	window_stack_push(window, true);
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
  
  accel_data_service_subscribe(0, NULL);

  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);

	//send_message();
}

void deinit(void) {
	app_message_deregister_callbacks();
  accel_data_service_unsubscribe();
	window_destroy(window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}