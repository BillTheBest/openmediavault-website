<?php

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

require_once ("openmediavault/object.inc");
require_once ("openmediavault/error.inc");
require_once ("openmediavault/util.inc");
require_once ("openmediavault/rpc.inc");
require_once ("openmediavault/notify.inc");

class OMVRpcServiceWebsite extends OMVRpcServiceAbstract
{
    /**
     * Get the RPC service name
     */
    public function getName()
    {
        return "Website";
    }

    /**
     * Initialize the RPC service
     */
    public function initialize()
    {
        $this->registerMethod("getSettings");
        $this->registerMethod("setSettings");
        $this->registerMethod("getPhpIni");
        $this->registerMethod("setPhpIni");
    }
    
    /**
     * Get general settings object
     * 
     * @throws E_CONFIG_GET_OBJECT_FAILED
     */
    public function getSettings($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Get configuration object
        $xpath = "//services/website";
        $object = $xmlConfig->get($xpath);
        
        if (is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, 
                $xpath);

        // Modify result object
        $object['enable']               = boolval($object['enable']);
        $object['enablessl']            = boolval($object['enablessl']);
        $object['ExecCGI']              = boolval($object['ExecCGI']);
        $object['Indexes']              = boolval($object['Indexes']);
        $object['FollowSymLinks']       = boolval($object['FollowSymLinks']);
        $object['SymLinksIfOwnerMatch'] = boolval($object['SymLinksIfOwnerMatch']);
        $object['Includes']             = boolval($object['Includes']);
        $object['MultiViews']           = boolval($object['MultiViews']);
        $object['AuthConfig']           = boolval($object['AuthConfig']);
        $object['OverrideIndexes']      = boolval($object['OverrideIndexes']);
        $object['Limit']                = boolval($object['Limit']);
        $object['Options']              = boolval($object['Options']);
        
        return $object;
    }

    /**
     * Set general settings object
     * 
     * @throws E_CONFIG_GET_OBJECT_FAILED
     * @throws E_CONFIG_SET_OBJECT_FAILED
     */
    public function setSettings($params, $context)
    {
        global $xmlConfig;
        
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));
        
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type":"object",
            "properties":{
                "enable":{"type":"boolean"},
                "sharedfolderref":{"type":"string","optional":true},
                "vhosttype":{"type":"string","enum":["port","name"]},
                "port":{"type":"integer","optional":true},
                "hostname":{"type":"string","optional":true},
                "enablessl":{"type":"boolean"},
                "sslcertificateref":{"type":"string","optional":true},
                "cgiuser":{"type":"string","optional":true},
                "extraoptions":{"type":"string","optional":true},
                "ExecCGI":{"type":"boolean"},
                "Indexes":{"type":"boolean"},
                "FollowSymLinks":{"type":"boolean"},
                "SymLinksIfOwnerMatch":{"type":"boolean"},
                "Includes":{"type":"boolean"},
                "MultiViews":{"type":"boolean"},
                "AuthConfig":{"type":"boolean"},
                "OverrideIndexes":{"type":"boolean"},
                "Limit":{"type":"boolean"},
                "Options":{"type":"boolean"}
            }
        }');
        
        // Get existing configuration object
        $xpath = "//services/website";
        $object = $xmlConfig->get($xpath);
        
        if (is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);
                
        // Prepare configuration data
        $object = array(
            "enable"                => array_boolval($params, 'enable'),
            "sharedfolderref"       => $params['sharedfolderref'],
            "vhosttype"             => $params['vhosttype'],
            "port"                  => $params['port'],
            "hostname"              => $params['hostname'],
            "enablessl"             => array_boolval($params, 'enablessl'),
            "sslcertificateref"     => $params['sslcertificateref'],
            "cgiuser"               => $params['cgiuser'],
            "ExecCGI"               => array_boolval($params, 'ExecCGI'),
            "Indexes"               => array_boolval($params, 'Indexes'),
            "FollowSymLinks"        => array_boolval($params, 'FollowSymLinks'),
            "SymLinksIfOwnerMatch"  => array_boolval($params, 'SymLinksIfOwnerMatch'),
            "Includes"              => array_boolval($params, 'Includes'),
            "MultiViews"            => array_boolval($params, 'MultiViews'),
            "AuthConfig"            => array_boolval($params, 'AuthConfig'),
            "OverrideIndexes"       => array_boolval($params, 'OverrideIndexes'),
            "Limit"                 => array_boolval($params, 'Limit'),
            "Options"               => array_boolval($params, 'Options'),
            "extraoptions"          => $params['extraoptions']
        );
        
        if ($object['enable'] && $object['vhosttype'] == 'port') {
            $webconf = $xmlConfig->get("//webadmin");
            $ports      = array();
            
            if (empty($webconf['forcesslonly']))
                $ports[] = $webconf['port'];
            if (!empty($webconf['enablessl']))
                $ports[] = $webconf['sslport'];
            
            foreach ($ports as $port) {
                if (intval($port) == intval($object['port']))
                    throw new OMVException('Port ' . $object['port'] . ' conflicts with OpenMediaVault\'s web GUI port.');
            }
        }
        
        // Set configuration object
        if ($xmlConfig->replace($xpath, $object) === false)
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);
        
        // Notify configuration changes
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY, "org.openmediavault.services.website", $object);
        
        return $object;
    }

    /**
     * Get php.ini file
     * 
     * @throws E_EXEC_FAILED
     */
    public function getPhpIni($params, $context)
    {
        global $xmlConfig;
        
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));
        
        $cmd = "cat /var/www/openmediavault-website/php.ini 2>&1";
        
        $result = $this->exec( $cmd, $output);
        $output = implode("\n", $output);
        
        if ($result !== 0)
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, $output);
        
        $object = new stdClass();
        $object->phpini = $output;
        
        return $object;
    }

    /**
     * Set php.ini file
     * 
     * @throws E_EXEC_FAILED
     */
    public function setPhpIni($params, $context)
    {
        global $xmlConfig;
        
        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));
        
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type" : "object",
            "properties" : {
                "phpini" : {
                    "type" :"string"
                }
            }
        }');

        $tmpFile = tempnam('/tmp', '_phpini');
        file_put_contents($tmpFile, $params['phpini']);

        $cmd = "cp $tmpFile /var/www/openmediavault-website/php.ini 2>&1";
        
        $result = $this->exec($cmd, $output);
        unlink($tmpFile);
        
        if ($result !== 0)
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, implode("\n", $output));

        // Notify configuration changes
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY, "org.openmediavault.services.website", $object);
        
        return $object;
    }

}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceWebsite());
