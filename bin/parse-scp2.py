#!/usr/bin/env python

import sys, os, os.path, optparse, subprocess

arg_desc = ""

desc = """
Run this script at the root of OOo source tree."""

def error (msg):
    sys.stderr.write(msg + "\n")

class ErrorBase(Exception):
    
    def __init__ (self, name, msg, sev):
        self.value = "%s: %s"%(name, msg)
        self.sev = sev                    # error severity, 0 = least severe

    def __str__ (self):
        return repr(self.value)

class ParseError(ErrorBase):
    
    def __init__ (self, msg, sev = 0):
        ErrorBase.__init__(self, "ParseError", msg, sev)

# Parse each .scp file.
class Scp2Parser(object):

    class Type:
        File       = 0
        Directory  = 1
        FolderItem = 2

    NodeTypes = [
        'DataCarrier',
        'Directory', 
        'File', 
        'Folder',
        'FolderItem', 
        'Installation', 
        'Module',
        'Profile',
        'ProfileItem',
        'RegistryItem', 
        'ScpAction',
        'Shortcut',
        'StarRegistry',
        'Unixlink',
        'WindowsCustomAction'
    ]

    def __init__ (self, content, filename):
        self.content = content
        self.filename = filename
        self.nodes = {}

    def tokenize (self):
        self.tokens = []
        i = 0
        n = len(self.content)
        token = ''
        while i < n:
            c = self.content[i]
            if c in '\t\n':
                c = ' '

            if c in ' ;':
                if len(token) > 0:
                    self.tokens.append(token)
                    token = ''
                if c == ';':
                    self.tokens.append(c)
            else:
                token += c
            i += 1

    def parse (self):
        if len(self.tokens) == 0:
            # No tokens to parse.  Bail out.
            return
            
        self.i = 0
        self.n = len(self.tokens)
        while self.i < self.n:
            t = self.token()
            if t in Scp2Parser.NodeTypes:
                name, attrs, values = self.__parseEntity()
                attrs['__node_type__'] = t                 # type of node
                attrs['__node_location__'] = self.filename # file where the node is defined
                attrs['__node_values__'] = values
                if self.nodes.has_key(name):
                    raise ParseError("node named %s already exists"%name, 1)
                self.nodes[name] = attrs
            else:
                raise ParseError("Unknown node type: %s"%t)

            self.next()

    def append_nodes (self, nodes):
        for key in self.nodes.keys():
            if nodes.has_key(key):
                raise ParseError("node named %s already exists"%key, 1)
            nodes[key] = self.nodes[key]

    def next (self):
        self.i += 1

    def token (self):
        return self.tokens[self.i]

    def __parseEntity (self):
        self.next()
        name = self.token()
        left = True
        attr_name = ''
        attr_value = ''
        attrs = {}
        values = []
        self.next()
        while self.token() != 'End':
            if self.token() == '=':
                if not left:
                    raise ParseError("multiple '='s in a single line")

                if len(attr_name) == 0:
                    raise ParseError("empty attribute name")

                left = False
            
            elif left:
                if self.token() == ';':
                    # Not a valid attribute.  Ignore this for now.
                    values.append(attr_name)
                    attr_name = ''
                else:
                    attr_name += self.token()
            else:
                # Parse all the way up to ';'
                attr_value = ''
                while self.token() != ';':
                    attr_value += self.token()
                    self.next()
                attrs[attr_name] = attr_value
                left = True
                attr_name = ''

            self.next()

        return name, attrs, values



# Collect all .scp files in scp2 directory, and run preprocessor.
class Scp2Processor(object):

    tmpin  = "/tmp/parse-scp2.py.cpp"
    tmpout = "/tmp/parse-scp2.py.out"

    SkipList = {
        'scp2/source/ooo/ure_standalone.scp': True,
        'scp2/source/sdkoo/sdkoo.scp': True,
        'scp2/source/ooo/starregistry_ooo.scp': True
    }

    def __init__ (self, cur_dir, mod_output_dir):
        self.cur_dir = cur_dir
        self.mod_output_dir = mod_output_dir
        self.scp_files = []
        self.nodes = {}

        # Check file paths first.
        if not os.path.isfile("%s/scp2/inc/macros.inc"%self.cur_dir):
            raise ParseError("You don't appear to be at the root of OOo's source tree.")
        if not os.path.isdir("%s/scp2/%s/inc"%(self.cur_dir, self.mod_output_dir)):
            raise ParseError("You don't appear to be at the root of OOo's source tree.")

    def to_relative (self, fullpath):
        i = fullpath.find("/scp2/")
        if i < 0:
            return fullpath
        i += 1 # skip '/' before 'scp2'
        return fullpath[i:]

    def run (self):
        # Collect all .scp files under scp2.
        os.path.walk(self.cur_dir + "/scp2", Scp2Processor.visit, self)

        # Process each .scp file.
        for scp in self.scp_files:
            relpath = self.to_relative(scp)
            if Scp2Processor.SkipList.has_key(relpath):
                error("skipping %s"%scp)
                continue

            self.process_scp(scp)

    def process_scp (self, scp):
        ret = subprocess.call("cp %s %s"%(scp, Scp2Processor.tmpin), shell=True)
        if ret > 0:
            raise ParseError("failed to copy scp file to a temporary location.")

        subprocess.call("gcc -E -I./scp2/inc -I%s -DUNX %s 2>/dev/null | grep -v -E \"^\#\" > %s"%
            (self.mod_output_dir, Scp2Processor.tmpin, Scp2Processor.tmpout), shell=True)

        file = open(Scp2Processor.tmpout, 'r')
        content = file.read()
        file.close()
        parser = Scp2Parser(content, self.to_relative(scp))
        parser.tokenize()
        try:
            parser.parse()
            parser.append_nodes(self.nodes)
        except ParseError as e:
            # Skip mal-formed files, instead of exit with error.
            error (e.value)
            error ("Error parsing %s"%scp)
            if e.sev > 0:
                # This is a severe error.  Exit right away.
                sys.exit(1)

    def print_summary (self):
        names = self.nodes.keys()
        names.sort()
        for name in names:
            attrs = self.nodes[name]
            node_type = attrs['__node_type__']
            print ('-'*70)
            print ("%s (%s)"%(name, node_type))
            print ("[node location: %s]"%attrs['__node_location__'])

            # Print values first.
            values = attrs['__node_values__']
            for value in values:
                print("  %s"%value)

            # Print all attributes.
            attr_names = attrs.keys()
            attr_names.sort()
            for attr_name in attr_names:
                if attr_name in ['__node_type__', '__node_location__', '__node_values__']:
                    # Skip special attribute.
                    continue
                print ("  %s = %s"%(attr_name, attrs[attr_name]))

    @staticmethod
    def visit (arg, dirname, names):
        instance = arg
        for name in names:
            filepath = dirname + "/" + name
            if os.path.splitext(filepath)[1] == '.scp':
                instance.scp_files.append(filepath)


if __name__ == '__main__':

    parser = optparse.OptionParser()
    parser.usage += " " + arg_desc + "\n" + desc
    parser.add_option("", "--module-output-dir", dest="mod_output_dir", default="unxlngi6.pro", metavar="DIR",
        help="Specify the name of module output directory.  The default value is 'unxlngi6.pro'.")

    options, args = parser.parse_args()
    cur_dir = os.getcwd()
    try:
        processor = Scp2Processor(cur_dir, options.mod_output_dir)
        processor.run()
        processor.print_summary()
    except ParseError as e:
        print (e.value)
