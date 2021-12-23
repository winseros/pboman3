# Pbo.json

You can put the `pbo.json` file into the PBO content root directory when you pack a PBO via the PBO Manager. The PBO Manager reads the PBO headers and the compression rules from the `pbo.json`.

An example `pbo.json`:

```json
{
    "headers": [{
        "name": "prefix",
        "value": "my-addon"
    }, {
        "name": "version",
        "value": "1.0.0"
    }, {
        "name": "product",
        "value": "my-mod"
    }],
    "compress": {
        "include": ["\.txt$", "\.sqf$", "\.ext"],
        "exclude": ["^description.ext$"]
    }
}
```

## Pbo.json fields

### headers

The collection of the headers the PBO file should have.

Type: `array`

Required: `no`

Default: `[]`

#### headers[].name

The name of the header.

Type: `string`

Required: `yes`

#### headers[].value

The value of the header.

Type: `string`

Required: `no`

Default: `""`

### compress

The rules of compressing the contents of the PBO file.

#### compress.include

Which files to compress. 

Type: `array` of [regular expressions](https://en.wikipedia.org/wiki/Regular_expression). (The regular expressions will run against the **relative** file names with **forward slashes** as separators, e.g. `scripts/script1.sqf`)

Required: `no`

#### compress.exclude

Which of the **included** files to exclude from compression. Use this to _"include all then exclude a couple"_ scenario.

Type: `array` of [regular expressions](https://en.wikipedia.org/wiki/Regular_expression)

Required: `no`


## Regular expression debugging

There are lots of the services for regular expression debugging, such as https://regex101.com

## Regular expression examples

| What it will match                                | Example files                                                      | RegEx                  |
| ------------------------------------------------- | ------------------------------------------------------------------ | ---------------------- |
| All the `.sqf` files.                             | `my-script.sqf` or `Scripts/script.sqf`                            | `\.sqf$`               |
| All the `.sqf` files in the `Scripts` root foler. | `Scripts/Client/Alpha/fn_run.sqf`                                  | `^scripts\/.+\.sqf$`   |
| All the `.sqf` files in the `Alpha` subfolders.   | `Scripts/Client/Alpha/initClient.sqf` or `Scripts/Server/Alpha/initServer.sqf` | `\/.+\/Alpha\/.+\.sqf$` |
| The `decription.ext` file in the root.            | `description.ext`                                                  | `^description.ext$`    |
