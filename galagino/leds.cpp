/*
 * leds.cpp
 * 
 * driver for the seven optional ws2812b marquee leds in galagino
 */

/* 
 * TODO: Test
 * https://github.com/JSchaenzle/ESP32-NeoPixel-WS2812-RMT
 */

#include "leds.h"

#ifdef NUM_LEDS

#include "emulation.h"

#ifndef SINGLE_MACHINE
extern signed char menu_sel;
#endif

CRGB leds[NUM_LEDS];
unsigned char led_state = 0;       // state set by game (usually video driver)

void leds_init() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS);
}

void leds_state_reset(void) {
  led_state = 0;  
}

void leds_check_galaga_sprite(struct sprite_S *spr) {
  if(game_started) {
    // this is a visible sprite. Extract information for LEDs if needed
    // printf("SPR %d = %d @ %d %d\n", idx, spr.code, spr.x, spr.y);

    // sprite code 6 at y == 257 is the players ship,
    // below are remaining ships (which we ignore)
    if(spr->code >= 0 && spr->code <= 7 && spr->y <= 257) {
      // printf("SHIP %d/%d @ %d %d\n", spr->code, spr->color, spr->x, spr->y);

      if(spr->y == 257 && spr->code == 6)
      	led_state = 1;    // normal fighter
	    
      // color 9 is regular ship color, color 7 is red captured one
      if(spr->color == 7)
      	led_state = 2;    // captured fighter
      
      if(spr->flags & 0x08)
        ;
      
      // spr->code = 0x66;
    }

    // check for exploding enemies
    if(spr->code >= 0x41 && spr->code <= 0x4b) {
      // printf("EXPLODE %d @ %d/%d\n", spr->code-0x41, spr->x, spr->y);
      led_state = 3 + spr->code-0x41;
    }
    
    // check for score marks
    //	  if(spr->code >= 0x34 && spr->code <= 0x3a)
    //	    printf("SCORE %d @ %d/%d\n", spr->code-0x34, spr->x, spr->y);
  }
}

void leds_update(void) {
#ifndef SINGLE_MACHINE
  if(machine == MCH_MENU) {
    // process leds for menu
    static const CRGB menu_leds[][NUM_LEDS] = {
      // static colors that somehow resemble the games theme
#ifdef ENABLE_PACMAN      
      { LED_BLUE, LED_BLACK, LED_YELLOW, LED_YELLOW, LED_YELLOW, LED_BLACK, LED_BLUE },
#endif
#ifdef ENABLE_GALAGA
      { LED_RED, LED_BLUE, LED_WHITE, LED_WHITE, LED_WHITE, LED_BLUE, LED_RED },
#endif
#ifdef ENABLE_DKONG      
      { LED_BLACK, LED_YELLOW, LED_RED, LED_RED, LED_RED, LED_YELLOW, LED_BLACK }
#endif
    };
    memcpy(leds, menu_leds+menu_sel-1, NUM_LEDS*sizeof(CRGB));
  } else
#endif  

#ifdef ENABLE_PACMAN
#ifndef SINGLE_MACHINE
  if(machine == MCH_PACMAN) 
#endif  
  {
    // pacman: yellow on blue "knight rider" ...
    static char sub_cnt = 0;
    if(sub_cnt++ == 4) {
      sub_cnt = 0;
      
      // and also do the marquee LEDs
      static char led = 0;
      
      char il = (led<NUM_LEDS)?led:((2*NUM_LEDS-2)-led);
      for(char c=0;c<NUM_LEDS;c++) {
        if(c == il) leds[c] = LED_YELLOW;
        else        leds[c] = LED_BLUE;
      }
      led = (led + 1) % (2*NUM_LEDS-2);      
    }    
  }
#ifndef SINGLE_MACHINE
  else
#endif
#endif

#ifdef ENABLE_GALAGA  
#ifndef SINGLE_MACHINE
  if(machine == MCH_GALAGA) 
#endif  
  {
    // led_state:
    // 0:      idle
    // 1:      normal fighter visible
    // 2:      fighter captured
    // 3 - 12: enemy exploding
    
    static const CRGB galaga_leds[][NUM_LEDS] = {
      {  LED_BLACK,  LED_BLACK,  LED_BLACK,  LED_BLACK,  LED_BLACK,  LED_BLACK,  LED_BLACK },  // 0
      {    LED_RED,   LED_BLUE,  LED_WHITE,  LED_WHITE,  LED_WHITE,   LED_BLUE,    LED_RED },  // 1
      {  LED_WHITE,   LED_BLUE,    LED_RED,    LED_RED,    LED_RED,   LED_BLUE,  LED_WHITE },  // 2
      {  LED_BLACK,  LED_BLACK,  LED_BLACK, LED_YELLOW,  LED_BLACK,  LED_BLACK,  LED_BLACK },  // 3   
      {  LED_BLACK,  LED_BLACK, LED_YELLOW,   LED_BLUE, LED_YELLOW,  LED_BLACK,  LED_BLACK },  // 4
      {  LED_BLACK, LED_YELLOW,   LED_BLUE,   LED_CYAN,   LED_BLUE, LED_YELLOW,  LED_BLACK },  // 5
      { LED_YELLOW,   LED_BLUE,   LED_CYAN,    LED_RED,   LED_CYAN,   LED_BLUE, LED_YELLOW },  // 6
      {   LED_BLUE,   LED_CYAN,    LED_RED,  LED_WHITE,    LED_RED,   LED_CYAN,   LED_BLUE },  // 7
      {   LED_CYAN,    LED_RED,  LED_WHITE, LED_YELLOW,  LED_WHITE,    LED_RED,   LED_CYAN },  // 8
      {    LED_RED,  LED_WHITE, LED_YELLOW,  LED_WHITE, LED_YELLOW,  LED_WHITE,    LED_RED },  // 9
      {  LED_WHITE, LED_YELLOW,  LED_WHITE,  LED_BLACK,  LED_WHITE, LED_YELLOW,  LED_WHITE },  // 10
      { LED_YELLOW,  LED_WHITE,  LED_BLACK,  LED_BLACK,  LED_BLACK, LED_WHITE,  LED_YELLOW },  // 11
      {  LED_WHITE,  LED_BLACK,  LED_BLACK,  LED_BLACK, LED_BLACK,  LED_BLACK,   LED_WHITE }   // 12
    };
    memcpy(leds, galaga_leds+led_state, NUM_LEDS*sizeof(CRGB));
  } 
#ifdef ENABLE_DKONG
  else
#endif
#endif  

#ifdef ENABLE_DKONG
  {    
    // dkong: red "knight rider" ...
    static char sub_cnt = 0;
    if(sub_cnt++ == 4) {
      sub_cnt = 0;
      
      // and also do the marquee LEDs
      static char led = 0;
      
      char il = (led<NUM_LEDS)?led:((2*NUM_LEDS-2)-led);
      for(char c=0;c<NUM_LEDS;c++) {
	      if(c == il) leds[c] = LED_RED;
      	else        leds[c] = LED_BLACK;
      }
      led = (led + 1) % (2*NUM_LEDS-2);      
    }
  }
#endif
  
  FastLED.show();
}
   
#endif