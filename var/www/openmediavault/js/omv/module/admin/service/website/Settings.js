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
// require("js/omv/workspace/form/Panel.js")
// require("js/omv/form/plugin/LinkedFields.js")
// require("js/omv/form/field/SharedFolderComboBox.js")

Ext.define("OMV.module.admin.service.website.Settings", {
    extend       : "OMV.workspace.form.Panel",

    rpcService   : "Website",
    rpcGetMethod : "getSettings",
    rpcSetMethod : "setSettings",

    plugins : [{
        ptype        : "linkedfields",
        correlations : [{
            name: [
                "sharedfolderref"
            ],
            conditions : [{
                name  : "enable",
                value : true
            }],
            properties : [
                "!allowBlank",
                "!allowNone"
            ]
        },{
            name: [
                "port"
            ],
            conditions : [{
                name  : "vhosttype",
                value : "name"
            }],
            properties : [
                "readOnly",
                "hide"
            ]
        },{
            name       : [
                "hostname"
            ],
            conditions : [{
                name  : "vhosttype",
                value : "port"
            }],
            properties : [
                "readOnly",
                "hide"
            ]
        },{
            name : [
                "sslcertificateref"
            ],
            conditions : [{
                name  : "enablessl",
                value : true
            }],
            properties : [
                "!allowBlank",
                "!allowNone",
                "!readOnly"
            ]
        },{
            name : [
                "cgiuser"
            ],
            conditions : [{
                name  : "ExecCGI",
                value : true
            }],
            properties : [
                "!allowBlank",
                "!allowNone",
                "!readOnly"
            ]
        }]
    }],

    getFormItems : function() {
        return [{
            xtype    : "fieldset",
            title    : _("General settings"),
            defaults : {
                labelSeparator : ""
            },
            items : [{
                xtype      : "checkbox",
                name       : "enable",
                fieldLabel : _("Enable"),
                checked    : false,
            },{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref",
                fieldLabel : _("Document root"),
                allowNone  : true,
                plugins       : [{
                    ptype : "fieldinfo",
                    text  : _("The location of the files to share. Permissions must be at least Users: read-only.")
                }]
            },{
                xtype      : "combo",
                name       : "vhosttype",
                fieldLabel : _("Virtual host Type"),
                allowBlank : true,
                editable   : false,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("In order to create a website that does not conflict with OpenMediaVault's web interface, this plugin will create a virtual host (a virtual website) in OpenMediaVault's web server.") + "<br /><br />" +
                            _("In a <b>port based</b> virtual host configuration, the website configured here will be accessible at the same IP / name of this OpenMediaVault server, but on a different port. E.g. http://some-host:8500/. When using this type of configuration, be sure that the website port configured here is not the same port on which OpenMediaVault is running. This can be checked / changed in System -> General Settings.<Br /><br>In a <b>name based</b> virtual host configuration, the website configured here will be accessed as a different host name. E.g. http://some-host/. Where the name 'some-host' must resolve to the same IP address as OpenMediaVault. This usually requires that you have a DNS server on your network, or that a public DNS server has an entry for <b>some-host</b>.") +
                            "<br /><br />",
                }],
                triggerAction : "all",
                queryMode     : "local",
                store         : Ext.create("Ext.data.SimpleStore", {
                    fields : [
                        "value",
                        "text"
                    ],
                    data   : [
                        [ "port", _("Port based") ],
                        [ "name", _("Name based") ]
                    ]
                }),
                displayField : "text",
                valueField   : "value",
                value        : "port",
            },{
                xtype         : "numberfield",
                name          : "port",
                fieldLabel    : _("Virtual host port"),
                width         : 60,
                value         : 8181,
                vtype         : "port",
                minValue      : 0,
                maxValue      : 65535,
                allowDecimals : false,
                allowNegative : false,
                allowBlank    : true
            },{
                xtype      : "textfield",
                fieldLabel : _("Virtual host hostname"),
                name       : "hostname",
                vtype      : "domainname",
                allowBlank : true
            }]
        },{
            xtype : "fieldset",
            title : _("Secure connection"),
            items : [{
                xtype      : "checkbox",
                name       : "enablessl",
                fieldLabel : _("Enable SSL/TLS"),
                checked    : false,
                boxLabel   : _("Enable secure connection."),
            },{
                xtype      : "certificatecombo",
                name       : "sslcertificateref",
                hiddenName : "sslcertificateref",
                fieldLabel : _("Certificate"),
                allowNone  : true,
                allowBlank : true,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("The SSL certificate.")
                }]
            }]

        },{
            xtype    : "fieldset",
            title    : _("PHP"),
            defaults : {
                xtype : "checkbox"
            },
            items : [{
                name       : "ExecCGI",
                fieldLabel : _("Enable"),
                boxLabel   : _("Allow PHP scripts to be executed."),
            },{
                xtype         : 'usercombo',
                name          : 'cgiuser',
                fieldLabel    : _("User"),
                userType      : "normal",
                allowBlank    : true,
                allowNone     : true,
                editable      : false,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("For security reasons, PHP scripts will run as the selected user.")
                }]
            }]
        },{
            xtype    : "fieldset",
            title    : _("Options"),
            defaults : {
                xtype : "checkbox"
            },
            items : [{
                name       : "Indexes",
                fieldLabel : _("Indexes"),
                boxLabel   : _("If a URL which maps to a directory is requested, and there is no DirectoryIndex (e.g., index.html) in that directory, then a formatted listing of the directory will be returned.")
            },{
                name       : "FollowSymLinks",
                fieldLabel : _("Follow SymLinks"),
                boxLabel   : _("The server will follow symbolic links in the document root.")
            },{
                name       : "SymLinksIfOwnerMatch",
                fieldLabel : _("SymLinks If Owner Match"),
                boxLabel   : _("The server will only follow symbolic links for which the target file or directory is owned by the same user id as the link.")
            },{
                name       : "Includes",
                fieldLabel : _("Includes"),
                boxLabel   : _("Server-side includes are permitted.")
            },{
                name       : "MultiViews",
                fieldLabel : _("MultiViews"),
                boxLabel   : _("Content negotiated \"MultiViews\" are allowed.")
            }]
        },{
            xtype    : "fieldset",
            title    : _("Allow Override"),
            defaults : {
                xtype : "checkbox"
            },
            items : [{
                xtype : "panel",
                html  : _("Allow .htaccess files to override the items selected below.")
            },{
                name       : "AuthConfig",
                fieldLabel : _("Auth Config"),
                boxLabel   : _("Authorization directives (AuthDBMGroupFile, AuthDBMUserFile, AuthGroupFile, AuthName, AuthType, AuthUserFile, Require, etc.).")
            },{
                name       : "OverrideIndexes",
                fieldLabel : _("Indexes"),
                boxLabel   : _("Directives controlling directory indexing (AddDescription, AddIcon, AddIconByEncoding, AddIconByType, DefaultIcon, DirectoryIndex, FancyIndexing, HeaderName, IndexIgnore, IndexOptions, ReadmeName, etc.).")
            },{
                name       : "Limit",
                fieldLabel : _("Limit"),
                boxLabel   : _("Directives controlling host access (Allow, Deny and Order).")
            },{
                name       : "Options",
                fieldLabel : _("Options"),
                boxLabel   : _("Directives controlling specific directory features (Options and XBitHack).")
            }]
        },{
            xtype    : "fieldset",
            title    : _("Extra options"),
            defaults : {
                labelSeparator : ""
            },
            items : [{
                xtype      : "textarea",
                name       : "extraoptions",
                fieldLabel : _("Extra options"),
                allowBlank : true,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("Extra options for the &lt;VirtualHost&gt; directive.")
                }]
            }]
        }];
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "configure",
    path      : "/service/website",
    text      : _("Settings"),
    position  : 10,
    className : "OMV.module.admin.service.website.Settings"
});
