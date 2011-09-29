/*  shiftcolumn.c - a Geany plugin
 *
 *  Copyright 2009 Andrew L Janke <a.janke@gmail.com>
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

#include <vte/vte.h>
#include <geanyplugin.h>
#include <gdk/gdkkeysyms.h>

/*
#include "geany.h"
#include "support.h"

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

#include "ui_utils.h"

#include "document.h"
#include "keybindings.h"
#include "plugindata.h"
#include "geanyfunctions.h"

#include <glib.h>
#include <glib/gprintf.h>*/

//Macros
#define EVAL_KEY_SEQ (GDK_CONTROL_MASK | GDK_SHIFT_MASK) //The eval function will be triggered by    Shift + Ctrl + Enter
#define MACROEXPAND_KEY_SEQ (1 << 3 | GDK_SHIFT_MASK)    //macroexpand callback will be triggered by Shift + Alt + Enter
#define PLUGIN_NAME _("LispEdit")
#define PLUGIN_DESCRIPTION  _("A Geany plugin that provides shorcuts for sending commands to a Common Lisp process.")

PLUGIN_VERSION_CHECK(130);
PLUGIN_SET_INFO(PLUGIN_NAME, PLUGIN_DESCRIPTION, "1.0", "Tapiwa Gutu");

//Variables
GeanyPlugin     *geany_plugin;
GeanyData       *geany_data;
GeanyFunctions  *geany_functions;
static GtkWidget *macroexpand_menu_item = NULL;
static GtkWidget *eval_menu_item = NULL;
static VteTerminal *vte = NULL;
static gboolean have_vte = FALSE;
static GeanyDocument *doc = NULL;
static gint start_pos, end_pos;
//Functions
static void show_error_message(void);
static void set_vte(GtkWidget *widget);
static void init_vte();


// Keybinding(s) 
enum{
   KB_EVAL,
   KB_MACROEXPAND,
   KB_COUNT
   };
PLUGIN_KEY_GROUP(lisp_shortcuts, KB_COUNT)

static void insert_string(GeanyDocument *doc, const gchar *string)
{
	if (doc != NULL)
	{
		gint pos = sci_get_current_position(doc->editor->sci);
		sci_insert_text(doc->editor->sci, pos, string);
	}
}

/*
 * Refer to plugindata.h and geanyfunctions.h sci_get_text_range
 * http://www.cs.bu.edu/teaching/cpp/string/array-vs-ptr/
 * 
 */
 
static void eval_cb(G_GNUC_UNUSED GtkMenuItem *menuitem, G_GNUC_UNUSED gpointer gdata)
{
	if (have_vte)
    {
		doc = document_get_current();
		start_pos = sci_get_current_position(doc->editor->sci);
		if (start_pos > 0) start_pos--;
		gchar letter = sci_get_char_at(doc->editor->sci, start_pos);
		
		switch (letter)
		{ 
			case ')':	dialogs_show_msgbox(GTK_MESSAGE_INFO, "Closing brace found!");
						end_pos = sci_find_matching_brace(doc->editor->sci, start_pos);
						//insert_string(doc, "<<<<<<<<<I AM HERE>>>>>>>>>>>");
						//dialogs_show_msgbox(GTK_MESSAGE_INFO, "Sending expression to eval-1.");
						//vte_terminal_feed_child(vte, "ls\n", strlen("ls\n"));
						break;
						
			default:  dialogs_show_msgbox(GTK_MESSAGE_INFO, "Some other character found.");
			break;
		}
    }
	else
	{
		show_error_message();
	}	
}

static void macroexpand_cb(G_GNUC_UNUSED GtkMenuItem *menuitem, G_GNUC_UNUSED gpointer gdata)
{
	if (have_vte)
    {
		doc = document_get_current();
		start_pos = sci_get_current_position(doc->editor->sci);
		if (start_pos > 0) start_pos--;
		gchar letter = sci_get_char_at(doc->editor->sci, start_pos);
		
		switch (letter)
		{ 
			case ')':	dialogs_show_msgbox(GTK_MESSAGE_INFO, "Closing brace found!");
						end_pos = sci_find_matching_brace(doc->editor->sci, start_pos);
						//insert_string(doc, "<<<<<<<<<I AM HERE>>>>>>>>>>>");
						//dialogs_show_msgbox(GTK_MESSAGE_INFO, "Sending expression to eval-1.");
						//vte_terminal_feed_child(vte, "ls\n", strlen("ls\n"));
						break;
						
			default:	dialogs_show_msgbox(GTK_MESSAGE_INFO, "Some other character found.");
						break;
		}
    }
	else
	{
		show_error_message();
	}
}


static void on_eval_key(G_GNUC_UNUSED guint key_id)
{  
   // sanity check
   if (document_get_current() == NULL){
       return;
       }
   
   eval_cb(NULL, NULL);
}

static void on_macroexpand_key(G_GNUC_UNUSED guint key_id)
{ 
   // sanity check
   if (document_get_current() == NULL){
       return;
       }
   
   macroexpand_cb(NULL, NULL);
} 

void plugin_init(G_GNUC_UNUSED GeanyData *data)
{
	GtkWidget* parent_menu = geany->main_widgets->tools_menu;
	eval_menu_item = gtk_menu_item_new_with_mnemonic(_("Lisp eval"));
	gtk_widget_show(eval_menu_item);
	gtk_container_add(GTK_CONTAINER(parent_menu),							
		eval_menu_item);
	g_signal_connect(eval_menu_item, "activate",
		G_CALLBACK(eval_cb), NULL);
   
	macroexpand_menu_item = gtk_menu_item_new_with_mnemonic(_("Lisp macroexpand-1"));
	gtk_widget_show(macroexpand_menu_item);
	gtk_container_add(GTK_CONTAINER(parent_menu),
		macroexpand_menu_item);
	g_signal_connect(macroexpand_menu_item, "activate",
		G_CALLBACK(macroexpand_cb), NULL);

	/* make sure our menu items aren't called when there is no doc open */
	ui_add_document_sensitive(eval_menu_item);
	ui_add_document_sensitive(macroexpand_menu_item);

	// setup keybindings 
	keybindings_set_item(plugin_key_group, KB_MACROEXPAND, on_macroexpand_key,
		GDK_Return, MACROEXPAND_KEY_SEQ, "lisp_macroexpand_1", _("Lisp macroexpand-1"), macroexpand_menu_item);
	keybindings_set_item(plugin_key_group, KB_EVAL, on_eval_key,
		GDK_Return, EVAL_KEY_SEQ, "lisp_eval", _("Lisp eval"), eval_menu_item);
	
	//Initialiase the binding to the Geany virtual terminal (VTE).
	init_vte();

}

void plugin_cleanup(void)
{
   gtk_widget_destroy(macroexpand_menu_item);
   gtk_widget_destroy(eval_menu_item);
}

/* for when the vte cannot be located */
static void show_error_message(void)
{
    GtkWidget *dlg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s %s",
                        PLUGIN_NAME, _("Plugin"));
    
    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dlg), "%s",
        _("There is currently no terminal loaded in Geany. Enable the terminal "
        "in Geany's prefrences dialog and restart Geany to use the plugin "
        "or disable the plugin to stop seeing this error message."));
    
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
}

static void set_vte(GtkWidget *widget)
{
	if (VTE_IS_TERMINAL(widget))
	{
        have_vte = TRUE;
        vte = VTE_TERMINAL(widget);
    }
    else if (GTK_IS_CONTAINER(widget))
    {
        GList *children, *iter;
        
        children = gtk_container_get_children(GTK_CONTAINER(widget));
        
        for (iter=children; !have_vte && iter; iter=g_list_next(iter))
            set_vte(iter->data);
            
        g_list_free(children);
    }
}

/* locate vte anywhere at or below widget */
static void init_vte()
{
    GtkNotebook *nb;
    GtkWidget *vte_frame = NULL;
    
    nb = GTK_NOTEBOOK(geany_data->main_widgets->message_window_notebook);
    vte_frame = gtk_notebook_get_nth_page(nb, MSG_VTE);
    
    //If the frame contains the vte, go get the VTE and set it to GtkWidget *vte
    if (vte_frame != NULL)
		set_vte(vte_frame);
}
