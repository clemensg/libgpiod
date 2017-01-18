/*
 * Daemon controlling GPIOs via the character device.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include <gpiod.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <sys/signalfd.h>
#include <sys/epoll.h>

/*****************************************************************************
 * Helper macros & functions
 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

#define UNUSED			__attribute__((unused))
#define PRINTF(fmt, arg)	__attribute__((format(printf, fmt, arg)))
#define NORETURN		__attribute__((noreturn))
#define ARRAY_SIZE(x)		(sizeof(x) / sizeof(*(x)))

struct list_head {
	struct list_head *prev;
	struct list_head *next;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

static void list_add(struct list_head *new,
		     struct list_head *prev,
		     struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static void list_add_tail(struct list_head *new, struct list_head *head)
{
	list_add(new, head->prev, head);
}

static void list_del(struct list_head *entry)
{
	struct list_head *prev = entry->prev;
	struct list_head *next = entry->next;

	next->prev = prev;
	prev->next = next;
}

#define list_entry(ptr, type, member)					\
	({								\
		const typeof(((type *)0)->member) *__mptr = (ptr);	\
		(type *)((char *)__mptr - offsetof(type, member));	\
	})

#define list_for_each(iter, head)					\
	for (iter = (head)->next; iter != (head); iter = iter->next)

#define list_for_each_safe(iter, tmp, head)				\
	for (iter = (head)->next, tmp = iter->next;			\
	     iter != (head); iter = tmp, tmp = iter->next)

static void * zalloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	if (ptr)
		memset(ptr, 0, size);

	return ptr;
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * Helper macros & functions
 *****************************************************************************/

/*****************************************************************************
 * Globals
 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

struct {
	const char *progname;
	bool should_stop;
	int epollfd;
	struct list_head listeners;
} globals = {
	.listeners = LIST_HEAD_INIT(globals.listeners),
};

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * Globals
 *****************************************************************************/

/*****************************************************************************
 * Logging
 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

enum {
	MSG_ERROR = 0,
	MSG_WARN,
	MSG_INFO,
	MSG_DEBUG,
};

static void vmsg(int level, const char *fmt, va_list va)
{
	const char *lvlstr;

	switch (level) {
	case MSG_ERROR:
		lvlstr = "ERROR";
		break;
	case MSG_WARN:
		lvlstr = " WARN";
		break;
	case MSG_INFO:
		lvlstr = " INFO";
		break;
	case MSG_DEBUG:
		lvlstr = "DEBUG";
		break;
	default:
		/* This should not happen. */
		lvlstr = "UNKNOWN";
		break;
	}

	fprintf(stderr, "gpioctld [%s]: ", lvlstr);
	vfprintf(stderr, fmt, va);
	fputc('\n', stderr);
}

static PRINTF(1,2) void err(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	vmsg(MSG_ERROR, fmt, va);
	va_end(va);
}

static PRINTF(1,2) void warn(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	vmsg(MSG_WARN, fmt, va);
	va_end(va);
}

static PRINTF(1,2) void info(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	vmsg(MSG_INFO, fmt, va);
	va_end(va);
}

static NORETURN PRINTF(1, 2) void die(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	fprintf(stderr, "%s: ", globals.progname);
	vfprintf(stderr, fmt, va);
	fputc('\n', stderr);
	va_end(va);

	exit(EXIT_FAILURE);
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * Logging
 *****************************************************************************/

/*****************************************************************************
 * Listeners
 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

struct listener {
	struct list_head listener_list;
	int fd;
	void (*callback)(int fd, void *data);
	void *data;
	void (*free_data)(void *data);
};

static struct listener * listener_new(void)
{
	return zalloc(sizeof(struct listener));
}

static void listener_free(struct listener *lst)
{
	free(lst);
}

static int listener_add(struct listener *lst)
{
	struct epoll_event event;
	int status;

	event.events = EPOLLIN | EPOLLPRI;
	event.data.ptr = lst;

	status = epoll_ctl(globals.epollfd, EPOLL_CTL_ADD, lst->fd, &event);
	if (status < 0)
		return status;

	list_add_tail(&lst->listener_list, &globals.listeners);

	return 0;
}

static void destroy_listeners(void)
{
	struct list_head *iter, *tmp;
	struct listener *lst;

	list_for_each_safe(iter, tmp, &globals.listeners) {
		lst = list_entry(iter, struct listener, listener_list);

		if (lst->free_data)
			lst->free_data(lst->data);

		close(lst->fd);
		list_del(&lst->listener_list);
		listener_free(lst);
	}
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * Listeners
 *****************************************************************************/

/*****************************************************************************
 * Signal handling
 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

static void handle_signal(int fd, void *data UNUSED)
{
	struct signalfd_siginfo sinfo;
	ssize_t rd;

	rd = read(fd, &sinfo, sizeof(sinfo));
	if (rd < 0) {
		err("error reading signal info: %s", strerror(errno));
		return;
	} else if (rd != sizeof(sinfo)) {
		err("invalid size of signal info");
		return;
	}

	switch (sinfo.ssi_signo) {
	case SIGTERM:
	case SIGINT:
		info("signal received - terminating");
		globals.should_stop = true;
		break;
	default:
		warn("unexpected signal caught: %d", sinfo.ssi_signo);
		break;
	}
}

static void setup_signalfd(void)
{
	struct listener *lst;
	int sigfd, status;
	sigset_t sigmask;

	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGTERM);
	sigaddset(&sigmask, SIGINT);

	status = sigprocmask(SIG_BLOCK, &sigmask, NULL);
	if (status < 0)
		die("unable to init signal mask: %s", strerror(errno));

	sigfd = signalfd(-1, &sigmask, 0);
	if (sigfd < 0)
		die("unable to setup signalfd: %s", strerror(errno));

	lst = listener_new();
	if (!lst)
		die("out of memory");

	lst->fd = sigfd;
	lst->callback = handle_signal;

	status = listener_add(lst);
	if (status < 0)
		die("unable to add signalfd listener: %s", strerror(errno));
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * Signal handling
 *****************************************************************************/

/*****************************************************************************
 * Main loop
 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

static void setup_epollfd(void)
{
	globals.epollfd = epoll_create1(0);
	if (globals.epollfd < 0)
		die("error creating epoll socket: %s", strerror(errno));
}

static void teardown_epollfd(void)
{
	close(globals.epollfd);
}

static void main_loop(void)
{
	struct epoll_event eventbuf[64];
	struct listener *lst;
	int status, i;

	do {
		memset(eventbuf, 0, sizeof(eventbuf));

		status = epoll_wait(globals.epollfd, eventbuf,
				    ARRAY_SIZE(eventbuf), -1);
		if (status < 0) {
			err("epoll_wait() failure: %s", strerror(errno));
			globals.should_stop = true;
		}

		for (i = 0; i < status; i++){
			lst = eventbuf[i].data.ptr;
			if (lst->callback)
				lst->callback(lst->fd, lst->data);

			/*
			 * Check if we should already stop before
			 * processing each event.
			 */
			if (globals.should_stop)
				break;
		}
	} while (!globals.should_stop);
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * Main loop
 *****************************************************************************/

/*****************************************************************************
 * Command-line parsing
 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

static void print_help(void)
{
	printf("HELP TODO\n");
}

static const struct option longopts[] = {
	{ "help",	no_argument,	NULL,	'h' },
	{ "version",	no_argument,	NULL,	'v' },
	{ 0 },
};

static const char *const shortopts = "vh";

static void parse_args(int argc, char **argv)
{
	int optc, opti;


	for (;;) {
		optc = getopt_long(argc, argv, shortopts, longopts, &opti);
		if (optc < 0)
			break;

		switch (optc) {
		case 'h':
			print_help();
			exit(EXIT_SUCCESS);
		case 'v':
			printf("gpioctld (libgpiod) %s\n",
			       gpiod_version_string());
			exit(EXIT_SUCCESS);
		case '?':
			die("try %s --help", argv[0]);
		default:
			abort();
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 0)
		die("unrecognized argument: %s", argv[0]);
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * Command-line parsing
 *****************************************************************************/

int main(int argc, char **argv)
{
	globals.progname = argv[0];

	parse_args(argc, argv);
	setup_epollfd();
	setup_signalfd();

	main_loop();

	destroy_listeners();
	teardown_epollfd();

	return EXIT_SUCCESS;
}
