/*  Cliphist Panel Plugin - Main plugin file
 *  Copyright (c) 2026 - GPL-2.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxfce4util/libxfce4util.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "cliphist-plugin.h"
#include "cliphist-button.h"

static void cliphist_plugin_construct(XfcePanelPlugin *plugin);
static void cliphist_plugin_free(XfcePanelPlugin *plugin, CliphistPlugin *cliphist_plugin);
static gboolean cliphist_plugin_size_changed(XfcePanelPlugin *plugin, gint size, CliphistPlugin *cliphist_plugin);

XFCE_PANEL_PLUGIN_REGISTER(cliphist_plugin_construct);

static void
cliphist_plugin_construct(XfcePanelPlugin *plugin)
{
    CliphistPlugin *cliphist_plugin = g_slice_new0(CliphistPlugin);
    cliphist_plugin->plugin = plugin;

    g_signal_connect(plugin, "free-data", G_CALLBACK(cliphist_plugin_free), cliphist_plugin);
    g_signal_connect(plugin, "size-changed", G_CALLBACK(cliphist_plugin_size_changed), cliphist_plugin);

    cliphist_plugin->button = cliphist_button_new(cliphist_plugin);
    gtk_container_add(GTK_CONTAINER(plugin), cliphist_plugin->button);

    gtk_widget_show_all(cliphist_plugin->button);
}

static void
cliphist_plugin_free(XfcePanelPlugin *plugin, CliphistPlugin *cliphist_plugin)
{
    if (cliphist_plugin) {
        g_list_free_full(cliphist_plugin->entries, (GDestroyNotify)cliphist_entry_free);
        g_slice_free(CliphistPlugin, cliphist_plugin);
    }
}

static gboolean
cliphist_plugin_size_changed(XfcePanelPlugin *plugin, gint size, CliphistPlugin *cliphist_plugin)
{
    gint icon_size = size - 9;
    if (icon_size < 16)
        icon_size = 16;

    cliphist_plugin->icon_size = icon_size;

    cliphist_button_update_icon((CliphistButton *)cliphist_plugin->button);
    return TRUE;
}
