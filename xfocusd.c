#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <xcb/xcb.h>

#include <wm.h>

xcb_connection_t *conn;
xcb_screen_t *scrn;

static void (*handle_event[XCB_NO_OPERATION])(xcb_generic_event_t *ev);

void cleanup(void);
void create_win(xcb_generic_event_t*);
void enter_win(xcb_generic_event_t*);
void focus(xcb_window_t);
void handle_events(void);
void handle_signals(int);
void map_win(xcb_generic_event_t*);
void unmap_win(xcb_generic_event_t*);

int main(int argc, char **argv) {
  int i, winc;
  xcb_window_t *winv;
  
  wm_init_xcb();
  atexit(cleanup);
  if (signal(SIGINT, handle_signals) == SIG_ERR) {
    wm_kill_xcb();
    errx(EXIT_FAILURE, "Cannot register signal handler.");
  }
  
  wm_get_screen();
  
  // Notify when windows are created or destroyed
  wm_reg_event(scrn->root, XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY);
  
  // Trigger an event when any window is entered
  if ((winc = wm_get_windows(scrn->root, &winv))) {
    for (i = 0; i < winc; i++) {
      if (wm_is_ignored(winv[i])) continue;
      
      wm_reg_event(winv[i], XCB_EVENT_MASK_ENTER_WINDOW);
    }
    
    free(winv);
  }
  
  for (i = 0; i < XCB_NO_OPERATION; i++) {
    handle_event[i] = NULL;
  }
  // Install event handlers
  handle_event[XCB_CREATE_NOTIFY] = create_win;
  handle_event[XCB_ENTER_NOTIFY] = enter_win;
  handle_event[XCB_MAP_NOTIFY] = map_win;
  handle_event[XCB_UNMAP_NOTIFY] = unmap_win;
  
  handle_events();
  
  return EXIT_SUCCESS;
}

void cleanup(void) {
  wm_kill_xcb();
}

void create_win(xcb_generic_event_t *ev) {
  xcb_create_notify_event_t *e = (xcb_create_notify_event_t *)ev;
  int h, w, x, y;
  
  if (wm_is_ignored(e->window)) return;
  
  w = wm_get_attribute(e->window, ATTR_W);
  h = wm_get_attribute(e->window, ATTR_H);
  
  // Move new win to mouse position
  if ((wm_get_cursor(0, scrn->root, &x, &y))) {
    wm_move(e->window, ABSOLUTE, x-(w/2), y-(h/2));
  }
  
  wm_reg_event(e->window, XCB_EVENT_MASK_ENTER_WINDOW);
}

void enter_win(xcb_generic_event_t *ev) {
  xcb_enter_notify_event_t *e = (xcb_enter_notify_event_t *)ev;
  
  if (e->event == wm_get_focus() || wm_is_ignored(e->event)) return;
  
  focus(e->event);
}

void focus(xcb_window_t w) {
  wm_set_focus(w);  
  printf("0x%08x\n", w);
}

void handle_events(void) {
  xcb_generic_event_t *ev;
  
  while (1) {
    xcb_flush(conn);
    
    if (xcb_connection_has_error(conn)) {
      errx(EXIT_FAILURE, "An error occurred with the X server connection");
    }
    
    if ((ev = xcb_wait_for_event(conn)) && handle_event[ev->response_type & ~0x80]) {
      handle_event[ev->response_type & ~0x80](ev);      
      free(ev);
    }
  }
}

void handle_signals(int signo)
{
  if (signo == SIGINT)
    exit(EXIT_SUCCESS);
}   

void map_win(xcb_generic_event_t *ev) {
  xcb_map_notify_event_t *e = (xcb_map_notify_event_t *)ev;
  
  focus(e->window);
}

void unmap_win(xcb_generic_event_t *ev) {
  //xcb_unmap_notify_event_t *e = (xcb_unmap_notify_event_t *)ev;
  int i, winc;
  xcb_window_t *winv;
  
  // Set focus to highest window on the stack
  if ((winc = wm_get_windows(scrn->root, &winv))) {
    for (i = winc - 1; ; i--) {
      if (wm_is_ignored(winv[i])) continue;
      
      wm_set_focus(winv[winc - 1]);
      break;
    }
  
    free(winv);
  }
}