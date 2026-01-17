/*  Cliphist Panel Plugin - Clipboard entry data structure
 *  Copyright (c) 2026 - GPL-2.0-or-later
 */

#ifndef __CLIPHIST_ENTRY_H__
#define __CLIPHIST_ENTRY_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct _CliphistEntry CliphistEntry;

struct _CliphistEntry
{
    guint index;
    gchar *display_text;
    gchar *mime_type;
    gboolean is_image;
};

CliphistEntry *cliphist_entry_new(guint index, const gchar *text, const gchar *mime_type);
void cliphist_entry_free(CliphistEntry *entry);

G_END_DECLS

#endif /* __CLIPHIST_ENTRY_H__ */
