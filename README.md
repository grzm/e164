# E.164 Telephone Number Type

WARNING: This is an *ALPHA* release. Do *not* run this software in production.

E.164 is a recommendation by the International Telecommunication Union (ITU) for
international telephone numbers. The E164 type provides a convenient method of
storing international telephone numbers consistent with the E.164 recommendation.
In particular,
  * Country code validation
  * Rudimentary format checking
    - Number consists of proper + prefix followed by digits.
    - Minimum and maximum length checking for corresponding E.164 Type

The E164 type does not check that the number is consistent with formats specific
to particular national standards: formats vary by country. (Support for national
format checking may be added in a future release.)

## TODO

* Add ENUM output (and possibly input) functions
* Properly handle error conditions.
* Fix issues raising compiler warnings:
e164.c: In function 'e164_in':
e164.c:146: warning: 'theNumber' may be used uninitialized in this function
e164.c:156: warning: control reaches end of non-void function
e164_base.c: In function 'e164In':
e164_base.c:553: warning: 'theNumber' may be used uninitialized in this function
e164_base.c:561: warning: control reaches end of non-void function
* Fix all other FIXMEs in code :)

## Copyright and License
Copyright (c) 2007-2011, Michael Glaesemann
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
