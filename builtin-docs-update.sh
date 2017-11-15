#!/bin/sh

echo "Building built-in user manual..."
if ! xsltproc -stringparam version `./VERSION` -html htm_to_qml.xsl fracplanet.htm | sed 's/"/\\"/g' | sed 's/^/"/g' | sed 's/$/\\n"/g'> usage_text.h ; then echo "Couldn't build usage_text.h" ; fi
if ! test -s usage_text.h ; then echo "\"Full built-in user documentation not available due to problem during build configuration.  Maybe the builder didn't have xsltproc or sed installed ?\"" > usage_text.h ; echo "Something went wrong, used built-in user documentation fallback plan"; fi
echo "...built built-in user documentation"
