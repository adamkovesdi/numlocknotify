/*
 * numlocknotify.c
 * (c) 2021 adamkov
 * based on the code of indicator-keylock
 *
 * one liner build script
 * gcc numlocknotify.c -o numlocknotify -lX11 `pkg-config --cflags --libs glib-2.0` `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs libnotify`
 *
 */

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/XKBlib.h>
#include <libnotify/notify.h>

#define NOTIFICATION_TIMEOUT_MS 1500

GtkApplication *		self_gtk_app;
NotifyNotification *	noti;

static gboolean old_on[3] = {FALSE, FALSE, FALSE};

static Display *rootwin = NULL;
static gint xkbev = 0;
static gint xkberr = 0;

gboolean *get_led_states() {
	guint state = 0;
	static gboolean on[3] = {FALSE, FALSE, FALSE};
	int i;
	XkbGetIndicatorState(rootwin, XkbUseCoreKbd, &state);

	for (i = 0 ; i < 3 ; i++) {
		if (state & (1 << i))
			on[i] = TRUE;
		else
			on[i] = FALSE;
	}
	return(on);
}

/* Mostly borrowed from lock-keys-applet */
GdkFilterReturn event_filter(GdkXEvent *gdkxevent, GdkEvent *event, GCallback show_func) {
	XkbEvent ev;
	memcpy(&ev.core, gdkxevent, sizeof(ev.core));

	if (ev.core.type == xkbev + XkbEventCode) {
		if (ev.any.xkb_type == XkbIndicatorStateNotify)
			show_func();
	}

	return GDK_FILTER_CONTINUE;
}

gboolean init_xkb_extension(GCallback show_func) {
	int code;
	int maj = XkbMajorVersion;
	int min = XkbMinorVersion;
	rootwin = gdk_x11_get_default_xdisplay();

	if (!XkbLibraryVersion(&maj, &min))
		return FALSE;

	if (!XkbQueryExtension(rootwin, &code, &xkbev, &xkberr, &maj, &min))
		return FALSE;

	if (!XkbSelectEvents(rootwin, XkbUseCoreKbd, XkbIndicatorStateNotifyMask, XkbIndicatorStateNotifyMask))
		return FALSE;

	gdk_window_add_filter(NULL, (GdkFilterFunc)event_filter, show_func);

	return TRUE;
}

void on_xkb_event() {
	gboolean *on = get_led_states();
	if(!memcmp(on, old_on, sizeof(old_on))) {
		return;
	}
	if(on[1] && !old_on[1]) {
		notify_notification_update(noti, "NumLock", "ON", 0);
	}
	else if(!on[1] && old_on[1]) {
		notify_notification_update(noti, "NumLock", "off", 0);
	}
	else if(on[0] && !old_on[0]) {
		notify_notification_update(noti, "CapsLock", "ON", 0);
	}
	else if(!on[0] && old_on[0]) {
		notify_notification_update(noti, "CapsLock", "off", 0);
	}
	notify_notification_show(noti,0);
	memcpy(old_on,  get_led_states(), sizeof(old_on));
}

int main(int argc, char * argv[]) {
	gtk_init_with_args(&argc, &argv, _("numlocknotify"), NULL, NULL, NULL);
	self_gtk_app = gtk_application_new("org.desktop.numlocknotify", 0);
	notify_init("numlocknotify");
	noti = notify_notification_new("NumLock", "State", 0);
	notify_notification_set_timeout(noti, NOTIFICATION_TIMEOUT_MS);

	if (!init_xkb_extension(G_CALLBACK(on_xkb_event))) {
		GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("Could not initialize X Keyboard Extension."));
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		return(0);
	}
	gtk_main();
}
