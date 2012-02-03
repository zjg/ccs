#!/usr/bin/perl
# Parameters that this script accepts
# -c "javac options"       javac options
# -j "jar options"         jar options
# -s "source dir1 dir2"    source dirs or files separated by space
# -r "resource dir1 dir2"  resource dirs separated by space
# -o outputfile.jar        output file
# -d "dep1.jar dep2.jar"   dependency jar files
# -h path                  java home dir. It is expected that javac and jar are present in $JAVA_HOME/bin
# -a jar_name              name of the jar binary (default: 'jar') - needed because some systems have 'gjar'

use strict;
use warnings;
use Getopt::Std;
use File::Copy;
#use File::Find;

sub copy_recursively {
  my($from_dir, $to_dir) = @_;
  opendir my($dh), $from_dir or die "Could not open dir '$from_dir': $!";
  for my $entry (readdir $dh) {
    next if $entry eq ".." or $entry eq ".";
    my $source = "$from_dir/$entry";
    my $destination = "$to_dir/$entry";
    if (-d $source) {
      mkdir $destination or die "mkdir '$destination' failed: $!" if not -e $destination;
      copy_recursively($source, $destination);
    } else {
      if ($entry !~ /\.java$/) {
        copy($source, $destination) or die "copy failed: $!";
      }
    }
  }
  closedir $dh;
  return;
}

sub find_java_files {
  my($inputs, $files_ref) = @_;

  for my $in (split ' ', $inputs) {
     if (-f $in) {
        push(@$files_ref, $in);
     }
     elsif (-d $in) {
        opendir my($dh), $in or die "Could not open dir '$in': $!";
        for my $entry (readdir $dh) {
          next if $entry eq ".." or $entry eq ".";
          my $child = "$in/$entry";
          if (-d $child) {
            find_java_files($child, \@$files_ref);
          } else {
            if ($entry =~ /\.java$/) {
              push(@$files_ref, $child);
            }
          }
        }
        closedir $dh;
     }
     else {
        die "Source '$in' is neither file nor directory";
     }
  }
}

sub split_text {
  my $text = $_[0];
  
  $text =~ s/(^\s+|\s+$)//g;
  $text =~ s/\s+/ /g;
  return () if $text eq "";
  return split(' ', $text);
}

our($opt_o, $opt_c, $opt_j, $opt_s, $opt_r, $opt_d, $opt_h, $opt_a);
getopt('cjosrdha');

die "Specify source directories or files with -s" if !$opt_s;
die "Specify output jar file with -o" if !$opt_o;

my ($jar_bin);
unless (!$opt_a) {
   $jar_bin = $opt_a;
} else {
   $jar_bin = 'jar';
}

my ($javac_path, $jar_path);
unless (!$opt_h) {
  $javac_path = $opt_h.'/bin/javac';
  $jar_path = $opt_h.'/bin/'.$jar_bin;
} else {
  # Use binaries from path
  $javac_path = 'javac';
  $jar_path = $jar_bin;
}

my @java_files = ();
foreach my $dir (split_text($opt_s)) {
  find_java_files($dir, \@java_files);
#  find sub { push(@java_files, $File::Find::name) if $File::Find::name =~ /\.java$/ }, $dir;
}

die "No *.java files found" unless @java_files;

my $workdir = "/tmp/tup.javac.$$";

rmdir $workdir if -d $workdir;
mkdir $workdir;


# Run java compiler
my @javac_args = ($javac_path, '-d', $workdir);
push(@javac_args, split_text($opt_c)) unless !$opt_c;
push(@javac_args, '-classpath', join(':', split_text($opt_d))) unless !$opt_d;
push(@javac_args, @java_files);
system(@javac_args) and exit 1;

# Copy resources
unless (!$opt_r) {
  foreach my $dir (split_text($opt_r)) {
    copy_recursively($dir, $workdir);
  }
}

# Jar *.class files
my @jar_args = ($jar_path, 'cf', $opt_o, '-C', $workdir);
push(@jar_args, split_text($opt_j)) unless !$opt_j;
push(@jar_args, '.');
system(@jar_args) and exit 1;

rmdir $workdir;

