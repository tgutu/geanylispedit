/*  geanylispedit.c - a Geany plugin
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


#include "geanylispedit.h"

/* Locates the widget that contains the virtual terminal(VTE) */
static void init_vte(void)
{
    GtkNotebook *nb;
    GtkWidget *vte_frame = NULL;
    
    nb = GTK_NOTEBOOK(geany_data->main_widgets->message_window_notebook);
    vte_frame = gtk_notebook_get_nth_page(nb, MSG_VTE);
    
    //If the frame contains the vte, go get the VTE and set it to GtkWidget *vte
    if (vte_frame != NULL)
		set_vte(vte_frame);
}

static void cb_eval(G_GNUC_UNUSED GtkMenuItem *menuitem, G_GNUC_UNUSED gpointer gdata)
{
	if (have_vte)
    {
		doc = document_get_current();
		end_pos = sci_get_current_position(doc->editor->sci);
		if (end_pos > 0) end_pos--;
		gchar letter = sci_get_char_at(doc->editor->sci, end_pos);
		
		switch (letter)
		{ 
			case ')':	
						start_pos = sci_find_matching_brace(doc->editor->sci, end_pos);
						if (start_pos < 0)
						{
							dialogs_show_msgbox(GTK_MESSAGE_WARNING, "Found an isolated closing brace!!!");
						}
						else if (start_pos >= 0)
						{
							sci_get_text_range(doc->editor->sci, start_pos, ++end_pos, cmd_string);
							vte_terminal_feed_child(vte, "\n", strlen("\n"));
							vte_terminal_feed_child(vte, cmd_string, strlen(cmd_string));
							vte_terminal_feed_child(vte, "\n", strlen("\n"));
						}
						break;						
		}
    }
	else
	{
		show_error_message();
	}	
}

static void cb_macroexpand(G_GNUC_UNUSED GtkMenuItem *menuitem, G_GNUC_UNUSED gpointer gdata)
{
	if (have_vte)
    {
		doc = document_get_current();
		end_pos = sci_get_current_position(doc->editor->sci);
		if (end_pos > 0) end_pos--;
		gchar letter = sci_get_char_at(doc->editor->sci, end_pos );
		
		switch (letter)
		{ 
			case ')':	start_pos = sci_find_matching_brace(doc->editor->sci, end_pos);
						
						if (start_pos < 0)
							dialogs_show_msgbox(GTK_MESSAGE_WARNING, "Found an isolated closing brace!!!");
						else
						{
							sci_get_text_range(doc->editor->sci, start_pos, ++end_pos, cmd_string);
							vte_terminal_feed_child(vte, "\n(macroexpand-1 '", strlen("\n(macroexpand-1 '"));
							vte_terminal_feed_child(vte, cmd_string, strlen(cmd_string));
							vte_terminal_feed_child(vte, ")\n", strlen(")\n"));
						}
						break;
		}
    }
	else
	{
		show_error_message();
	}
}

/* Callback functions for the keyboard shortcuts */

static void on_eval_key(G_GNUC_UNUSED guint key_id)
{  
   // sanity check
   if (document_get_current() == NULL){
       return;
       }
   
   cb_eval(NULL, NULL);
}

static void on_macroexpand_key(G_GNUC_UNUSED guint key_id)
{ 
   // sanity check
   if (document_get_current() == NULL){
       return;
       }
   
   cb_macroexpand(NULL, NULL);
} 

/* The Geany plugin initialization function. This is called automatically by Geany when a user installs the plugin. */
void plugin_init(G_GNUC_UNUSED GeanyData *data)
{
	GtkWidget* parent_menu = ui_lookup_widget(GTK_WIDGET(geany->main_widgets->window), "edit1_menu");//"tools1_menu"
	eval_menu_item = gtk_menu_item_new_with_mnemonic(EVAL_MENU_STR);
	gtk_widget_show(eval_menu_item);
	gtk_container_add(GTK_CONTAINER(parent_menu),							
		eval_menu_item);
	g_signal_connect(eval_menu_item, "activate",
		G_CALLBACK(cb_eval), NULL);
   
	macroexpand_menu_item = gtk_menu_item_new_with_mnemonic(MACROEXPAND_MENU_STR);
	gtk_widget_show(macroexpand_menu_item);
	gtk_container_add(GTK_CONTAINER(parent_menu),
		macroexpand_menu_item);
	g_signal_connect(macroexpand_menu_item, "activate",
		G_CALLBACK(cb_macroexpand), NULL);

	/* make sure our menu items aren't called when there is no doc open */
	ui_add_document_sensitive(eval_menu_item);
	ui_add_document_sensitive(macroexpand_menu_item);

	// setup keybindings
	keybindings_set_item(plugin_key_group, KB_MACROEXPAND, on_macroexpand_key,
		GDK_Return, MACROEXPAND_KEY_SEQ, MACROEXPAND_ID_STR, MACROEXPAND_MENU_STR, macroexpand_menu_item);
	keybindings_set_item(plugin_key_group, KB_EVAL, on_eval_key,
		GDK_Return, EVAL_KEY_SEQ, EVAL_ID_STR, EVAL_MENU_STR, eval_menu_item);
	
	//Initialiase the binding to the Geany virtual terminal (VTE).
	init_vte();

}

void plugin_cleanup(void)
{
   gtk_widget_destroy(macroexpand_menu_item);
   gtk_widget_destroy(eval_menu_item);
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

/* This function displays an error function when the user attempts to sent commands to a Lisp process running in the terminal
 * if no terminal exists. */
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
    g_free(cmd_string);
}
