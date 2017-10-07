This directory hosts proof-of-concept codes accompanying script gadgets research.

Authors:

  * Sebastian Lekies <slekies@google.com>
  * Eduardo Vela Nava <evn@google.com>
  * Krzysztof Kotowicz <koto@google.com>

Presentations / papers:

*  "Don’t trust the DOM: Bypassing XSS mitigations via Script gadgets" ([AppSec EU 2017 slides](http://sebastian-lekies.de/slides/appsec2017.pdf)) [![AppSec EU 2017 video](https://img.youtube.com/vi/p07acPBi-qw/0.jpg)](https://www.youtube.com/watch?v=p07acPBi-qw)
* ([Black Hat USA 2017 slides](Breaking_XSS_mitigations_via_Script_Gadgets_BHUSA.pdf)).
* [ACM CCS'17 paper](ccs_gadgets.pdf) - with Samuel Groß and Martin Johns.

In order to use the code, you'll need a HTTP(S) server with PHP support. We used Apache2 + mod_php. Respective virtual hosts used in the PoCs are:

  * victim.example.com
  * attacker.example.com

Feel free to replace those hostnames according to your setup with grep. Please set up both virtual hosts as serving the same directory, with the root of the site pointing to `script-gadgets` directory.

The proof-of-concepts are organized by the XSS mitigation bypassed, so e.g. `/repo/csp/sd` will host all Content Security Policy (CSP) `strict-dynamic` bypasses. In each directory
each of the `*-exploit.*` files contains bypass using a given framework or library, so `/repo/csp/ue/aurelia_exploit.php` demonstrates how aurelia framework may bypass the `unsafe-eval` CSP.

The full list of bypasses is in [bypasses.md](bypasses.md).

Some of the payloads require additional modules e.g. [ModSecurity](https://modsecurity.org/). You will need TLS certificates for some of the payloads - try [LetsEncrypt](https://letsencrypt.org/).
