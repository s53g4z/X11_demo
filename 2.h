#ifndef TWO_H
#define TWO_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <EGL/egl.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-egl-core.h>

typedef struct wl_display wl_display;
typedef struct wl_event_queue wl_event_queue;
typedef struct wl_registry wl_registry;
typedef struct wl_surface wl_surface;
typedef struct wl_shm_pool wl_shm_pool;
typedef struct wl_buffer wl_buffer;
typedef struct wl_compositor wl_compositor;
typedef struct wl_subcompositor wl_subcompositor;
typedef struct wl_shm wl_shm;
typedef struct wl_drm wl_drm;
typedef struct wl_seat wl_seat;
typedef struct wl_output wl_output;
typedef struct wl_egl_window wl_egl_window;
typedef struct wl_shell wl_shell;
typedef struct wl_data {
	wl_display *wldisplay;
	wl_event_queue *wleventqueue;
	wl_registry *wlregistry;
	wl_surface *wlsurface;
	wl_shm_pool *wlshmpool;
	wl_buffer *wlbuffer;
	wl_compositor *wlcompositor;
	wl_subcompositor *wlsubcompositor;
	wl_shm *wlshm;
	wl_drm *wldrm;
	wl_seat *wlseat;
	wl_output *wloutput;
	wl_egl_window *wleglwindow;
	wl_shell *wlshell;
	int wlfd;
} wl_data;
static wl_data wl;

extern void exit(int status);
extern int strcmp(const char *, const char *);

#endif
