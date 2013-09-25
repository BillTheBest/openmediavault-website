/**
 * Copyright (C) 2011 Ian Moore 
 * Copyright (C) 2012 Marcel Beck
 * Copyright (C) 2013 OpenMediaVault Plugin Developers
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// require("js/omv/WorkspaceManager.js")

Ext.define("OMV.module.admin.service.website.PhpIni", {
    extend       : "OMV.workspace.form.Panel",
    
    rpcService   : "Website",
    rpcGetMethod : "getPhpIni",
    rpcSetMethod : "setPhpIni",
    
    layout       : 'fit',
    
    getFormItems : function() {
        return [{
            xtype      : 'textarea',
            name       : 'phpini',
            allowBlank : false
        }];
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "manage",
    path      : "/service/website",
    text      : _("php.ini"),
    position  : 20,
    className : "OMV.module.admin.service.website.PhpIni"
});
