/*  Cliphist Panel Plugin - Clipboard entry data structure
 *  Copyright (c) 2026 - GPL-2.0-or-later
 */

#include "cliphist-entry.h"
#include <string.h>

#define MAX_DISPLAY_CHARS 40
#define MAX_DISPLAY_LINES 5

static gchar *
truncate_text(const gchar *text)
{
    if (!text || strlen(text) == 0)
        return g_strdup("");

    const gchar *p = text;
    gint line_count = 0;
    gint char_count = 0;
    const gchar *truncate_point = NULL;

    while (*p && line_count < MAX_DISPLAY_LINES && char_count < MAX_DISPLAY_CHARS) {
        if (*p == '\n') {
            line_count++;
            if (line_count >= MAX_DISPLAY_LINES) {
                truncate_point = p;
                break;
            }
        }
        char_count++;
        p++;
    }

    if (*p == '\0') {
        return g_strdup(text);
    } else if (truncate_point != NULL) {
        gchar *result = g_strndup(text, truncate_point - text);
        gchar *with_ellipsis = g_strdup_printf("%s...", result);
        g_free(result);
        return with_ellipsis;
    } else {
        gchar *result = g_strndup(text, p - text);
        gchar *with_ellipsis = g_strdup_printf("%s...", result);
        g_free(result);
        return with_ellipsis;
    }
}

CliphistEntry *
cliphist_entry_new(guint index, const gchar *text, const gchar *mime_type)
{
    CliphistEntry *entry = g_slice_new0(CliphistEntry);
    entry->index = index;
    entry->mime_type = g_strdup(mime_type ? mime_type : "text/plain");
    entry->is_image = (mime_type && g_str_has_prefix(mime_type, "image/"));

    if (entry->is_image) {
        entry->display_text = g_strdup("[IMAGE]");
    } else {
        entry->display_text = truncate_text(text);
    }

    return entry;
}

void
cliphist_entry_free(CliphistEntry *entry)
{
    if (entry) {
        g_free(entry->display_text);
        g_free(entry->mime_type);
        g_slice_free(CliphistEntry, entry);
    }
}
