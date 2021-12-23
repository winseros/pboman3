# Prefix files

Since the times of Operation Flashpoint, they used so-called [prefix files](https://community.bistudio.com/wiki/PBOPREFIX) to provide the packaging tools the information regarding the headers a packed PBO should have.

To use a prefix file, you create a text file with the name i.e. `$pboprefix$` (or `pboprefix.txt`) and some text. And when the PBO Manager packs the folder, the produced `PBO` will have the header with the name `prefix` and the text from the file.

The PBO Manager supports the following prefix files (names are case-insensitive):

| File name      | Alternative file name | Resulting PBO header |
| -------------- | --------------------- | -------------------- |
| \$pboprefix\$  | pboprefix.txt         | prefix               |
| \$pboproduct\$ | pboproduct.txt        | product              |
| \$pboversion\$ | pboversion.txt        | version              |

However, there is a more efficient way to control file headers: [pbo.json](pbo_json.md).
