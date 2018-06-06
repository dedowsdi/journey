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
  public class LimitedEdition {
    private static var editionsMade:int = 0;
    private static const MAX_EDITIONS:int = 20;
    
    private var serialNumber:int;
    
    public function LimitedEdition(serialNumber:int) {
      this.serialNumber = serialNumber;
    }
    
    public static function getOne():LimitedEdition {
      if (editionsMade++ < MAX_EDITIONS) {
        return new LimitedEdition(editionsMade);
      }
      return null;
    }
    
    public function toString():String {
      return "Limited Edition Object #" + serialNumber;
    }
  }
}
