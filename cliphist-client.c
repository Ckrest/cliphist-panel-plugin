/*  Cliphist Panel Plugin - Cliphist CLI client
 *  Copyright (c) 2026 - GPL-2.0-or-later
 */

#include "cliphist-client.h"
#include "cliphist-entry.h"
#include <string.h>
#include <unistd.h>

G_DEFINE_TYPE(CliphistClient, cliphist_client, G_TYPE_OBJECT);

#define MAX_ENTRIES 6

static gchar *
execute_command(const gchar *command)
{
    gchar *output = NULL;
    GError *error = NULL;

    if (!g_spawn_command_line_sync(command, &output, NULL, NULL, &error)) {
        g_warning("Failed to execute command: %s", error ? error->message : "unknown error");
        g_clear_error(&error);
        return NULL;
    }

    return output;
}

static void
cliphist_client_init(CliphistClient *client)
{
}

static void
cliphist_client_class_init(CliphistClientClass *klass)
{
}

CliphistClient *
cliphist_client_new(CliphistPlugin *plugin)
{
    CliphistClient *client;

    client = g_object_new(CLIPHIST_TYPE_CLIENT, NULL);
    client->plugin = plugin;

    return client;
}

void
cliphist_client_refresh_entries(CliphistClient *client)
{
    gchar *output;
    gchar **lines;
    guint count = 0;

    g_list_free_full(client->plugin->entries, (GDestroyNotify)cliphist_entry_free);
    client->plugin->entries = NULL;

    output = execute_command("cliphist list");
    if (!output || strlen(output) == 0) {
        g_free(output);
        return;
    }

    lines = g_strsplit(output, "\n", -1);
    g_free(output);

    for (gint i = 0; lines[i] != NULL && count < MAX_ENTRIES; i++) {
        g_strstrip(lines[i]);
        if (strlen(lines[i]) == 0)
            continue;

        gchar *tab_pos = strchr(lines[i], '\t');
        if (!tab_pos)
            continue;

        gchar *text = tab_pos + 1;
        gchar *mime_type = "text/plain";

        if (g_str_has_prefix(text, "binary data ")) {
            mime_type = text + strlen("binary data ");
        }

        CliphistEntry *entry = cliphist_entry_new(count, text, mime_type);
        client->plugin->entries = g_list_append(client->plugin->entries, entry);
        count++;
    }

    g_strfreev(lines);
}

void
cliphist_client_select_entry(CliphistClient *client, guint index)
{
    gchar *cmd = g_strdup_printf("/bin/sh -c 'cliphist list | sed -n \"%d p\" | cliphist decode | wl-copy'", index + 1);
    execute_command(cmd);
    g_free(cmd);
}

GdkPixbuf *
cliphist_client_load_image(CliphistClient *client, guint index, gint max_width, gint max_height)
{
    gchar *cmd;
    gchar *tmpfile;
    gint fd;
    GError *error = NULL;
    GdkPixbuf *pixbuf = NULL;
    GdkPixbuf *scaled_pixbuf = NULL;

    fd = g_file_open_tmp("cliphist-XXXXXX.png", &tmpfile, &error);
    if (fd < 0) {
        g_warning("Failed to create temp file: %s", error ? error->message : "unknown error");
        g_clear_error(&error);
        return NULL;
    }
    close(fd);

    cmd = g_strdup_printf("/bin/sh -c 'cliphist list | sed -n \"%d p\" | cliphist decode > %s'", index + 1, tmpfile);

    if (!g_spawn_command_line_sync(cmd, NULL, NULL, NULL, &error)) {
        g_warning("Failed to execute cliphist decode: %s", error ? error->message : "unknown error");
        g_clear_error(&error);
        g_free(cmd);
        unlink(tmpfile);
        g_free(tmpfile);
        return NULL;
    }
    g_free(cmd);

    pixbuf = gdk_pixbuf_new_from_file(tmpfile, &error);
    unlink(tmpfile);
    g_free(tmpfile);

    if (error) {
        g_warning("Failed to load image from temp file: %s", error->message);
        g_clear_error(&error);
        return NULL;
    }

    if (!pixbuf) {
        return NULL;
    }

    gint width = gdk_pixbuf_get_width(pixbuf);
    gint height = gdk_pixbuf_get_height(pixbuf);

    if (width <= max_width && height <= max_height) {
        return pixbuf;
    }

    gdouble scale_w = (gdouble)max_width / width;
    gdouble scale_h = (gdouble)max_height / height;
    gdouble scale = MIN(scale_w, scale_h);

    gint new_width = (gint)(width * scale);
    gint new_height = (gint)(height * scale);

    scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
    g_object_unref(pixbuf);

    return scaled_pixbuf;
}
