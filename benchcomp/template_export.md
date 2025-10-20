## Export to gems3k format files

gems3k-export the Qt console application for export systems from the internal database projects.


## How to execute

```sh 
./gems3k-export -d -s . -p template_export.json -e gems3k
```

# Command line parameters

```sh
Usage: gems3k-export [ option(s) ] -p|--projects-config JSON_FILE -e|--export-path FOLDER 
Export database projects to IPM, DCH and DBR files
Options:
        -h,	--help  		show this help message

        -s,	--system-dir   FOLDER   	path for Resources 
        -u,	--user-dir     FOLDER   	GEMS projects location 

        -d,	--from-ini-files        	remake DOD and module dialog configurators (default false) 
        -c,	--with-default-config   	file configuration if project subfolder(s) were added/removed (default false) 

```


## Export template file

File template_export.json consists of a list of projects and systems to be unloaded, and describes some export rules.


```json

{
     "comment" :   "template file for gems3k export",
     "calc_mode" :   0,
     "io_mode" :   "-j",
     "brief_mode" :   true,
     "with_comments" :   false,
     "projects" :   [
          {
               "key" :   "Kaolinite:Test-JNC:",
               "systems" :   [
                    {
                         "key" :   "Kaolinite:G:pHtitr:0:0:1:25:0:",
                         "calc_mode" :   1,
                         "brief_mode" :   false,
                         "io_mode" :   "-j",
                         "Tai" :   [
                              25,
                              125,
                              10,
                              1
                         ],
                         "Pai" :   [
                              1,
                              1,
                              0,
                              0.5
                         ]
                    },
                    {
                         "key" :   "Kaolinite:G:pHtitrKa:0:0:1:25:0:",
                         "io_mode" :   "-t"
                    }
               ]
          },
          {
               "key" :   "Solvus:test project solvus:",
               "systems" :   [
                    {
                         "key" :   "Solvus:G:series1:0001:0:500:400:0:",
                         "brief_mode" :   false,
                         "Tai" :   [
                              400,
                              700,
                              10,
                              1
                         ],
                         "Pai" :   [
                              500,
                              500,
                              0,
                              500
                         ]
                    },
                    {
                         "key" :   "Solvus:G:series2:0002:0:1000:640:0:"
                    }
               ]
          }
     ]
}

````


Commentas for template_export.json  

1. "calc_mode" flag for calculation of equilibrium state before export. Possible values: 0 - no recalculate system; 1 - NEED_GEM_AIA; 2 - NEED_GEM_SIA.

2. "io_mode" flag to write IPM, DCH, and DBR files in binary, txt, or JSON mode. Possible values: `-j` - json format; '-t' - key-value format; '-f' - json format with thermofun files; '-o' - key-value format with thermofun files. 

3. If not defined, "calc_mode", "io_mode", or "brief_mode" flags use top-level or default values.

4. If not defined, "Tai" and "Pai" use T and P from the current system.

