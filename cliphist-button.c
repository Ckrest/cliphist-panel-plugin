/*  Cliphist Panel Plugin - Button widget
 *  Copyright (c) 2026 - GPL-2.0-or-later
 */

#include "cliphist-button.h"
#include "cliphist-client.h"
#include "cliphist-entry.h"

G_DEFINE_TYPE(CliphistButton, cliphist_button, GTK_TYPE_TOGGLE_BUTTON);

static GtkWidget *cliphist_button_create_menu(CliphistButton *button);

static void
cliphist_button_menu_deactivate(CliphistButton *button, GtkMenuShell *menu)
{
    g_return_if_fail(button != NULL);
    g_return_if_fail(GTK_IS_MENU_SHELL(menu));

    if (button->deactivate_id) {
        g_signal_handler_disconnect(menu, button->deactivate_id);
        button->deactivate_id = 0;
    }

    if (button->menu != NULL) {
        gtk_menu_detach(GTK_MENU(button->menu));
        gtk_menu_popdown(GTK_MENU(button->menu));
        button->menu = NULL;
    }

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), FALSE);
}

static void
on_entry_selected(GtkMenuItem *menuitem, gpointer user_data)
{
    guint index = GPOINTER_TO_UINT(user_data);
    CliphistButton *button = g_object_get_data(G_OBJECT(menuitem), "cliphist-button");

    CliphistClient *client = cliphist_client_new(button->plugin);
    cliphist_client_select_entry(client, index);
    g_object_unref(client);
}

static gboolean
cliphist_button_button_press(GtkWidget *widget, GdkEventButton *event)
{
    CliphistButton *button = CLIPHIST_BUTTON(widget);

    if ((event->type == GDK_2BUTTON_PRESS) || (event->type == GDK_3BUTTON_PRESS))
        return TRUE;

    if (event->button == 1 && button->menu == NULL) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
        button->menu = cliphist_button_create_menu(button);
        gtk_menu_attach_to_widget(GTK_MENU(button->menu), widget, NULL);

        if (button->deactivate_id == 0) {
            button->deactivate_id = g_signal_connect_swapped(
                GTK_MENU_SHELL(button->menu), "deactivate",
                G_CALLBACK(cliphist_button_menu_deactivate), button);
        }

#if LIBXFCE4PANEL_CHECK_VERSION(4, 17, 2)
        xfce_panel_plugin_popup_menu(XFCE_PANEL_PLUGIN(button->plugin->plugin), GTK_MENU(button->menu),
                                     widget, (GdkEvent *)event);
#else
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
        gtk_menu_popup(GTK_MENU(button->menu),
                      NULL, NULL,
                      xfce_panel_plugin_position_menu, button->plugin->plugin,
                      1,
                      event->time);
G_GNUC_END_IGNORE_DEPRECATIONS
#endif

        return TRUE;
    }

    return FALSE;
}

static void
cliphist_button_init(CliphistButton *button)
{
    button->menu = NULL;
    button->deactivate_id = 0;
}

static void
cliphist_button_class_init(CliphistButtonClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    widget_class->button_press_event = cliphist_button_button_press;
}

GtkWidget *
cliphist_button_new(CliphistPlugin *plugin)
{
    CliphistButton *button;
    GtkCssProvider *css_provider;
    GtkStyleContext *context;

    button = g_object_new(CLIPHIST_TYPE_BUTTON, NULL);
    button->plugin = plugin;

    GtkIconTheme *icon_theme = gtk_icon_theme_get_default();

    if (gtk_icon_theme_has_icon(icon_theme, "clipman-symbolic"))
        plugin->image = gtk_image_new_from_icon_name("clipman-symbolic", GTK_ICON_SIZE_MENU);
    else if (gtk_icon_theme_has_icon(icon_theme, "edit-paste-symbolic"))
        plugin->image = gtk_image_new_from_icon_name("edit-paste-symbolic", GTK_ICON_SIZE_MENU);
    else
        plugin->image = gtk_image_new_from_icon_name("edit-paste", GTK_ICON_SIZE_MENU);

    gtk_container_add(GTK_CONTAINER(button), plugin->image);

    gtk_widget_set_tooltip_text(GTK_WIDGET(button), "Clipboard History");

    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
        "button { "
        "  border: none; "
        "  background: transparent; "
        "  box-shadow: none; "
        "  outline: none; "
        "  padding: 0; "
        "} "
        "button:hover { "
        "  background: rgba(255, 255, 255, 0.1); "
        "} "
        "button:checked, button:active { "
        "  border: none; "
        "  background: rgba(255, 255, 255, 0.15); "
        "  box-shadow: none; "
        "  outline: none; "
        "} ",
        -1, NULL);
    context = gtk_widget_get_style_context(GTK_WIDGET(button));
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider);

    gtk_widget_show_all(GTK_WIDGET(button));

    return GTK_WIDGET(button);
}

void
cliphist_button_update_icon(CliphistButton *button)
{
    CliphistPlugin *plugin = button->plugin;

    if (plugin->icon_size <= 0) {
        return;
    }

    gtk_image_set_pixel_size(GTK_IMAGE(plugin->image), plugin->icon_size);
}

static GtkWidget *
cliphist_button_create_menu(CliphistButton *button)
{
    CliphistPlugin *plugin = button->plugin;
    GtkWidget *menu;
    GtkWidget *item;

    menu = gtk_menu_new();
    gtk_widget_set_size_request(menu, 400, -1);

    CliphistClient *client = cliphist_client_new(plugin);
    cliphist_client_refresh_entries(client);
    g_object_unref(client);

    if (plugin->entries == NULL) {
        item = gtk_menu_item_new_with_label("No clipboard history");
        gtk_widget_set_sensitive(item, FALSE);
        gtk_widget_set_margin_top(item, 4);
        gtk_widget_set_margin_bottom(item, 4);
        gtk_widget_set_margin_start(item, 12);
        gtk_widget_set_margin_end(item, 12);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    } else {
        gboolean first = TRUE;
        for (GList *l = plugin->entries; l != NULL; l = l->next) {
            CliphistEntry *entry = l->data;

            if (!first) {
                GtkWidget *sep = gtk_separator_menu_item_new();
                gtk_widget_set_margin_top(sep, 2);
                gtk_widget_set_margin_bottom(sep, 2);
                gtk_menu_shell_append(GTK_MENU_SHELL(menu), sep);
            }
            first = FALSE;

            if (entry->is_image) {
                CliphistClient *client = cliphist_client_new(plugin);
                GdkPixbuf *pixbuf = cliphist_client_load_image(client, entry->index, 300, 200);
                g_object_unref(client);

                if (pixbuf) {
                    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
                    g_object_unref(pixbuf);

                    item = gtk_menu_item_new();
                    gtk_container_add(GTK_CONTAINER(item), image);
                } else {
                    item = gtk_menu_item_new_with_label(entry->display_text);
                }
            } else {
                item = gtk_menu_item_new_with_label(entry->display_text);
            }

            gtk_widget_set_margin_top(item, 4);
            gtk_widget_set_margin_bottom(item, 4);
            gtk_widget_set_margin_start(item, 12);
            gtk_widget_set_margin_end(item, 12);

            g_object_set_data(G_OBJECT(item), "cliphist-button", button);
            g_signal_connect(item, "activate", G_CALLBACK(on_entry_selected),
                           GUINT_TO_POINTER(entry->index));

            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
        }
    }

    gtk_widget_show_all(menu);

    return menu;
}
