## GEMSGUI tools


### data-json-generator

*data-json-generator* the CLI utility to convert `Resources/data/vis_cn.ini`, `Resources/data/vis_od.ini` and
`Resources/data/units.ini` text I/O files from the old key-value arrays format into the new JSON format used in GEM-Selektor.

Application for developers to easily insert changes into the objects list or forms.

```sh
./data-json-generator <path_to_Resources> 
```

### gems3helpconfig-qhp-generator

*gems3helpconfig-qhp-generator* the CLI utility to generate [Qt Help Project File](https://doc.qt.io/qt-6/qthelpproject.html) is used to generate the GEM-Selektor help system.
A Qt help project collects all data necessary to generate a compressed help file. Along with the actual help data, like the table of contents, index keywords and help documents.

Application for developers to generate `gems3helpconfig.qhp` if new HTML files or objects were added.

```sh
./gems3helpconfig-qhp-generator <path_to_Resources> 
```


