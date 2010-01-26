#!/usr/bin/python

import zipfile
import xml.dom.minidom
import os
import csv
import sys


def read_list( file ):
    langs = dict();

    file_hd = open( file )
    reader = csv.reader( file_hd )
    reader.next( ) # Skip the headers line
    for row in reader:
        locale = row[0]
        if locale == "":
            locale = 'all'
        values = []
        if locale in langs:
            values = langs[locale]
        values.append( ( row[1], unicode( row[2], "utf-8" ) ) )
        langs[locale] = values
    return langs

def extract_items( data, locale ):
    entries = []
    if 'all' in data:
        entries = entries + data['all']
    if locale in data:
        entries = entries + data[locale]
    return entries

def update_zip( file, items ):
    tmpname = file + ".new"

    # First open the dat to extract the existing Documentlist.xml file
    inFile = zipfile.ZipFile( file, "r" )
    content = inFile.read( "DocumentList.xml" )
   
    xmlDoc = xml.dom.minidom.parseString( content )

    # Actually add the elements in the XML file
    for pair in items:
        node = xmlDoc.createElement( "block-list:block" )
        node.setAttribute( "block-list:abbreviated-name", pair[0] )
        node.setAttribute( "block-list:name", pair[1] )
        xmlDoc.documentElement.appendChild( node )
    
    # Open the dat file for writing now
    outFile = zipfile.ZipFile( tmpname, "w" )
    data = xmlDoc.toxml().encode( "utf-8" )
    for name in inFile.namelist( ):
        if name != "DocumentList.xml":
            outFile.writestr( name, inFile.read( name ) )
    
    outFile.writestr( "DocumentList.xml", data )
    inFile.close()
    outFile.close()
    
    # Rename the new file
    os.rename( tmpname, file )

def main():
    if ( len( sys.argv ) == 3 ):
        builddir = sys.argv[1]
        datafile = sys.argv[2]
    else:
        print "arguments: build-dir data-file.csv"
        sys.exit( 1 )

    data = read_list( datafile )
    for root, dirs, files in os.walk( os.path.join( builddir, "extras", "source", "autotext", "lang" ) ):
        for f in files:
            if ( f.endswith( ".dat" ) ):
                locale = os.path.basename( root )
                items = extract_items( data, locale )
                if len( items ) > 0:
                    print "Updating: " + os.path.join( root, f )
                    update_zip( os.path.join( root, f ), items )

if __name__ == '__main__':
    main( )
