/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details:
 *
 * Copyright (C) 2008 - 2009 Novell, Inc.
 * Copyright (C) 2009 - 2013 Red Hat, Inc.
 * Copyright (C) 2012 Aleksander Morgado <aleksander@gnu.org>
 */

#include <string.h>
#include <gmodule.h>

#define _LIBMM_INSIDE_MM
#include <libmm-glib.h>

#include "mm-port-enums-types.h"
#include "mm-log.h"
#include "mm-modem-helpers.h"
#include "mm-plugin-telit.h"
#include "mm-common-telit.h"
#include "mm-broadband-modem-telit.h"

G_DEFINE_TYPE (MMPluginTelit, mm_plugin_telit, MM_TYPE_PLUGIN)

MM_PLUGIN_DEFINE_MAJOR_VERSION
MM_PLUGIN_DEFINE_MINOR_VERSION

/*****************************************************************************/

static MMBaseModem *
create_modem (MMPlugin *self,
              const gchar *sysfs_path,
              const gchar **drivers,
              guint16 vendor,
              guint16 product,
              GList *probes,
              GError **error)
{
    return MM_BASE_MODEM (mm_broadband_modem_telit_new (sysfs_path,
                                                        drivers,
                                                        mm_plugin_get_name (self),
                                                        vendor,
                                                        product));
}

/*****************************************************************************/

G_MODULE_EXPORT MMPlugin *
mm_plugin_create (void)
{
    static const gchar *subsystems[] = { "tty", NULL };
    /* Vendors: Telit */
    static const guint16 vendor_ids[] = { 0x1bc7, 0 };
    /* Only handle TELIT tagged devices here. */
    static const gchar *udev_tags[] = {
        "ID_MM_TELIT_TAGGED",
        NULL
    };
    /* Custom init for port identification */
    static const MMAsyncMethod custom_init = {
        .async  = G_CALLBACK (telit_custom_init),
        .finish = G_CALLBACK (telit_custom_init_finish),
    };

    return MM_PLUGIN (
        g_object_new (MM_TYPE_PLUGIN_TELIT,
                      MM_PLUGIN_NAME,               "Telit",
                      MM_PLUGIN_ALLOWED_SUBSYSTEMS, subsystems,
                      MM_PLUGIN_ALLOWED_VENDOR_IDS, vendor_ids,
                      MM_PLUGIN_ALLOWED_AT,         TRUE,
                      MM_PLUGIN_ALLOWED_UDEV_TAGS,  udev_tags,
                      MM_PLUGIN_CUSTOM_INIT,        &custom_init,
                      NULL));
}

static void
mm_plugin_telit_init (MMPluginTelit *self)
{
}

static void
mm_plugin_telit_class_init (MMPluginTelitClass *klass)
{
    MMPluginClass *plugin_class = MM_PLUGIN_CLASS (klass);

    plugin_class->create_modem = create_modem;
    plugin_class->grab_port = telit_grab_port;
}