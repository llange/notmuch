/* notmuch - Not much of an email program, (just index and search)
 *
 * Copyright © 2009 Carl Worth
 * Copyright © 2009 Keith Packard
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
 * along with this program.  If not, see http://www.gnu.org/licenses/ .
 *
 * Author: Keith Packard <keithp@keithp.com>
 */

#include "notmuch-client.h"

enum {
    OUTPUT_THREADS,
    OUTPUT_MESSAGES,
};

int
notmuch_count_command (void *ctx, int argc, char *argv[])
{
    notmuch_config_t *config;
    notmuch_database_t *notmuch;
    notmuch_query_t *query;
    char *query_str;
    int opt_index;
    int output = OUTPUT_MESSAGES;

    notmuch_opt_desc_t options[] = {
	{ NOTMUCH_OPT_KEYWORD, &output, "output", 'o',
	  (notmuch_keyword_t []){ { "threads", OUTPUT_THREADS },
				  { "messages", OUTPUT_MESSAGES },
				  { 0, 0 } } },
	{ 0, 0, 0, 0, 0 }
    };

    opt_index = parse_arguments (argc, argv, options, 1);

    if (opt_index < 0) {
	return 1;
    }

    config = notmuch_config_open (ctx, NULL, NULL);
    if (config == NULL)
	return 1;

    notmuch = notmuch_database_open (notmuch_config_get_database_path (config),
				     NOTMUCH_DATABASE_MODE_READ_ONLY);
    if (notmuch == NULL)
	return 1;

    query_str = query_string_from_args (ctx, argc-opt_index, argv+opt_index);
    if (query_str == NULL) {
	fprintf (stderr, "Out of memory.\n");
	return 1;
    }

    if (*query_str == '\0') {
	query_str = talloc_strdup (ctx, "");
    }

    query = notmuch_query_create (notmuch, query_str);
    if (query == NULL) {
	fprintf (stderr, "Out of memory\n");
	return 1;
    }

    switch (output) {
    case OUTPUT_MESSAGES:
	printf ("%u\n", notmuch_query_count_messages (query));
	break;
    case OUTPUT_THREADS:
	printf ("%u\n", notmuch_query_count_threads (query));
	break;
    }

    notmuch_query_destroy (query);
    notmuch_database_close (notmuch);

    return 0;
}
