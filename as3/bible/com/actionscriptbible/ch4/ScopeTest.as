// The following code is example code from 
// "ActionScript 3.0 Bible, 2nd Edition" by Roger Braunstein
// Find all of the source code on the companion website
// http://actionscriptbible.com/
//
// Copyright (c) 2010 by Wiley Publishing, Inc., Indianapolis, Indiana
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

package com.actionscriptbible.ch4 {
  import com.actionscriptbible.Example;
  public class ScopeTest extends Example {
    private var a:String = "apricot";
    public function ScopeTest() {
      testA(); testB();
    }
    private function testA():void {
      var a:String = "acid";
      trace(a); //acid;
//      trace(b); //ERROR: Access of undefined property b
      trace(this.a); //apricot
    }
    private function testB():void {
      var b:String = "banana pancakes";
      trace(a); //apricot
      trace(b); //banana pancakes
      this.finish();
    }
    private function finish():void {
      trace("done!");
    }
  }
}