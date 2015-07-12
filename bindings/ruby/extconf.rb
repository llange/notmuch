#!/usr/bin/env ruby
# coding: utf-8
# Copyright 2010, 2011, 2012 Ali Polatel <alip@exherbo.org>
# Distributed under the terms of the GNU General Public License v3

require 'mkmf'

dir = File.join('..', '..', 'lib')

# includes
$INCFLAGS = "-I#{dir} #{$INCFLAGS}"

# make sure there are no undefined symbols
if CONFIG["arch"] !~ /darwin/
  $LDFLAGS += " -Wl,--no-undefined"
end

def have_local_library(lib, path, func, headers = nil)
  checking_for checking_message(func, lib) do
    lib = File.join(path, lib)
    if try_func(func, lib, headers)
      $LOCAL_LIBS += lib
    end
  end
end

['so', 'dylib', 'dll'].detect(lambda { print "Local library notmuch seems absent\n"; exit }) { |ext|
	have_local_library("libnotmuch.#{ext}", dir, 'notmuch_database_create', 'notmuch.h')
}

if ENV['EXTRA_LDFLAGS']
  $LDFLAGS += " " + ENV['EXTRA_LDFLAGS']
end

if not ENV['LIBNOTMUCH']
  exit 1
end

$LOCAL_LIBS += ENV['LIBNOTMUCH']

# Create Makefile
dir_config('notmuch')
create_makefile('notmuch')
