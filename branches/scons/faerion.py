"""faerion

Faerion macros for SCons.

"""

#
# Copyright (C) 2005 The Faerion Team
#
# $Id$
# vim:ts=2:sts=2:sw=2:et:nocindent:syntax=python
#

import os
import re
import shutil
import SCons.Defaults

class Faerion:
  # A function to list files in a directory.
  def listFilesW(self, at, r):
    nodes = []
    for location in at:
      for file in os.listdir(location):
        fname = os.path.join(location, file)
        if os.path.isfile(fname):
          if r.match(file):
            nodes.append(fname)
        elif os.path.isdir(fname):
          if not os.path.basename(fname) == '.svn':
            for more in self.listFilesW([fname], r):
              nodes.append(more)
    return nodes

  def listFiles(self, at, r):
    l = self.listFilesW(at, re.compile(r))
    l.sort()
    return l

  def compile(self, env, paths):
    objs = []
    for item in self.listFiles(paths, r'^.*(\.cc)$'):
      fname, ext = os.path.splitext(item)
      if env.has_key('category'):
        fname = fname + '~' + env['category']
      if env['debug'] == 1:
        fname = fname + '~debug'
      objs.append(env.SharedObject(fname + '.o', item))
    return objs

  def makeInstaller(self, xenv, name, fmask):
    env = xenv.Copy(TARFLAGS = '-c -z')

    dir = name
    tar = name + '-src.tar.gz'

    for item in self.listFiles(['.'], fmask):
      f = env.File(item)

      src = f.path
      dst = dir + '/' + os.path.dirname(f.path)
      dsf = dir + '/' + f.path

      if not os.path.exists(dst):
        os.makedirs(dst)

      shutil.copy2(src, dsf)
      env.Tar(tar, dsf)

    env.AddPostAction(tar, SCons.Defaults.Delete(dir))

def Build():
	return Faerion()
