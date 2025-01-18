#include "TVout.h"

TVout TV;

// Initial position of the block
int x = 10;  
int y = 10;  
// Movement direction
int dx = 1;  
int dy = 1;  

void setup() {
  TV.begin(NTSC, 120, 96);  // Initialize TVout with NTSC and 120x96 resolution
  TV.clear_screen();

  // Draw the initial 10x10 block
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      TV.set_pixel(x + i, y + j, WHITE);
    }
  }

  delay(2000);  // Wait for 2 seconds
}

void loop() {
  TV.clear_screen();  // Clear the screen each frame

  // Update the block's position
  x += dx;
  y += dy;

  // Bounce off edges
  if (x <= 0 || x >= TV.hres() - 10) dx = -dx;  // Ensure the whole block stays on screen
  if (y <= 0 || y >= TV.vres() - 10) dy = -dy;

  // Draw the updated 10x10 block
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      TV.set_pixel(x + i, y + j, WHITE);
    }
  }

  delay(50);  // Small delay for smoother animation
}
