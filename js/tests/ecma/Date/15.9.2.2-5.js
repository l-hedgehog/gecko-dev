/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
/**
    File Name:          15.9.2.2.js
    ECMA Section:       15.9.2.2 Date constructor used as a function
                        Date( year, month, date, hours, minutes, seconds )
    Description:        The arguments are accepted, but are completely ignored.
                        A string is created and returned as if by the
                        expression (new Date()).toString().

    Author:             christine@netscape.com
    Date:               28 october 1997
    Version:            9706

*/
    var VERSION = 9706;
    startTest();
    var SECTION = "15.9.2.2";
    var TOLERANCE = 100;
    var TITLE = "The Date Constructor Called as a Function";

    writeHeaderToLog(SECTION+" "+TITLE );
    var tc= 0;
    var testcases = getTestCases();

//  all tests must call a function that returns an array of TestCase objects.
    test();

function getTestCases() {
    var array = new Array();
    var item = 0;

    // Dates around jan 1, 2005
    array[item++] = new TestCase( SECTION, "Date(2004,11,31,23,59,59)",     (new Date()).toString(),    Date(2004,11,31,23,59,59));
    array[item++] = new TestCase( SECTION, "Date(2005,0,1,0,0,0)",          (new Date()).toString(),    Date(2005,0,1,0,0,0) );
    array[item++] = new TestCase( SECTION, "Date(2005,0,1,0,0,1)",          (new Date()).toString(),    Date(2005,0,1,0,0,1) );
    array[item++] = new TestCase( SECTION, "Date(2004,11,31,16,0,0,0)",     (new Date()).toString(),    Date(2004,11,31,16,0,0,0));
/*
    // Dates around jan 1, 2032
    array[item++] = new TestCase( SECTION, "Date(2031,11,31,23,59,59)",     (new Date()).toString(),    Date(2031,11,31,23,59,59));
    array[item++] = new TestCase( SECTION, "Date(2032,0,1,0,0,0)",          (new Date()).toString(),    Date(2032,0,1,0,0,0) );
    array[item++] = new TestCase( SECTION, "Date(2032,0,1,0,0,1)",          (new Date()).toString(),    Date(2032,0,1,0,0,1) );
    array[item++] = new TestCase( SECTION, "Date(2031,11,31,16,0,0,0)",     (new Date()).toString(),    Date(2031,11,31,16,0,0,0));
*/
    return ( array );
}
function test() {
    for ( tc=0; tc < testcases.length; tc++ ) {
        testcases[tc].passed = writeTestCaseResult(
                            testcases[tc].expect,
                            testcases[tc].actual,
                            testcases[tc].description +" = "+
                            testcases[tc].actual );

        testcases[tc].reason += ( testcases[tc].passed ) ? "" : "wrong value ";
    }
    stopTest();
    return ( testcases );
}
