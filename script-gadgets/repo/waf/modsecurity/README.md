To simplify testing vectors against ModSecurity only the /waf/modsecurity directory has it enabled.

Relevant section of the virtual host configuration in Apache:

```
        <Directory /path/to/script-gadgets/repo/>
        AllowOverride all
        # disable mod security
        SecRuleEngine Off
        </Directory>

        <Directory /path/to/script-gadgets/repo/waf/modsecurity>
        # enable mod security
        SecRuleEngine On
        </Directory>
```

Additionally, create a `repo` symlink in this directory pointing to `script-gadgets/repo` directory.
