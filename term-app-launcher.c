/*-
 * Copyright (C) 2010 Peter Tworek<tworaz666@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <vte/vte.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define VERSION "0.1"


void
fork_done(VteTerminal *vte, gpointer udata)
{
	gtk_main_quit();
}


gboolean
window_deleted(GtkWidget *w, GdkEvent *ev, gpointer data)
{
	gtk_main_quit();
	return TRUE;
}


void 
print_help(const char* progname)
{
	printf("Usage: %s [options]\n", progname);
	printf("Options:\n");
	printf(" -e <command>  : execute specified command (required)\n");
	printf(" -f <fontname> : specify console font, for example \"Monospace 8\"\n");
	printf(" -T <title>    : set window title\n");
	printf(" -v            : print version string\n");
	printf(" -h            : print this help\n");
}


int 
main(int argc, char** argv)
{
	int 	opt, status;
	char* 	tmp = NULL;
	char* 	command = NULL;
	char* 	font = NULL;
	char*   title = NULL;
	char** 	cmd_args = NULL;
	int     cmd_args_cnt = 0;
	pid_t   cmd_pid = 0;

	if (argc < 2) {
		print_help(argv[0]);
		return 1;
	}

	while ((opt = getopt(argc, argv, "e:f:T:hv")) != -1) {
		switch (opt) {
		case 'f':
			font = optarg;
			break;
		case 'e':
			command = optarg;
			break;
		case 'T':
			title = optarg;
			break;
		case 'v':
			printf("%s version %s\n", argv[0], VERSION);
			return 0;
		default:
			print_help(argv[0]);
			return 1;
		}
	}

	if (NULL == command) {
		print_help(argv[0]);
		return 1;
	}

	tmp = command;
	do {
		cmd_args_cnt++;
	} while ((tmp = strchr(tmp+1, ' ')));
	cmd_args_cnt += 1;
	cmd_args = malloc(cmd_args_cnt * sizeof(char*));

	tmp = strtok(command, " ");
	cmd_args[0] = malloc(strlen(tmp)+1);
	strncpy(cmd_args[0], tmp, strlen(tmp));

	for (opt = 1; opt < cmd_args_cnt - 1; opt++) {
		tmp = strtok(NULL, " ");
		cmd_args[opt] = malloc(strlen(tmp)+1);
		strncpy(cmd_args[opt], tmp, strlen(tmp));
	}
	cmd_args[cmd_args_cnt - 1] = NULL;

	gtk_init(&argc, &argv);

	GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	GtkWidget *vte1 = vte_terminal_new ();

	g_signal_connect(GTK_OBJECT(vte1), "child-exited",
	                 G_CALLBACK(fork_done), NULL);
	g_signal_connect(G_OBJECT(window), "delete_event",
	                 G_CALLBACK(window_deleted), vte1);
               
	gtk_container_add (GTK_CONTAINER(window), vte1);
	gtk_widget_show_all (window);

	if (font) {
		vte_terminal_set_font_from_string(VTE_TERMINAL(vte1), font);
	}

	if (title) {
		gtk_window_set_title(GTK_WINDOW(window), title);
	}

	cmd_pid = vte_terminal_fork_command (VTE_TERMINAL(vte1), cmd_args[0], cmd_args, 
	                                     NULL, "~/", FALSE, FALSE, FALSE);
	gtk_main ();

	kill(cmd_pid, SIGTERM);
	waitpid(cmd_pid, &status, 0);

	for (opt = 0; opt < cmd_args_cnt; opt++) {
		free(cmd_args[opt]);
	}
	free(cmd_args);

	return 0;
} 
