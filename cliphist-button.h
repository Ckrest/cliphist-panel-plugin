/*  Cliphist Panel Plugin - Button widget
 *  Copyright (c) 2026 - GPL-2.0-or-later
 */

#ifndef __CLIPHIST_BUTTON_H__
#define __CLIPHIST_BUTTON_H__

#include <gtk/gtk.h>
#include "cliphist-plugin.h"

G_BEGIN_DECLS

#define CLIPHIST_TYPE_BUTTON (cliphist_button_get_type())
G_DECLARE_FINAL_TYPE(CliphistButton, cliphist_button, CLIPHIST, BUTTON, GtkToggleButton)

struct _CliphistButton
{
    GtkToggleButton parent;
    CliphistPlugin *plugin;
    GtkWidget *menu;
    gulong deactivate_id;
};

GtkWidget *cliphist_button_new(CliphistPlugin *plugin);
void cliphist_button_update_icon(CliphistButton *button);

G_END_DECLS

#endif /* __CLIPHIST_BUTTON_H__ */
