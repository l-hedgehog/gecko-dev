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
    File Name:          15.4.2.3.js
    ECMA Section:       15.4.2.3 new Array()
    Description:        The [[Prototype]] property of the newly constructed
                        object is set to the origianl Array prototype object,
                        the one that is the initial value of Array.prototype.
                        The [[Class]] property of the new object is set to
                        "Array".  The length of the object is set to 0.

    Author:             christine@netscape.com
    Date:               7 october 1997
*/

    var SECTION = "15.4.2.3";
    var VERSION = "ECMA_1";
    startTest();
    var TITLE   = "The Array Constructor:  new Array()";

    writeHeaderToLog( SECTION + " "+ TITLE);

    var testcases = getTestCases();
    test();

function getTestCases() {
    var array = new Array();
    var item = 0;
    array[item++] = new TestCase( SECTION,	"new   Array() +''",        "",                 (new Array()) +"" );
    array[item++] = new TestCase( SECTION,	"typeof new Array()",       "object",           (typeof new Array()) );
    array[item++] = new TestCase(   SECTION,
                                    "var arr = new Array(); arr.getClass = Object.prototype.toString; arr.getClass()",
                                    "[object Array]",
                                    eval("var arr = new Array(); arr.getClass = Object.prototype.toString; arr.getClass()") );

    array[item++] = new TestCase( SECTION,	"(new Array()).length",     0,                  (new Array()).length );
    array[item++] = new TestCase( SECTION,	"(new Array()).toString == Array.prototype.toString",   true,       (new Array()).toString == Array.prototype.toString );
    array[item++] = new TestCase( SECTION,	"(new Array()).join  == Array.prototype.join",          true,       (new Array()).join  == Array.prototype.join );
    array[item++] = new TestCase( SECTION,	"(new Array()).reverse == Array.prototype.reverse",     true,       (new Array()).reverse  == Array.prototype.reverse );
    array[item++] = new TestCase( SECTION,	"(new Array()).sort  == Array.prototype.sort",          true,       (new Array()).sort  == Array.prototype.sort );

    return ( array );
}
function test() {
    for ( tc=0; tc < testcases.length; tc++ ) {
        testcases[tc].passed = writeTestCaseResult(
                            testcases[tc].expect,
                            testcases[tc].actual,
                            testcases[tc].description +" = "+ testcases[tc].actual );

        testcases[tc].reason += ( testcases[tc].passed ) ? "" : "wrong value ";
    }
    stopTest();
    return ( testcases );
}
