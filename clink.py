#!/usr/bin/env python
#
# clink: compacts folders by replacing identical files by symbolic links
#
# Usage: clink <files or directories>
#
# Version 1.1.1
# Copyright (C) 2005-2006 Free Electrons
# Author: Michael Opdenacker <michael@free-electrons.com>
# Home page: http://free-electrons.com/community/tools/scripts/clink
# 
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
# 
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
# NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# You should have received a copy of the  GNU General Public License along
# with this program; if not, write  to the Free Software Foundation, Inc.,
# 675 Mass Ave, Cambridge, MA 02139, USA.

import sys, os, md5, sha, shutil, optparse

##########################################################
# Common routines
##########################################################

def common_dir(files):

    # Returns the path of the first parent directory
    # containing all the files in the files list. 
    # 
    # All the file paths must be absolute and normalized!
 
    return os.path.dirname(os.path.commonprefix(files))

def path_to_file(src_file, dest_file):

    # Returns the path from the parent directory of src_file
    # to dest_file, suitable for creating a relative symbolic link
    #
    # Both file paths must be absolute!

    common_parent = common_dir([src_file, dest_file])
  
    # Relative path between dest_file and the common parent
    path_to_common_parent = '' 
    current_parent = os.path.dirname(dest_file)

    while current_parent != common_parent: 
	path_to_common_parent = os.path.join('..', path_to_common_parent)
        current_parent = os.path.dirname(current_parent)

    # Relative path of src in the common parent
    rel_src_file = src_file[len(common_parent)+1:]

    return os.path.join(path_to_common_parent, rel_src_file)

def checksums(file):

    # Returns a tuple containing the sha and md5 checksums
    # of the given file
    #
    # The file has to be existing and readable!

    fid = open(file_path, 'r')
    sh = sha.new()
    m5 = md5.new()

    # Iterates on each line instead of calling fid.read()
    # which could eat up the whole RAM with big files

    line = fid.readline()

    while (line != ''):    
          sh.update(line)
          m5.update(line)
	  line = fid.readline()

    fid.close()

    return sh.hexdigest(), m5.hexdigest()

##########################################################
# Main program
##########################################################

# Define and check command line arguments

usage = 'usage: %prog [options] [files or directories]'
description = 'Compacts folders by replacing identical files by symbolic links'

optparser = optparse.OptionParser(usage=usage, version='clink 1.1', description=description)

optparser.add_option('-d', '--dry-run',
                     action='store_true', dest='dryrun', default=False,
                     help='just reports identical files, doesn\'t make any change.')
		     
(options, args) = optparser.parse_args()

if len(args) == 0:
   print 'No files to check. Exiting.'
   sys.exit()

# Check that arguments correspond to existing files or directories

for file in args:
    if not os.path.lexists(file):
       print 'ERROR (link_compress): no such ', file, ' file or directory'   
       sys.exit()

# Inits

md5sums = dict()
shasums = dict()
removed_size = 0
total_size = 0

# Compute and store the checksums of all the files

for arg in args:
    # Turn each path into an absolute one and loop
    for root, dirs, files in os.walk(os.path.abspath(arg)):
    	for name in files:

    	    file_path = os.path.join(root, name)

    	    # Ignore symlinks
            # and other non regular files (named pipes, device files...)

    	    if (not os.path.islink(file_path)) and os.path.isfile(file_path):

	       shsum, m5sum = checksums(file_path)
	       total_size += os.path.getsize(file_path)

	       # Store all the files in a list dict,
 	       # indexed by shasum

    	       if not shasums.has_key(shsum):
    		  shasums[shsum] = []
    	       shasums[shsum].append(file_path)

	       # Store the md5sum for each file

    	       md5sums[file_path] = m5sum
   
# Replace all the files with the same checksum by symbolic links

for key in shasums.keys():

    files = shasums[key]

    # Ignore sha checksums corresponding to only 1 file

    if len(files) > 1: 

       # Security check: check that md5 checksums and sizes are also identical

       md5s = set()
       sizes = set()
       
       for file in files:
           md5s.add(md5sums[file])
	   sizes.add(os.path.getsize(file))

       if len(md5s) == 1 and len(sizes) == 1:
       
          size = sizes.pop()
	  removed_size += size * (len(files) -1)
	  
	  if options.dryrun:
	     print 'Identical files (' + str(size) + ' bytes each):'
             for file in files:
                 print '   ', file
	     
	  else:

	     # Going to replace files by links
	     # First find the common parent directory

	     root = common_dir(files)

 	     # Find a name and path for the common file
	     # Make sure we don't override an existing one!
	     # CAUTION: could fail it it found an existing "common-files" file (not being a directory)

 	     target_file_name = os.path.basename(files[0])
	     target_file_dir = os.path.join(root, 'common-files')
 	     target_file_path = os.path.join(target_file_dir, target_file_name)
 	     i = 1

	     while os.path.lexists(target_file_path):
	     	   i += 1
	     	   target_file_path = os.path.join(target_file_dir, target_file_name + '.' + str(i))

 	     # Create the target directory

 	     if not os.path.lexists(target_file_dir):
	     	os.makedirs(target_file_dir)

 	     # Move the first file and replace it with a link
 	     #
	     # shutil.move doesn't copy (much more efficient) when the
 	     # src and dest are on the same filesystem

	     shutil.move(files[0], target_file_path)
	     os.symlink(path_to_file(target_file_path, files[0]), files[0])

	     # Replace the other identical files by links

	     for file in files[1:]:
 		 os.remove(file)
		 os.symlink(path_to_file(target_file_path, file), file)

if removed_size ==0:
   print 'Found no duplicate files. No changes to make.'
else:
   if options.dryrun:
      cond = 'could have '
   else:
      cond = ''
   print 'Done: ' + cond + 'replaced', removed_size, 'bytes of files by symbolic links,'
   print 'representing', (removed_size * 100 ) / total_size, '% of the initial total file size.'
   print 'This may represent even more savings in disk blocks (du command output)'
