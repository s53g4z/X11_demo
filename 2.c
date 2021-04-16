#include "2.h"

typedef struct egl_data {
	EGLDisplay egldisplay;
	EGLContext eglcontext;
	EGLSurface eglsurface;
} egl_data;
egl_data egl;

static inline void explode(void) {
	fprintf(stderr, "ASSERT: terminating\n");

}

static void assert(unsigned long long condition) {
	if (condition) return;
	explode();
	exit(33);
}

static void check_data_types(void) {
	assert(sizeof(EGLint) == 4);
	assert(sizeof(EGLAttrib) == 8);
	assert(sizeof(EGLTime) == 8);
}

static void attempt_egl_init(void) {
	// select the OpenGL ES API
	assert(EGL_FALSE != eglBindAPI(EGL_OPENGL_ES_API));
	
	// get an EGLDisplay
	egl.egldisplay = eglGetDisplay((void *)wl.wldisplay);
	assert(EGL_NO_DISPLAY != egl.egldisplay);
	
	// initialize EGL
	EGLint major = -1, minor = -1;
	assert(EGL_TRUE == eglInitialize(egl.egldisplay, &major, &minor));
	assert(major >= 1 && minor >= 4);
	fprintf(stderr, "EGL version %d.%d\n", major, minor);
	
	// choose an EGLConfig
	EGLConfig config;
	EGLint num_config = 1;
	EGLint const attrib_list[] = {
		EGL_BLUE_SIZE,				8,
		EGL_GREEN_SIZE,				8,
		EGL_RED_SIZE, 				8,
		EGL_NONE,					EGL_NONE,
	};
	assert(EGL_TRUE ==
		eglChooseConfig(egl.egldisplay, attrib_list, &config, 1, &num_config) &&
		num_config > 0);
	
	// create an EGL rendering context
	egl.eglcontext = eglCreateContext(egl.egldisplay,
		config, EGL_NO_CONTEXT, NULL);
	assert(EGL_NO_CONTEXT != egl.eglcontext);
	
	// create an EGL window surface
	egl.eglsurface = eglCreatePlatformWindowSurface(egl.egldisplay,
		config, wl.wleglwindow, NULL);
	assert(EGL_NO_SURFACE != egl.eglsurface);
	
	return;
}

static void attempt_egl_end() {
	assert(EGL_TRUE == eglDestroySurface(egl.egldisplay, egl.eglsurface));
	assert(EGL_TRUE == eglDestroyContext(egl.egldisplay, egl.eglcontext));
	assert(EGL_TRUE == eglTerminate(egl.egldisplay));
}

// Do nothing with v.
static inline void unused_fn(void *v, ...) {
	v = v;
}

static void wlregistrylistenerglobal(void *data, wl_registry *wl_registry,
	uint32_t name, const char *interface, uint32_t version) {
	unused_fn(data, wl_registry);
	
//	fprintf(stderr, "INFO: Global object named \"%u\" implements version \"%u\""
//		" of interface \"%s\"\n", name, version, interface);
	
	if (0 == strcmp(interface, "wl_compositor")) {
		// found the goddamn compositor
		wl.wlcompositor = wl_registry_bind(wl_registry, name,
			&wl_compositor_interface, version);
	} else if (0 == strcmp(interface, "wl_subcompositor")) {
		wl.wlsubcompositor = wl_registry_bind(wl_registry, name,
			&wl_subcompositor_interface, version);
	} else if (0 == strcmp(interface, "wl_shm")) {
		wl.wlshm = wl_registry_bind(wl_registry, name,
			&wl_shm_interface, version);
	} else if (0 == strcmp(interface, "wl_seat")) {
		wl.wlseat = wl_registry_bind(wl_registry, name,
			&wl_seat_interface, version);
	} else if (0 == strcmp(interface, "wl_output")) {
		wl.wloutput = wl_registry_bind(wl_registry, name,
			&wl_output_interface, version);
	}
}

static void wlregistrylistenerglobalremove(void *data, wl_registry *wl_registry,
	uint32_t name) {
	unused_fn(data, wl_registry);
	fprintf(stderr, "Global object %u has been removed\n", name);
}

static void wlbufferlistenerrelease(void *data, struct wl_buffer *wl_buffer) {
	unused_fn(data, wl_buffer);
	fprintf(stderr, "%s\n", "Wayland, the compositor has released the buffer\n");
}

static void attempt_wayland_init(void) {
	// get wl_display
	wl.wldisplay = wl_display_connect(NULL);
	assert(NULL != wl.wldisplay);
	fprintf(stderr, "wl_display, version %u\n",
		wl_display_get_version(wl.wldisplay));
	
	// get wl_display's file descriptor
	wl.wlfd = wl_display_get_fd(wl.wldisplay);
	assert(wl.wlfd > 0);
	
	// get the wl_registry
	wl.wlregistry = wl_display_get_registry(wl.wldisplay);
	fprintf(stderr, "wl_registry, version %u\n",
		wl_registry_get_version(wl.wlregistry));
	const struct wl_registry_listener wlregistrylistener= {
		.global = &wlregistrylistenerglobal,
		.global_remove = &wlregistrylistenerglobalremove,
	};
	wl_registry_add_listener(wl.wlregistry, &wlregistrylistener, NULL);
	
	// sync!!
	assert(-1 != wl_display_roundtrip(wl.wldisplay));
	
	// get a surface
	assert(NULL != wl.wlcompositor);
	wl.wlsurface = wl_compositor_create_surface(wl.wlcompositor);
	fprintf(stderr, "wl_surface, version %u\n",
		wl_surface_get_version(wl.wlsurface));
		
	// get a wl_egl_window
	wl.wleglwindow = wl_egl_window_create(wl.wlsurface, 640, 480);
	
	return;
}

static void attempt_wayland_end() {
	wl_egl_window_destroy(wl.wleglwindow);
	wl_surface_destroy(wl.wlsurface);
	//wl_buffer_destroy(wl.wlbuffer);
	//wl_event_queue_destroy(wl.wleventqueue);
	wl_compositor_destroy(wl.wlcompositor);
	wl_registry_destroy(wl.wlregistry);
	wl_display_disconnect(wl.wldisplay);
}

int main(int argc, char **argv) {
	argc += 0;
	argv[0][0] += 0;
	check_data_types();
	
	attempt_wayland_init();
	attempt_egl_init();
	
	attempt_egl_end();
	attempt_wayland_end();
}
