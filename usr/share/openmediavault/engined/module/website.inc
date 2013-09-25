<?php

/**
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

require_once ("openmediavault/module.inc");
require_once ("openmediavault/config.inc");
require_once ("openmediavault/error.inc");
require_once ("openmediavault/util.inc");
require_once ("openmediavault/initscript.inc");

class OMVModuleWebsite extends OMVModuleServiceAbstract implements OMVINotifyListener
{

    /**
     * Get the module name.
     */
    public function getName()
    {
        return "website";
    }

    /**
    * Generate the configuration.
    * 
    * @throws E_EXEC_FAILED
    */ 
    public function applyConfig()
    {
        // Currently transmission overwrites the settings.json configuration
        // file on exit. This means transmission must be terminated before
        // writing the new settings.json file.        
        $cmd = "omv-mkconf website 2>&1";
        
        if (0 !== $this->exec($cmd, $output))
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED,
                $cmd, implode("\n", $output));
    }

    /**
    * Stop the managed service.
    *
    * @throws E_CONFIG_GET_OBJECT_FAILED
    */ 
    public function stopService()
    {
        global $xmlConfig;
        
        // Get the configuration object
        $xpath = "//services/website";
        $object = $xmlConfig->get($xpath);
        
        if (is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, 
                $xpath);

        $cmd = "sudo a2dissite openmediavault-website";
        $this->exec($cmd);
        
        $cmd = "omv-mkconf apache2";
        $this->exec($cmd);
        
        $initd = new OMVSysVInitScript("apache2", boolval($object['enable']));
        $initd->invoke("graceful");
    }
    
    /**
    * Start the managed service.
    * 
    * @throws E_CONFIG_GET_OBJECT_FAILED
    * @throws E_EXEC_FAILED 
    */ 
    public function startService()
    {
        global $xmlConfig;
        
        // Get the configuration object
        $xpath = "//services/website";
        $object = $xmlConfig->get($xpath);
        
        if (is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, 
                $xpath);
        
        $cmd = "apache2ctl configtest";
        $result = $this->exec($cmd, $output);
        
        if ($result !== 0) {
            $this->stopService();
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, implode("\n", $output));
        }
        
        if (boolval($object['enable']) === true) {
            $initd = new OMVSysVInitScript("apache2", true);
            $initd->invoke("graceful");
        }
    }

    /**
     * Bind listeners.
     */
    function bindListeners(OMVNotifyDispatcher $dispatcher)
    {
        $dispatcher->addListener(
            OMV_NOTIFY_MODIFY,
            "org.openmediavault.services.website",
            array($this, "setDirty"));
    }

}

// Register module
$moduleMgr = &OMVModuleMgr::getInstance();
$moduleMgr->registerModule(new OMVModuleWebsite());
