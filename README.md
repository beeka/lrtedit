# lrtedit
Book module template viewer and editor for Adobe Lightroom 6

The book module in Adobe Lightroom comes with a defined set of page layout 
templates with no way of modifying them. The layouts are defined in lua
structures in plain text, making it possible to add your own custom ones.

This tool is bourne of a desire to tweak some existing layouts and create
a couple of new ones. The aim of the first iterations is for an assisted 
text editor, with aids in creating a custom page layout and visualising
the templates. The end goal would be WYSIWYG editing of layouts.

## How to use
First create a custom page on which to base the edited one on. Right-click on
a page in your book and select "Save as Custom Page". The page will appear
in the "Custom Pages" style (e.g. like Wedding or Clean) in the "Modify Page"
layout list. Currently only automatic manipulation of existing photo elements 
is supported so expect text blocks to break. You will have to restart lightroom 
and re-apply the page layout to each affected page for the changes to take 
effect.

The current implementation is very hard-coded and will load the custom pages 
for the first book / page type it finds and will automatically tidy the elements 
to snap to grid / align / apply spacing. Also note that any padding on any of 
the elements is left untouched. Previews of the tidied pages are generated 
with green highlights, so they can be spotted as 'special' in Lightroom.

## Limitations
This tool was made for normalising the layouts of a book made by combining 
several styles (mostly Wedding + Clean). There is lots it doesn't deal with 
(such as text). Hopefully someone else will find this tool useful "if only it did 
X", and then add the X.

The custom modifications must be made for each book / page type, although 
the tool currently only picks the first one.

Auto-applies all the fixes on startup. Saving is a separate step, after checking
the results of the fixes, but ideally the edits would be triggered manually.

Settings are hard-coded... there is a settings form but is not wired up to the 
algorithm and is not persisted. 

Would be nice to add/remove photos on a layout with GUI control / live 
preview.

Being able to export / import settings (e.g. after disaster recovery or sharing 
presets) has not been considered.
