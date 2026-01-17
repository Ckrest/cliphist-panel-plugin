/*  Cliphist Panel Plugin - XFCE4 panel integration for cliphist
 *  Copyright (c) 2026 - GPL-2.0-or-later
 */

#ifndef __CLIPHIST_PLUGIN_H__
#define __CLIPHIST_PLUGIN_H__

#include <libxfce4panel/libxfce4panel.h>
#include "cliphist-entry.h"

G_BEGIN_DECLS

typedef struct _CliphistPlugin CliphistPlugin;

struct _CliphistPlugin
{
    XfcePanelPlugin *plugin;

    /* Panel Widgets */
    GtkWidget *button;
    GtkWidget *image;

    /* State */
    gint icon_size;

    /* Clipboard entries */
    GList *entries;
};

GtkWidget *cliphist_button_new(CliphistPlugin *plugin);

G_END_DECLS

#endif /* __CLIPHIST_PLUGIN_H__ */
