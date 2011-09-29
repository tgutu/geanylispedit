/*  geanylispedit.h - a Geany plugin
 *
 *  Copyright 2011 Tapiwa Gutu
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */

#ifndef GEANYLISPEDIT_H
#define GEANYLISPEDIT_H

#include <vte/vte.h>
#include <geanyplugin.h>
#include <gdk/gdkkeysyms.h>

/*
#include "geany.h"
#include "support.h"
#include "ui_utils.h"
#include "document.h"
#include "keybindings.h"
#include "plugindata.h"
#include "geanyfunctions.h"
#include <glib.h>
#include <glib/gprintf.h>
*/

//Plugin description macros.
#define PLUGIN_NAME 				_("LispEdit")
#define PLUGIN_DESCRIPTION			_("A Geany plugin that provides shorcuts for sending commands to a Common Lisp process.")

PLUGIN_VERSION_CHECK(130);
PLUGIN_SET_INFO(PLUGIN_NAME, PLUGIN_DESCRIPTION, "1.0", "Tapiwa Gutu");

/* Macro definitions for values required by the plugin.
 * The macros define the strings displayed on the menu and the identifier name for the menu entry.*/
#define EVAL_ID_STR					"lisp_eval"
#define MACROEXPAND_ID_STR			"lisp_macroexpand_1"
#define EVAL_MENU_STR				_("LispEdit: macroexpand-1")
#define MACROEXPAND_MENU_STR		_("LispEdit: eval")
#define EVAL_KEY_SEQ				(GDK_CONTROL_MASK | GDK_SHIFT_MASK) //The eval function will be triggered by    Shift + Ctrl + Enter
#define MACROEXPAND_KEY_SEQ			(1 << 3 | GDK_SHIFT_MASK)    //macroexpand callback will be triggered by Shift + Alt + Enter
#define MAX_CMD_LENGTH				1024	//The maximum characters that can be captured in a string.

//Variables required by Geany
GeanyPlugin     *geany_plugin;
GeanyData       *geany_data;
GeanyFunctions  *geany_functions;

//Variables used by the plugin.
static GtkWidget *macroexpand_menu_item = NULL;
static GtkWidget *eval_menu_item = NULL;
static VteTerminal *vte = NULL;
static gboolean have_vte = FALSE;
static GeanyDocument *doc = NULL;
static gint start_pos, end_pos;
gchar cmd_string[MAX_CMD_LENGTH];

//Functions required by Geany
void plugin_cleanup(void);
void plugin_init(G_GNUC_UNUSED GeanyData *data);

//Callback functions used by the plugin
static void cb_eval(G_GNUC_UNUSED GtkMenuItem *menuitem, G_GNUC_UNUSED gpointer gdata);
static void cb_macroexpand(G_GNUC_UNUSED GtkMenuItem *menuitem, G_GNUC_UNUSED gpointer gdata);
static void on_eval_key(G_GNUC_UNUSED guint key_id);
static void on_macroexpand_key(G_GNUC_UNUSED guint key_id);

//Functions
static void init_vte();
static void insert_string(GeanyDocument *doc, const gchar *string);
static void show_error_message(void);
static void set_vte(GtkWidget *widget);

// Keybinding(s) 
enum{
   KB_EVAL,
   KB_MACROEXPAND,
   KB_COUNT
   };

//Set the keybinding group.
PLUGIN_KEY_GROUP(lispedit_shortcuts, KB_COUNT)

#endif
