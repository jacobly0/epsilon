extern "C" {
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
}
#include <chrono>
#include "init.h"
#include <kandinsky.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

#include "display/fltklcd.h"
#include "keyboard/fltkkbd.h"
#undef None // TODO: Remove me

static FltkLCD * sDisplay;
static FltkKbd * sKeyboard;
static KDFrameBuffer * sFrameBuffer;

#define FRAMEBUFFER_ADDRESS (sDisplay->m_framebuffer)

Ion::Events::Event currentEvent = Ion::Events::None;

void terminateHandler(Fl_Widget *) {
  currentEvent = Ion::Events::Termination;
}

void init_platform() {
  Fl::visual(FL_RGB);

  int margin = 10;
  int screen_width = Ion::Display::Width;
  int screen_height = Ion::Display::Height;
  int keyboard_height = screen_width;

  Fl_Window * window = new Fl_Window(screen_width+2*margin, margin+screen_height+margin+keyboard_height+margin);
  window->callback(terminateHandler);

  KDColor * pixels = (KDColor *)malloc(Ion::Display::Width*Ion::Display::Height*sizeof(KDColor));
  sFrameBuffer = new KDFrameBuffer(pixels, KDSize(Ion::Display::Width, Ion::Display::Height));
  /*
  sFrameBuffer.drawingArea.origin = KDPointZero;
  sFrameBuffer.drawingArea.size = sFrameBuffer.size;
  sFrameBuffer.drawingCursor = KDPointZero;
  */

  sDisplay = new FltkLCD(margin, margin, screen_width, screen_height, pixels);
  sKeyboard = new FltkKbd(margin, margin+screen_height+margin, screen_width, keyboard_height);

  window->end();
  window->show();

  //KDCurrentContext->fillRect = NULL;
}

void Ion::Display::pushRect(KDRect r, const KDColor * pixels) {
  sFrameBuffer->pushRect(r, pixels);
}

void Ion::Display::pushRectUniform(KDRect r, KDColor c) {
  sFrameBuffer->pushRectUniform(r, c);
}

void Ion::Display::pullRect(KDRect r, KDColor * pixels) {
  sFrameBuffer->pullRect(r, pixels);
}

void Ion::Display::waitForVBlank() {
}

Ion::Keyboard::State Ion::Keyboard::scan() {
  Ion::Keyboard::State result = 0;
  for (int i=0; i<Ion::Keyboard::NumberOfKeys; i++) {
    result = result << 1 | sKeyboard->key_down((Ion::Keyboard::Key)(Ion::Keyboard::NumberOfKeys-1-i));

  }
  return result;
}

#include <chrono>
template<typename Period, typename Rep, typename Clock, typename Duration>
static bool wait(Rep & remaining, const std::chrono::time_point<Clock, Duration> & target) {
  sDisplay->redraw();
  auto now = std::chrono::high_resolution_clock::now();
  if (now >= target) {
    Fl::check();
  } else {
    Fl::wait(std::chrono::duration_cast<std::chrono::duration<double>>(target - now).count());
  }
  now = std::chrono::high_resolution_clock::now();
  if (now >= target) {
    remaining = 0;
  } else {
    remaining = std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(target - now).count();
  }
  return remaining;
}
template<typename Period, typename Rep>
static void sleep(Rep timeout) {
  auto target = std::chrono::high_resolution_clock::now() + std::chrono::duration<Rep, Period>(timeout);
  while (wait<Period>(timeout, target));
}

Ion::Events::Event Ion::Events::getEvent(int * timeout) {
  auto target = std::chrono::high_resolution_clock::now() + std::chrono::duration<int, std::milli>(*timeout);
  while (wait<std::milli>(*timeout, target) && currentEvent == None);
  Event event = currentEvent;
  currentEvent = None;
  updateModifiersFromEvent(event);
  return event;
}

void Ion::msleep(long ms) {
  sleep<std::milli>(ms);
}

void Ion::usleep(long us) {
  sleep<std::micro>(us);
}

const char * Ion::serialNumber() {
  return "Simulator";
}
