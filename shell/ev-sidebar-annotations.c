/* ev-sidebar-annotations.c
 *  this file is part of xreader, a generic document viewer
 *
 * Copyright (C) 2010 Carlos Garcia Campos  <carlosgc@gnome.org>
 *
 * Xreader is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Xreader is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "config.h"

#include <glib/gi18n.h>

#include "ev-document-annotations.h"
#include "ev-sidebar-page.h"
#include "ev-sidebar-annotations.h"
#include "ev-annotations-toolbar.h"
#include "ev-jobs.h"
#include "ev-job-scheduler.h"
#include "ev-stock-icons.h"

enum {
	PROP_0,
	PROP_WIDGET
};

enum {
	COLUMN_MARKUP,
	COLUMN_ICON,
	COLUMN_ANNOT_MAPPING,
	N_COLUMNS
};

enum {
	ANNOT_ACTIVATED,
	N_SIGNALS
};

struct _EvSidebarAnnotationsPrivate {
	EvDocument  *document;

	GtkWidget *tree_view;
	GtkWidget *toolbar;

	EvJob *job;
	guint selection_changed_id;
};

static void ev_sidebar_annotations_page_iface_init (EvSidebarPageInterface *iface);
static void ev_sidebar_annotations_load            (EvSidebarAnnotations   *sidebar_annots);

static guint signals[N_SIGNALS] = { 0 };

G_DEFINE_TYPE_EXTENDED (EvSidebarAnnotations,
                        ev_sidebar_annotations,
                        GTK_TYPE_BOX,
                        0,
                        G_ADD_PRIVATE (EvSidebarAnnotations)
                        G_IMPLEMENT_INTERFACE (EV_TYPE_SIDEBAR_PAGE,
					       ev_sidebar_annotations_page_iface_init))

static void
ev_sidebar_annotations_dispose (GObject *object)
{
	EvSidebarAnnotations *sidebar_annots = EV_SIDEBAR_ANNOTATIONS (object);
	EvSidebarAnnotationsPrivate *priv = sidebar_annots->priv;

	if (priv->document) {
		g_object_unref (priv->document);
		priv->document = NULL;
	}

	G_OBJECT_CLASS (ev_sidebar_annotations_parent_class)->dispose (object);
}

static GtkTreeModel *
ev_sidebar_annotations_create_simple_model (const gchar *message)
{
	GtkTreeModel *retval;
	GtkTreeIter iter;
	gchar *markup;

	/* Creates a fake model to indicate that we're loading */
	retval = (GtkTreeModel *)gtk_list_store_new (N_COLUMNS,
						     G_TYPE_STRING,
						     GDK_TYPE_PIXBUF,
						     G_TYPE_POINTER);

	gtk_list_store_append (GTK_LIST_STORE (retval), &iter);
	markup = g_strdup_printf ("<span size=\"larger\" style=\"italic\">%s</span>",
				  message);
	gtk_list_store_set (GTK_LIST_STORE (retval), &iter,
			    COLUMN_MARKUP, markup,
			    -1);
	g_free (markup);

	return retval;
}


static void
ev_sidebar_annotations_init (EvSidebarAnnotations *ev_annots)
{
	GtkWidget *swindow;
	GtkTreeModel *loading_model;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *selection;
	GtkWidget *separator;
	GtkWidget *toolbar;
	GtkWidget *toolitem;
	GtkWidget *hbox;

	ev_annots->priv = ev_sidebar_annotations_get_instance_private (ev_annots);

	gtk_orientable_set_orientation (GTK_ORIENTABLE (ev_annots), GTK_ORIENTATION_VERTICAL);

	swindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start (GTK_BOX (ev_annots), swindow, TRUE, TRUE, 0);
	gtk_widget_show (swindow);

	/* Create tree view */
	loading_model = ev_sidebar_annotations_create_simple_model (_("Loading…"));
	ev_annots->priv->tree_view = gtk_tree_view_new_with_model (loading_model);
	g_object_unref (loading_model);

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (ev_annots->priv->tree_view), FALSE);
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (ev_annots->priv->tree_view));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_NONE);

	column = gtk_tree_view_column_new ();

	renderer = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_column_pack_start (column, renderer, FALSE);
	gtk_tree_view_column_set_attributes (column, renderer,
					     "pixbuf", COLUMN_ICON,
					     NULL);

	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	gtk_tree_view_column_set_attributes (column, renderer,
					     "markup", COLUMN_MARKUP,
					     NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (ev_annots->priv->tree_view), column);

	gtk_container_add (GTK_CONTAINER (swindow), ev_annots->priv->tree_view);
	gtk_widget_show (ev_annots->priv->tree_view);

	separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start (GTK_BOX (ev_annots), separator, FALSE, FALSE, 0);
	gtk_widget_show (separator);

	toolbar = gtk_toolbar_new ();
	gtk_widget_show (toolbar);

	toolitem = GTK_WIDGET (gtk_tool_item_new ());
	gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (toolitem), 0);
	gtk_widget_show (toolitem);

	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add (GTK_CONTAINER (toolitem), hbox);
	gtk_widget_show (hbox);

	/* Create annotation tool bar */
	ev_annots->priv->toolbar = ev_annotations_toolbar_new();
	gtk_widget_show (ev_annots->priv->toolbar);

	gtk_box_pack_end (GTK_BOX (ev_annots), ev_annots->priv->toolbar, FALSE, TRUE, 0);
	gtk_widget_show (GTK_WIDGET (ev_annots));
}

static void
ev_sidebar_annotations_get_property (GObject    *object,
				     guint       prop_id,
				     GValue     *value,
				     GParamSpec *pspec)
{
	EvSidebarAnnotations *ev_sidebar_annots;

	ev_sidebar_annots = EV_SIDEBAR_ANNOTATIONS (object);

	switch (prop_id) {
	        case PROP_WIDGET:
			g_value_set_object (value, ev_sidebar_annots->priv->toolbar);
			break;
	        default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
ev_sidebar_annotations_class_init (EvSidebarAnnotationsClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	g_object_class->get_property = ev_sidebar_annotations_get_property;
	g_object_class->dispose = ev_sidebar_annotations_dispose;

	g_object_class_override_property (g_object_class, PROP_WIDGET, "main-widget");

	signals[ANNOT_ACTIVATED] =
		g_signal_new ("annot-activated",
			      G_TYPE_FROM_CLASS (g_object_class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (EvSidebarAnnotationsClass, annot_activated),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__POINTER,
			      G_TYPE_NONE, 1,
			      G_TYPE_POINTER);
}

GtkWidget *
ev_sidebar_annotations_new (void)
{
	return GTK_WIDGET (g_object_new (EV_TYPE_SIDEBAR_ANNOTATIONS, NULL));
}

void
ev_sidebar_annotations_annot_added (EvSidebarAnnotations *sidebar_annots,
				    EvAnnotation         *annot)
{
	ev_sidebar_annotations_load (sidebar_annots);
}

void
ev_sidebar_annotations_annot_removed (EvSidebarAnnotations *sidebar_annots,
				    EvAnnotation         *annot)
{
	ev_sidebar_annotations_load (sidebar_annots);
}

EvAnnotationsToolbar *
ev_sidebar_annotations_get_toolbar(EvSidebarAnnotations *sidebar_annots)
{
	EvSidebarAnnotationsPrivate *priv = ev_sidebar_annotations_get_instance_private(sidebar_annots);
	return EV_ANNOTATIONS_TOOLBAR(priv->toolbar);
}


static void
selection_changed_cb (GtkTreeSelection     *selection,
		      EvSidebarAnnotations *sidebar_annots)
{
	GtkTreeModel *model;
	GtkTreeIter   iter;

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		EvMapping *mapping = NULL;

		gtk_tree_model_get (model, &iter,
				    COLUMN_ANNOT_MAPPING, &mapping,
				    -1);
		if (mapping)
			g_signal_emit (sidebar_annots, signals[ANNOT_ACTIVATED], 0, mapping);
	}
}

static void
job_finished_callback (EvJobAnnots          *job,
		       EvSidebarAnnotations *sidebar_annots)
{
	EvSidebarAnnotationsPrivate *priv;
	GtkTreeStore *model;
	GtkTreeSelection *selection;
	GList *l;
	GtkIconTheme *icon_theme;
	GdkScreen *screen;
	GdkPixbuf *text_icon = NULL;
	GdkPixbuf *attachment_icon = NULL;
        GdkPixbuf *highlight_icon = NULL;
        GdkPixbuf *strike_out_icon = NULL;
        GdkPixbuf *underline_icon = NULL;
        GdkPixbuf *squiggly_icon = NULL;

	priv = sidebar_annots->priv;

	if (!job->annots) {
		GtkTreeModel *list;

		list = ev_sidebar_annotations_create_simple_model (_("Document contains no annotations"));
		gtk_tree_view_set_model (GTK_TREE_VIEW (priv->tree_view), list);
		g_object_unref (list);

		g_object_unref (job);
		priv->job = NULL;

		return;
	}

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->tree_view));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	if (priv->selection_changed_id == 0) {
		priv->selection_changed_id =
			g_signal_connect (selection, "changed",
					  G_CALLBACK (selection_changed_cb),
					  sidebar_annots);
	}

	model = gtk_tree_store_new (N_COLUMNS,
				    G_TYPE_STRING,
				    GDK_TYPE_PIXBUF,
				    G_TYPE_POINTER);

	for (l = job->annots; l; l = g_list_next (l)) {
		EvMappingList *mapping_list;
		GList         *ll;
		gchar         *page_label;
		GtkTreeIter    iter;
		gboolean       found = FALSE;

		mapping_list = (EvMappingList *)l->data;
		page_label = g_strdup_printf (_("Page %d"),
					      ev_mapping_list_get_page (mapping_list) + 1);
		gtk_tree_store_append (model, &iter, NULL);
		gtk_tree_store_set (model, &iter,
				    COLUMN_MARKUP, page_label,
				    -1);
		g_free (page_label);

		screen = gdk_screen_get_default ();
		icon_theme = gtk_icon_theme_get_for_screen (screen);

		for (ll = ev_mapping_list_get_list (mapping_list); ll; ll = g_list_next (ll)) {
			EvAnnotation *annot;
			const gchar  *label;
			const gchar  *modified;
			gchar        *markup;
			GtkTreeIter   child_iter;
			GdkPixbuf    *pixbuf = NULL;

			annot = ((EvMapping *)(ll->data))->data;
			if (!EV_IS_ANNOTATION_MARKUP (annot))
				continue;

			label = ev_annotation_markup_get_label (EV_ANNOTATION_MARKUP (annot));
			modified = ev_annotation_get_modified (annot);
			if (modified) {
				markup = g_strdup_printf ("<span weight=\"bold\">%s</span>\n%s",
							  label, modified);
			} else {
				markup = g_strdup_printf ("<span weight=\"bold\">%s</span>", label);
			}

			if (EV_IS_ANNOTATION_TEXT (annot)) {
				if (!text_icon) {
					/* FIXME: use a better icon than EDIT */
					text_icon = gtk_icon_theme_load_icon (icon_theme,
														  "starred",
														  16,
														  GTK_ICON_LOOKUP_FORCE_REGULAR,
														  NULL);
				}
				pixbuf = text_icon;
			} else if (EV_IS_ANNOTATION_ATTACHMENT (annot)) {
				if (!attachment_icon) {
					attachment_icon = gtk_icon_theme_load_icon (icon_theme,
																"mail-attachment",
																16,
																GTK_ICON_LOOKUP_FORCE_REGULAR,
																NULL);
				}
				pixbuf = attachment_icon;
			} else if (EV_IS_ANNOTATION_TEXT_MARKUP (annot)) {
                                switch (ev_annotation_text_markup_get_markup_type (EV_ANNOTATION_TEXT_MARKUP (annot))) {
                                case EV_ANNOTATION_TEXT_MARKUP_HIGHLIGHT:
                                        if (!highlight_icon) {
                                                /* FIXME: use better icon than select all */
                                                highlight_icon = gtk_widget_render_icon_pixbuf (priv->tree_view,
                                                                                                GTK_STOCK_SELECT_ALL,
                                                                                                GTK_ICON_SIZE_BUTTON);
                                        }
                                        pixbuf = highlight_icon;

                                        break;
                                case EV_ANNOTATION_TEXT_MARKUP_STRIKE_OUT:
                                        if (!strike_out_icon) {
                                                strike_out_icon = gtk_widget_render_icon_pixbuf (priv->tree_view,
                                                                                                 GTK_STOCK_STRIKETHROUGH,
                                                                                                 GTK_ICON_SIZE_BUTTON);
                                        }
                                        pixbuf = strike_out_icon;
                                        break;
                                case EV_ANNOTATION_TEXT_MARKUP_UNDERLINE:
                                        if (!underline_icon) {
                                                underline_icon = gtk_widget_render_icon_pixbuf (priv->tree_view,
                                                                                                GTK_STOCK_UNDERLINE,
                                                                                                GTK_ICON_SIZE_BUTTON);
                                        }
                                        pixbuf = underline_icon;
                                        break;
                                case EV_ANNOTATION_TEXT_MARKUP_SQUIGGLY:
                                        if (!squiggly_icon) {
                                                squiggly_icon = gtk_widget_render_icon_pixbuf (priv->tree_view,
                                                                                               GTK_STOCK_UNDERLINE,
                                                                                               GTK_ICON_SIZE_BUTTON);
                                        }
                                        pixbuf = squiggly_icon;
                                        break;
                                }
                        }

			gtk_tree_store_append (model, &child_iter, &iter);
			gtk_tree_store_set (model, &child_iter,
					    COLUMN_MARKUP, markup,
					    COLUMN_ICON, pixbuf,
					    COLUMN_ANNOT_MAPPING, ll->data,
					    -1);
			g_free (markup);
			found = TRUE;
		}

		if (!found)
			gtk_tree_store_remove (model, &iter);
	}

	gtk_tree_view_set_model (GTK_TREE_VIEW (priv->tree_view),
				 GTK_TREE_MODEL (model));
	g_object_unref (model);

	if (text_icon)
		g_object_unref (text_icon);
	if (attachment_icon)
		g_object_unref (attachment_icon);
        if (highlight_icon)
                g_object_unref (highlight_icon);
        if (strike_out_icon)
                g_object_unref (strike_out_icon);
        if (underline_icon)
                g_object_unref (underline_icon);
        if (squiggly_icon)
                g_object_unref (squiggly_icon);

	g_object_unref (job);
	priv->job = NULL;
}

static void
ev_sidebar_annotations_load (EvSidebarAnnotations *sidebar_annots)
{
	EvSidebarAnnotationsPrivate *priv = sidebar_annots->priv;

	if (priv->job) {
		g_signal_handlers_disconnect_by_func (priv->job,
						      job_finished_callback,
						      sidebar_annots);
		g_object_unref (priv->job);
	}

	priv->job = ev_job_annots_new (priv->document);
	g_signal_connect (priv->job, "finished",
			  G_CALLBACK (job_finished_callback),
			  sidebar_annots);
	/* The priority doesn't matter for this job */
	ev_job_scheduler_push_job (priv->job, EV_JOB_PRIORITY_NONE);
}

static void
ev_sidebar_annotations_document_changed_cb (EvDocumentModel      *model,
					    GParamSpec           *pspec,
					    EvSidebarAnnotations *sidebar_annots)
{
	EvDocument *document = ev_document_model_get_document (model);
	EvSidebarAnnotationsPrivate *priv = sidebar_annots->priv;

	if (!EV_IS_DOCUMENT_ANNOTATIONS (document))
		return;

	if (priv->document)
		g_object_unref (priv->document);
	priv->document = g_object_ref (document);

	ev_document_annotations_can_add_annotation (EV_DOCUMENT_ANNOTATIONS (document));

	ev_sidebar_annotations_load (sidebar_annots);
}

/* EvSidebarPageIface */
static void
ev_sidebar_annotations_set_model (EvSidebarPage   *sidebar_page,
				  EvDocumentModel *model)
{
	g_signal_connect (model, "notify::document",
			  G_CALLBACK (ev_sidebar_annotations_document_changed_cb),
			  sidebar_page);
}

static gboolean
ev_sidebar_annotations_support_document (EvSidebarPage *sidebar_page,
					 EvDocument    *document)
{
	return (EV_IS_DOCUMENT_ANNOTATIONS (document));
}

static const gchar *
ev_sidebar_annotations_get_label (EvSidebarPage *sidebar_page)
{
	return _("Annotations");
}

static void
ev_sidebar_annotations_page_iface_init (EvSidebarPageInterface *iface)
{
	iface->support_document = ev_sidebar_annotations_support_document;
	iface->set_model = ev_sidebar_annotations_set_model;
	iface->get_label = ev_sidebar_annotations_get_label;
}
