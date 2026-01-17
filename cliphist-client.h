/*  Cliphist Panel Plugin - Cliphist CLI client
 *  Copyright (c) 2026 - GPL-2.0-or-later
 */

#ifndef __CLIPHIST_CLIENT_H__
#define __CLIPHIST_CLIENT_H__

#include <glib-object.h>
#include "cliphist-plugin.h"

G_BEGIN_DECLS

#define CLIPHIST_TYPE_CLIENT (cliphist_client_get_type())
G_DECLARE_FINAL_TYPE(CliphistClient, cliphist_client, CLIPHIST, CLIENT, GObject)

struct _CliphistClient
{
    GObject parent;
    CliphistPlugin *plugin;
};

CliphistClient *cliphist_client_new(CliphistPlugin *plugin);
void cliphist_client_refresh_entries(CliphistClient *client);
void cliphist_client_select_entry(CliphistClient *client, guint index);
GdkPixbuf *cliphist_client_load_image(CliphistClient *client, guint index, gint max_width, gint max_height);

G_END_DECLS

#endif /* __CLIPHIST_CLIENT_H__ */
