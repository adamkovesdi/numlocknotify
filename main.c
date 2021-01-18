#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libnotify/notify.h>
#include <X11/Xlib.h>

#define POLL_INTERVAL_MS 250
#define NOTIFICATION_TIMEOUT_MS 800

int prevstate;

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
	struct timespec ts;
	int res;

	if (msec < 0) {
		errno = EINVAL;
		return -1;
	}

	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;

	do {
		res = nanosleep(&ts, &ts);
	} while (res && errno == EINTR);

	return res;
}

static void notify_numlockstate(const char* ns) {
	notify_init("numlocknotify");
	NotifyNotification* n = notify_notification_new ("NumLock", ns, 0);
	notify_notification_set_timeout(n, NOTIFICATION_TIMEOUT_MS);
	notify_notification_show(n,0);
}

static int get_numlock_state(void) {
	Display *dpy = XOpenDisplay(":0"); 
	XKeyboardState x;
	XGetKeyboardControl(dpy, &x);
	XCloseDisplay(dpy);
	return (x.led_mask & 2) ? 1 : 0;
}

static void loop() {
	int ns = get_numlock_state();
	if(prevstate == ns) return;
	prevstate = ns;

	if(ns) {
		notify_numlockstate("on");
	}
	else {
		notify_numlockstate("off");
	}
}

static void spawn_daemon(void) {
	pid_t pid;
	/* Fork off the parent process */
	pid = fork();
	/* An error occurred */
	if (pid < 0)
		exit(EXIT_FAILURE);
	/* Success: Let the parent terminate */
	if (pid > 0)
		exit(EXIT_SUCCESS);
	/* On success: The child process becomes session leader */
	if (setsid() < 0)
		exit(EXIT_FAILURE);
	/* Catch, ignore and handle signals */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	/* Fork off for the second time*/
	pid = fork();
	/* An error occurred */
	if (pid < 0)
		exit(EXIT_FAILURE);
	/* Success: Let the parent terminate */
	if (pid > 0)
		exit(EXIT_SUCCESS);
	/* Set new file permissions */
	umask(0);
	/* Change the working directory to the root directory */
	/* or another appropriated directory */
	chdir("/");
	/* Close all open file descriptors */
	int x;
	for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
	{
		close (x);
	}
}

int main(int argc, char * argv[]) {
	prevstate = get_numlock_state();
	spawn_daemon();
	while(1) {
		loop();
		msleep(POLL_INTERVAL_MS);
	}
	return 0;
}
