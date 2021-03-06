/**
 **	A GTK widget showing a list of shapes from an image file.
 **
 **	Written: 7/25/99 - JSF
 **/

/*
Copyright (C) 1999  Jeffrey S. Freedman

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#ifdef XWIN
#include <gdk/gdkx.h>
#endif
#include <glib.h>
#include "shapelst.h"
#include "vgafile.h"
#include "ibuf8.h"
#include "Flex.h"
#include "u7drag.h"

using std::cout;
using std::endl;
using std::strlen;

/*
 *	Blit onto screen.
 */

inline void Shape_chooser::show
	(
	int x, int y, int w, int h	// Area to blit.
	)
	{
	Shape_draw::show(draw->window, x, y, w, h);
	if (selected >= 0)		// Show selected.
		{
		Rectangle b = info[selected].box;
					// Draw yellow box.
		gdk_draw_rectangle(draw->window, drawgc, FALSE, 
							b.x, b.y, b.w, b.h);
		}
	}

/*
 *	Select an entry.  This should be called after rendering
 *	the shape.
 */

void Shape_chooser::select
	(
	int new_sel
	)
	{
	selected = new_sel;
	int shapenum = info[selected].shapenum;
					// Update spin-button value, range.
	gtk_widget_set_sensitive(fspin, true);
	gtk_adjustment_set_value(frame_adj, info[selected].framenum);
	int nframes = ifile->get_num_frames(shapenum);
	frame_adj->upper = nframes - 1;
	gtk_adjustment_changed(frame_adj);
	gtk_widget_set_sensitive(fspin, true);
					// Remove prev. selection msg.
//	gtk_statusbar_pop(GTK_STATUSBAR(sbar), sbar_sel);
	char buf[150];			// Show new selection.
	g_snprintf(buf, sizeof(buf), "Shape %d (%d frames)",
						shapenum, nframes);
	if (names && names[shapenum])
		{
		int len = strlen(buf);
		g_snprintf(buf + len, sizeof(buf) - len, 
						":  '%s'", names[shapenum]);
		}
	gtk_statusbar_push(GTK_STATUSBAR(sbar), sbar_sel, buf);
	}

/*
 *	Render as many shapes as fit in the shape chooser window.
 */

void Shape_chooser::render
	(
	)
	{
	const int border = 4;		// Border at bottom, sides.
					// Look for selected frame.
	int selshape = -1, selframe = -1, new_selected = -1;
	if (selected >= 0)		// Save selection info.
		{
		selshape = info[selected].shapenum;
		selframe = info[selected].framenum;
		}
					// Remove "selected" message.
	//gtk_statusbar_pop(GTK_STATUSBAR(sbar), sbar_sel);
	delete [] info;			// Delete old info. list.
	int shapenum = shapenum0;
	int framenum = 0;
					// Get drawing area dimensions.
	gint winw = draw->allocation.width, winh = draw->allocation.height;
					// Provide more than enough room.
	info = new Shape_info[1024];
					// Clear window first.
	iwin->fill8(0);			// ++++Which color?
	int x = 0;
					// Get first shape.
	Shape_frame *shape = ifile->get_shape(shapenum, 0);
	int sw;
	info_cnt = 0;			// Count them.
	int curr_y = 0;
	int row_h = 0;
	int rows = 0;			// Count rows.
	do {
		while (shapenum<num_shapes) {
			if(shape) {
					// Check if we've exceeded max width
				if (x + (sw = shape->get_width()) > winw)
					break;
						// Get height, top y-coord.
				int sh = shape->get_height();
				if(sh>row_h)
					row_h = sh;
				int sy = curr_y+border;
				shape->paint(iwin, x + shape->get_xleft(),
						sy + shape->get_yabove());
				if (sh > winh) {
					sy += sh - winh;
					sh = winh;
				}
					// Store info. about where drawn.
				info[info_cnt].set(shapenum, framenum, 
								x, sy, sw, sh);
				if (shapenum == selshape)
						// Found the selected shape.
					new_selected = info_cnt;
			}
			shapenum++;		// Next shape.
			framenum = shapenum == selshape ? selframe : 0;
			shape = shapenum >= num_shapes ? 0 
					: ifile->get_shape(shapenum, framenum);
			if(shape) {
				x += sw + border;
				info_cnt++;
			}
		}
		curr_y += row_h + border;
		x = 0;
		rows++;
	} while(shapenum<num_shapes && (curr_y + 36 < winh));
	num_per_row = info_cnt/rows;		// Figure average.
	if (new_selected == -1)
		unselect(false);
	else
		select(new_selected);
	adjust_scrollbar();		// Set new scroll values.
}
	
/*
 *	Configure the viewing window.
 */

gint Shape_chooser::configure
	(
	GtkWidget *widget,		// The view window.
	GdkEventConfigure *event,
	gpointer data			// ->Shape_chooser
	)
	{
	Shape_chooser *chooser = (Shape_chooser *) data;
	chooser->Shape_draw::configure(widget);
	chooser->render();
	chooser->adjust_scrollbar();	// Figure new scroll amounts.
	return (TRUE);
	}

/*
 *	Handle an expose event.
 */

gint Shape_chooser::expose
	(
	GtkWidget *widget,		// The view window.
	GdkEventExpose *event,
	gpointer data			// ->Shape_chooser.
	)
	{
	Shape_chooser *chooser = (Shape_chooser *) data;
	chooser->show(event->area.x, event->area.y, event->area.width,
							event->area.height);
	return (TRUE);
	}

/*
 *	Handle a mouse button press event.
 */

gint Shape_chooser::mouse_press
	(
	GtkWidget *widget,		// The view window.
	GdkEventButton *event,
	gpointer data			// ->Shape_chooser.
	)
	{
	Shape_chooser *chooser = (Shape_chooser *) data;
	int old_selected = chooser->selected;
					// Search through entries.
	for (int i = 0; i < chooser->info_cnt; i++)
		if (chooser->info[i].box.has_point(
					(int) event->x, (int) event->y))
			{		// Found the box?
//			if (i == old_selected)
//				return TRUE;
					// Indicate we can dra.
			GtkTargetEntry tents[1];
			tents[0].target = U7_TARGET_SHAPEID_NAME;
			tents[0].flags = 0;
			tents[0].info = U7_TARGET_SHAPEID;
			gtk_drag_source_set (chooser->draw, 
				GDK_BUTTON1_MASK, tents, 1,
			   (GdkDragAction)(GDK_ACTION_COPY | GDK_ACTION_MOVE));
			chooser->selected = i;
			chooser->render();
			chooser->show();
					// Tell client.
			if (chooser->sel_changed)
				(*chooser->sel_changed)();
			break;
			}
	return (TRUE);
	}

/*
 *	Someone wants the dragged shape.
 */

void Shape_chooser::drag_data_get
	(
	GtkWidget *widget,		// The view window.
	GdkDragContext *context,
	GtkSelectionData *seldata,	// Fill this in.
	guint info,
	guint time,
	gpointer data			// ->Shape_chooser.
	)
	{
	cout << "In DRAG_DATA_GET" << endl;
	Shape_chooser *chooser = (Shape_chooser *) data;
	if (chooser->selected < 0 || info != U7_TARGET_SHAPEID)
		return;			// Not sure about this.
	guchar buf[30];
	int file = chooser->ifile->get_u7drag_type();
	if (file == U7_SHAPE_UNK)
		U7_SHAPE_SHAPES;	// Just assume it's shapes.vga.
	Shape_info& shinfo = chooser->info[chooser->selected];
	int len = Store_u7_shapeid(buf, file, shinfo.shapenum, 
							shinfo.framenum);
	cout << "Setting selection data (" << shinfo.shapenum <<
			'/' << shinfo.framenum << ')' << endl;
					// Make us owner of xdndselection.
	gtk_selection_owner_set(widget, gdk_atom_intern("XdndSelection", 0),
								time);
					// Set data.
	gtk_selection_data_set(seldata,
			gdk_atom_intern(U7_TARGET_SHAPEID_NAME, 0),
                                				8, buf, len);
	}

/*
 *	Another app. has claimed the selection.
 */

gint Shape_chooser::selection_clear
	(
	GtkWidget *widget,		// The view window.
	GdkEventSelection *event,
	gpointer data			// ->Shape_chooser.
	)
	{
//	Shape_chooser *chooser = (Shape_chooser *) data;
	cout << "SELECTION_CLEAR" << endl;
	return TRUE;
	}

/*
 *	Beginning of a drag.
 */

gint Shape_chooser::drag_begin
	(
	GtkWidget *widget,		// The view window.
	GdkDragContext *context,
	gpointer data			// ->Shape_chooser.
	)
	{
	cout << "In DRAG_BEGIN" << endl;
	Shape_chooser *chooser = (Shape_chooser *) data;
	if (chooser->selected < 0)
		return FALSE;		// ++++Display a halt bitmap.
					// Get ->shape.
	Shape_info& shinfo = chooser->info[chooser->selected];
	Shape_frame *shape = chooser->ifile->get_shape(shinfo.shapenum, 
							shinfo.framenum);
	if (!shape)
		return FALSE;
	int w = shape->get_width(), h = shape->get_height(),
		xright = shape->get_xright(), ybelow = shape->get_ybelow();
	Image_buffer8 tbuf(w, h);	// Create buffer to render to.
	tbuf.fill8(0xff);		// Fill with 'transparent' pixel.
	unsigned char *tbits = tbuf.get_bits();
	shape->paint(&tbuf, w - 1 - xright, h - 1 - ybelow);
					// Put shape on a pixmap.
	GdkPixmap *pixmap = gdk_pixmap_new(widget->window, w, h, -1);
	gdk_draw_indexed_image(pixmap, chooser->drawgc, 0, 0, w, h,
			GDK_RGB_DITHER_NORMAL, tbits,
			tbuf.get_line_width(), chooser->palette);
	int mask_stride = (w + 7)/8;	// Round up to nearest byte.
	char *mdata = new char[mask_stride*h];
	for (int y = 0; y < h; y++)	// Do each row.
					// Do each byte.
		for (int b = 0; b < mask_stride; b++)
			{
			char bits = 0;
			unsigned char *vals = tbits + y*w + b*8;
			for (int i = 0; i < 8; i++)
				if (vals[i] != 0xff)
					bits |= (1<<i);
			mdata[y*mask_stride + b] = bits;
			}
	GdkBitmap *mask = gdk_bitmap_create_from_data(widget->window,
							mdata, w, h);
	delete mdata;
					// This will be the shape dragged.
	gtk_drag_set_icon_pixmap(context,
			gdk_window_get_colormap(widget->window), pixmap, mask,
					w - 2 - xright, h - 2 - ybelow);
	gdk_pixmap_unref(pixmap);
	gdk_bitmap_unref(mask);
	return TRUE;
	}

/*
 *	Scroll to a new shape/frame.
 */

void Shape_chooser::scroll
	(
	int newindex			// Abs. index of leftmost to show.
	)
	{
	if (shapenum0 < newindex)	// Going forwards?
		shapenum0 = newindex < num_shapes ? newindex : num_shapes;
	else if (shapenum0 > newindex)	// Backwards?
		shapenum0 = newindex >= 0 ? newindex : 0;
	render();
	show();
	}

/*
 *	Adjust scroll amounts.
 */

void Shape_chooser::adjust_scrollbar
	(
	)
	{	
	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(shape_scroll));
	adj->step_increment = num_per_row ? num_per_row : 1;
	adj->page_increment = info_cnt;
	adj->page_size = info_cnt;
	gtk_signal_emit_by_name(GTK_OBJECT(adj), "changed");
	}

/*
 *	Handle a scrollbar event.
 */

void Shape_chooser::scrolled
	(
	GtkAdjustment *adj,		// The adjustment.
	gpointer data			// ->Shape_chooser.
	)
	{
	Shape_chooser *chooser = (Shape_chooser *) data;
cout << "Scrolled to " << adj->value << '\n';
	gint newindex = (gint) adj->value;
	chooser->scroll(newindex);
	}

/*
 *	Handle a change to the 'frame' spin button.
 */

void Shape_chooser::frame_changed
	(
	GtkAdjustment *adj,		// The adjustment.
	gpointer data			// ->Shape_chooser.
	)
	{
	Shape_chooser *chooser = (Shape_chooser *) data;
cout << "Frame changed to " << adj->value << '\n';
	gint newframe = (gint) adj->value;
	if (chooser->selected >= 0)
		{
		Shape_info& shinfo = chooser->info[chooser->selected];
		int nframes = chooser->ifile->get_num_frames(shinfo.shapenum);
		if (newframe >= nframes)	// Just checking
			return;
		shinfo.framenum = newframe;
		chooser->render();
		chooser->show();
		}
	}

/*
 *	Search for an entry.
 */

void Shape_chooser::search
	(
	char *srch,			// What to search for.
	int dir				// 1 or -1.
	)
	{
	if (!names)
		return;			// In future, maybe find shape #?
					// Start with selection, or top.
	int start = info[selected >= 0 ? selected : 0].shapenum + dir;
	int stop = dir == -1 ? -1 : num_shapes;
	int i;
	for (i = start; i != stop; i += dir)
		{
		if (strstr(names[i], srch))
			break;		// Found it.
		}
	if (i == stop)
		return;			// Not found.
	scroll(i);
	select(0);			// It's at top.
	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(shape_scroll));
	gtk_adjustment_set_value(adj, info[0].shapenum);
	show();
	}

/*
 *	Callbacks for 'search' buttons:
 */
extern "C" void
on_find_shape_down_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	Shape_chooser *chooser = (Shape_chooser *) user_data;
	chooser->search(gtk_entry_get_text(
			GTK_ENTRY(chooser->get_find_text())), 1);
}
extern "C" void
on_find_shape_up_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	Shape_chooser *chooser = (Shape_chooser *) user_data;
	chooser->search(gtk_entry_get_text(
			GTK_ENTRY(chooser->get_find_text())), -1);
}

/*
 *	Create box with 'find' and 'history' controls.
 */

GtkWidget *Shape_chooser::create_search_controls
	(
	)
	{
	GtkWidget *frame = gtk_frame_new (NULL);
	gtk_widget_show(frame);

	GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox1);
	gtk_container_add (GTK_CONTAINER (frame), hbox1);

	GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox2);
	gtk_box_pack_start (GTK_BOX (hbox1), hbox2, FALSE, FALSE, 0);

	GtkWidget *label1 = gtk_label_new ("Find:");
	gtk_widget_show (label1);
	gtk_box_pack_start (GTK_BOX (hbox2), label1, FALSE, FALSE, 0);
	gtk_misc_set_padding (GTK_MISC (label1), 4, 0);

	find_text = gtk_entry_new ();
	gtk_widget_show (find_text);
	gtk_box_pack_start (GTK_BOX (hbox2), find_text, FALSE, FALSE, 0);
	gtk_widget_set_usize (find_text, 110, -2);

	GtkWidget *hbuttonbox1 = gtk_hbutton_box_new ();
	gtk_widget_show (hbuttonbox1);
	gtk_box_pack_start (GTK_BOX (hbox2), hbuttonbox1, FALSE, FALSE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), 
							GTK_BUTTONBOX_START);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (hbuttonbox1), 0);
//	gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX(hbuttonbox1), 0, -1);

	GtkWidget *find_shape_down = gtk_button_new_with_label ("Down");
	gtk_widget_show (find_shape_down);
	gtk_container_add (GTK_CONTAINER (hbuttonbox1), find_shape_down);
	GTK_WIDGET_SET_FLAGS (find_shape_down, GTK_CAN_DEFAULT);

	GtkWidget *find_shape_up = gtk_button_new_with_label ("Up");
	gtk_widget_show (find_shape_up);
	gtk_container_add (GTK_CONTAINER (hbuttonbox1), find_shape_up);
	GTK_WIDGET_SET_FLAGS (find_shape_up, GTK_CAN_DEFAULT);
#if 0
	GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox3);
	gtk_box_pack_start (GTK_BOX (hbox1), hbox3, FALSE, FALSE, 0);

	GtkWidget *label2 = gtk_label_new ("History:");
	gtk_widget_show (label2);
	gtk_box_pack_start (GTK_BOX (hbox3), label2, FALSE, FALSE, 0);
	gtk_misc_set_padding (GTK_MISC (label2), 4, 0);

	GtkWidget *combo1 = gtk_combo_new ();
	gtk_widget_show (combo1);
	gtk_box_pack_start (GTK_BOX (hbox3), combo1, TRUE, TRUE, 0);

	GtkWidget *history_combo = GTK_COMBO (combo1)->entry;
	gtk_widget_show (history_combo);
	gtk_widget_set_usize (history_combo, 120, -2);
#endif

	gtk_signal_connect (GTK_OBJECT (find_shape_down), "clicked",
                      GTK_SIGNAL_FUNC (on_find_shape_down_clicked),
                      this);
	gtk_signal_connect (GTK_OBJECT (find_shape_up), "clicked",
                      GTK_SIGNAL_FUNC (on_find_shape_up_clicked),
                      this);
	return frame;
	}

/*
 *	Create the list.
 */

Shape_chooser::Shape_chooser
	(
	Vga_file *i,			// Where they're kept.
	unsigned char *palbuf,		// Palette, 3*256 bytes (rgb triples).
	int w, int h			// Dimensions.
	) : Shape_draw(i, palbuf, gtk_drawing_area_new()), find_text(0),
		shapenum0(0),
		info(0), info_cnt(0), num_per_row(0), 
		selected(-1), sel_changed(0)
	{
	guint32 colors[256];
	for (int i = 0; i < 256; i++)
		colors[i] = (palbuf[3*i]<<16)*4 + (palbuf[3*i+1]<<8)*4 + 
							palbuf[3*i+2]*4;
	palette = gdk_rgb_cmap_new(colors, 256);
					// Put things in a vert. box.
	GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
	set_widget(vbox); // This is our "widget"
	gtk_widget_show(vbox);
	
	GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

					// A frame looks nice.
	GtkWidget *frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
	gtk_widget_show(frame);
	gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 0);
					// NOTE:  draw is in Shape_draw.
					// Indicate the events we want.
	gtk_widget_set_events(draw, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK
		| GDK_POINTER_MOTION_HINT_MASK |
		GDK_BUTTON1_MOTION_MASK);
					// Set "configure" handler.
	gtk_signal_connect(GTK_OBJECT(draw), "configure_event",
				GTK_SIGNAL_FUNC(configure), this);
					// Set "expose" handler.
	gtk_signal_connect(GTK_OBJECT(draw), "expose_event",
				GTK_SIGNAL_FUNC(expose), this);
					// Set mouse click handler.
	gtk_signal_connect(GTK_OBJECT(draw), "button_press_event",
				GTK_SIGNAL_FUNC(mouse_press), this);
					// Mouse motion.
	gtk_signal_connect(GTK_OBJECT(draw), "drag_begin",
				GTK_SIGNAL_FUNC(drag_begin), this);
//	gtk_signal_connect(GTK_OBJECT(draw), "motion_notify_event",
//				GTK_SIGNAL_FUNC(Mouse_drag_motion), this);
	gtk_signal_connect (GTK_OBJECT(draw), "drag_data_get",
				GTK_SIGNAL_FUNC(drag_data_get), this);
	gtk_signal_connect (GTK_OBJECT(draw), "selection_clear_event",
				GTK_SIGNAL_FUNC(selection_clear), this);
	gtk_container_add (GTK_CONTAINER (frame), draw);
	gtk_drawing_area_size(GTK_DRAWING_AREA(draw), w, h);
	gtk_widget_show(draw);
					// Want a scrollbar for the shapes.
	GtkObject *shape_adj = gtk_adjustment_new(0, 0, 
				num_shapes, 1, 
				4, 1.0);
	shape_scroll = gtk_vscrollbar_new(GTK_ADJUSTMENT(shape_adj));
					// Update window when it stops.
	gtk_range_set_update_policy(GTK_RANGE(shape_scroll),
					GTK_UPDATE_DELAYED);
	gtk_box_pack_start(GTK_BOX(hbox), shape_scroll, FALSE, TRUE, 0);
					// Set scrollbar handler.
	gtk_signal_connect(GTK_OBJECT(shape_adj), "value_changed",
					GTK_SIGNAL_FUNC(scrolled), this);
	gtk_widget_show(shape_scroll);
					// At the bottom, status bar & frame:
	GtkWidget *hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
	gtk_widget_show(hbox1);
					// At left, a status bar.
	sbar = gtk_statusbar_new();
	sbar_sel = gtk_statusbar_get_context_id(GTK_STATUSBAR(sbar),
							"selection");
	gtk_box_pack_start(GTK_BOX(hbox1), sbar, TRUE, TRUE, 0);
	gtk_widget_show(sbar);
	GtkWidget *label = gtk_label_new("Frame:");
	gtk_box_pack_start(GTK_BOX(hbox1), label, FALSE, FALSE, 4);
	gtk_widget_show(label);
					// Finally, a spin button for frame#.
	frame_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 
				16, 1, 
				4, 1.0));
	fspin = gtk_spin_button_new(GTK_ADJUSTMENT(frame_adj), 
									1, 0);
	gtk_signal_connect(GTK_OBJECT(frame_adj), "value_changed",
					GTK_SIGNAL_FUNC(frame_changed), this);
	gtk_box_pack_start(GTK_BOX(hbox1), fspin, FALSE, FALSE, 0);
	gtk_widget_show(fspin);

					// Add search controls to bottom.
	gtk_box_pack_start(GTK_BOX(vbox), create_search_controls(),
						FALSE, FALSE, 0);
	}

/*
 *	Delete.
 */

Shape_chooser::~Shape_chooser
	(
	)
	{
	gtk_widget_destroy(get_widget());
	delete [] info;
	}
	
/*
 *	Unselect.
 */

void Shape_chooser::unselect
	(
	bool need_render			// 1 to render and show.
	)
	{
	if (selected >= 0)
		{
		selected = -1;
					// Update spin button for frame #.
		gtk_adjustment_set_value(frame_adj, 0);
		gtk_widget_set_sensitive(fspin, false);
		gtk_drag_source_unset(draw);
		if (need_render)
			{
			render();
			show();
			}
		if (sel_changed)	// Tell client.
			(*sel_changed)();
		}
	char buf[150];			// Show new selection.
	if (info_cnt > 0)
		{
//		gtk_statusbar_pop(GTK_STATUSBAR(sbar), sbar_sel);
		g_snprintf(buf, sizeof(buf), "Shapes %d to %d",
			info[0].shapenum, info[info_cnt - 1].shapenum);
		gtk_statusbar_push(GTK_STATUSBAR(sbar), sbar_sel, buf);
		}
	}


