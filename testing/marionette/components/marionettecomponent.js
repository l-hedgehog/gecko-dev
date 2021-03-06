/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const {classes: Cc, interfaces: Ci, utils: Cu} = Components;

const MARIONETTE_CONTRACTID = "@mozilla.org/marionette;1";
const MARIONETTE_CID = Components.ID("{786a1369-dca5-4adc-8486-33d23c88010a}");

Cu.import("resource://gre/modules/XPCOMUtils.jsm");
Cu.import("resource://gre/modules/Services.jsm");
Cu.import("resource://gre/modules/FileUtils.jsm");
Cu.import("resource://gre/modules/services-common/log4moz.js");

function MarionetteComponent() {
  this._loaded = false;
  // set up the logger
  this.logger = Log4Moz.repository.getLogger("Marionette");
}

MarionetteComponent.prototype = {
  classDescription: "Marionette component",
  classID: MARIONETTE_CID,
  contractID: MARIONETTE_CONTRACTID,
  QueryInterface: XPCOMUtils.generateQI([Ci.nsIObserver]),
  _xpcom_categories: [{category: "profile-after-change", service: true}],

  observe: function mc_observe(aSubject, aTopic, aData) {
    let observerService = Services.obs;
    switch (aTopic) {
      case "profile-after-change":
        if (Services.prefs.getBoolPref('marionette.defaultPrefs.enabled')) {

          this.logger.level = Log4Moz.Level["All"];
          let logf = FileUtils.getFile('ProfD', ['marionette.log']);
          
          let formatter = new Log4Moz.BasicFormatter();
          this.logger.addAppender(new Log4Moz.FileAppender(logf, formatter));
          this.logger.info("MarionetteComponent loaded");

          //add observers
          observerService.addObserver(this, "final-ui-startup", false);
          observerService.addObserver(this, "xpcom-shutdown", false);
        }
        else {
          this.logger.info("marionette not enabled");
        }
        break;
      case "final-ui-startup":
        observerService.removeObserver(this, "final-ui-startup");
        this.init();
        break;
      case "xpcom-shutdown":
        observerService.removeObserver(this, "xpcom-shutdown");
        this.uninit();
        break;
    }
  },

  init: function mc_init() {
    if (!this._loaded) {
      this._loaded = true;
      let port;
      try {
        port = Services.prefs.getIntPref('marionette.defaultPrefs.port');
      }
      catch(e) {
        port = 2828;
      }
      try {
        Cu.import('resource:///modules/devtools/dbg-server.jsm');
        DebuggerServer.addActors('chrome://marionette/content/marionette-actors.js');
        // This pref is required for the remote debugger to open a socket,
        // so force it to true.  See bug 761252.
        Services.prefs.setBoolPref('devtools.debugger.remote-enabled', true);
        // Always allow remote connections.
        DebuggerServer.initTransport(function () { return true; });
        DebuggerServer.openListener(port, true);
      }
      catch(e) {
        this.logger.error('exception: ' + e.name + ', ' + e.message);
      }
    }
  },

  uninit: function mc_uninit() {
    DebuggerServer.closeListener();
    this._loaded = false;
  },

};

const NSGetFactory = XPCOMUtils.generateNSGetFactory([MarionetteComponent]);
