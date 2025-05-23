###  New JSON config  
To generate new JSON config files from old *.ini input files use tool into  ```../../tools directory```

```
./tool <path to Resources directory>

```


### Visor Connector ini-file format description

[Section]	-	Module name
size xxx yyy    -       default width (xxx) and height (yyy) of window
<Page>		-	Page name
'#' and ';'	-	Comments

Format of DOD field description string:

DODLabel FieldType  nPos  par1	par2	abcd

FieldType:
   F_NUM	- Numeric with nPos visible characters
   F_FLOAT	- Float or double with nPos visible characters
   F_STRING	- One-line text with nPos visible characters
   F_TEXT	- Multi-line text with nPos visible character width
   F_CHECKBOX   - Button whose caption is one of the character sets
   		   defined in units.ini (nPos - name of this set)
   F_REF	- F_STRING + F2 - bibliography
   F_RECORD	- F_STRING + F7 - open record module

To implement:
   F_HTML	- Hypertext browser field

par1		- number of checkboxes (F_CHECKBOX); 
			1 in other field types
par2		- -1 for all field types (default); 
                  positive integer means max number of lines 
                  (or columns) after which the scrollbars appear   

Setup of data field:
a	- DOD label display mode {+-}
b	- data field relative position {*rdnbtsu}
c	- view-edit permissions {+-?}
d	- type {iowh}

Values in setup positions:
a:
 - 	- DOD label off 
 + 	- DOD label on screenform

b:
 *	- 1st field in a screen form
 r 	- to the right from previous field
 d 	- down (under the tallest element of prev.field)
 n 	- next (right or below from new-line)
 b 	- begin a new line (skip line)
 t 	- tied from right to previous field (right + tied)
 s 	- sticked from below to previous field (down + tied)
 u 	- under (right under the last element of prev. field)

c:  view-edit permissions
 + 	- view and edit
 - 	- view only
 ? 	- view only + parameter (edit in Remake mode)

d:  color highlighting of fields (reserved)
i	- input data
o	- output data
w	- work, intermediate or debug values
h 	- help information


Hot keys used in editing F_TEXT fields 
Ctr/C    copy text
Ctr/V    paste 
Ctr/X    cut
Ctr/K    kill line
Ctr/A    home
Ctr/B    left
Ctr/D    del
Ctr/E    end
Ctr/F    right
Ctr/H    backspace
Ctr/N    down
Ctr/P    up
