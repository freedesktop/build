#!/usr/bin/perl -w

use File::Copy;
use File::Basename;

sub find_file($$)
{
    my $dir = shift;
    my $file = shift;

    $file =~ m/\.diff$/ || die "Patch file '$file' is not a .diff";

    if (!-f "$dir/$file") {
	my @search = split /:/, $options{'PATCHPATH'};

	for $stem (@search) {
	    my $testdir = "$dir/$stem";
	    if (-f "$testdir/$file") {
		$dir = $testdir;
		last;
	    }
	}
    }

    -f "$dir/$file" || die "\n\n** Error ** - Can't find file $dir/$file\n\n\n";

    return "$dir/$file";
}

sub do_patch {
    my $patch = shift;
    my $base_cmd = shift;
    my $patch_file = basename($patch);
    my $cmd_output = "";
    my $cmd_suffix = "";

    print "$patch_file: testing..."; 

    if ($quiet) 
        { $cmd_output = " > /dev/null " }

    $cmd_pipe = " < $patch";
    my $cmd = "( $base_cmd --dry-run ".$cmd_output.$cmd_suffix.")".$cmd_pipe;

    print "$cmd\n" unless $quiet;
    system ($cmd) && die "Testing patch $patch_file failed.";

    $cmd = "( $base_cmd > /dev/null".$cmd_suffix.")".$cmd_pipe;
    if($quiet)
        { print "applying..." }
    else
        { print "$cmd\n" }

    system ($cmd) && die "Failed to patch $patch_file\n" .
			 "- this is often caused by duplicate sections in a patch.\n".
			 "you will need to manually reverse this patch\n";
    print "done.\n";
}

sub slurp {
    my $from = shift;
    my $save = $/;
    my $FROM;

    undef $/;
    open ($FROM, "<$from");
    # slurp whole file in one big string
    my $content = <$FROM>;
    close ($FROM);
    $/ = $save;

    return $content;
}

sub select_subset ($$);

sub select_subset ($$)
{
    my $subsets = shift;
    my $tag = shift;
    my @nodes = ();

    if (defined $subsets->{$tag}) {
	@nodes = @{$subsets->{$tag}};
    }
	
    if (!@nodes) {
	return @nodes;
    }

    my $subtag;
    my @result = @nodes;

    my $count = @nodes;
    for $subtag (@nodes) {
	my @subset = select_subset($subsets, $subtag);
	push @result, @subset;
    }

    return @result;
}

sub rules_pass($)
{
    my $original_rule = shift;
    my $rule = $original_rule;

    while ($rule ne '') {
	my $lastrule = $rule;
#	print "verify rule '$rule'\n";
	# less than or equal (<=)
	if ($rule =~ s/\<=\s*(\S+)// && ($tag cmp $1) > 0 ) { return 0; };
	# less than (<)
	if ($rule =~ s/\<\s*(\S+)// && ($tag cmp $1) >= 0 ) { return 0; };
	# greater than or equal (>=)
	if ($rule =~ s/\>=\s*(\S+)// && ($tag cmp $1) < 0 ) { return 0; }; 
	# greater than (>)
	if ($rule =~ s/\>\s*(\S+)// && ($tag cmp $1) <= 0 ) { return 0; }; 

	$rule =~ s/^\s*//;

	$lastrule eq $rule && die "Invalid syntax in rule: $original_rule\n";
    }
    return 1;
}

sub version_filter_targets($)
{
    my $tlist = shift;
    my @targets;

    $tlist =~ m/([^<>]*)(.*)/;
    my $rules = $2;
    my $targets = $1;
    @targets = split /\s*,\s*/, $targets;

#    printf "Rules '$rules' targets '$targets'\n";

    if (!rules_pass ($rules)) {
#	printf "Rule '$rules' failed\n";
	@targets = ();
    } else {
#	printf "Rule '$rules' passed\n";
    }

    return @targets;
}

sub list_patches {

    sub checkTerminal {
	my $subsets = shift;
	my $referenced = shift;
	my $distro = shift;

	if (defined $referenced->{$distro} ||
	    !defined $subsets->{$distro}) {
	    print "Error: you must use a terminal node for distro:\n";
	    my $ref;
	    my @terminals;
	    print "    One of: ";
	    for $ref(keys %{$subsets}) {
		if (!defined $referenced->{$ref}) {
		    push @terminals, $ref;
		}
	    }
	    print join ', ', sort (@terminals);
	    print "\n";
	    exit (1);
	}
    }

    my $forDistro = shift;
    my @Patches = ();

    open (PatchList, "$apply_list") || die "Can't find $apply_list";

    my @targets=();
    my %subsets=();
    my @selected_subsets=();
    my %referenced=();
    while (<PatchList>) {
            s/\s*#.*//;
            chomp;
	    s/\r//; # Win32
	    s/\s*$//;
	    s/^\s*//;
            $_ eq '' && next;

	    # continuation
	    if (defined $lastline) {
		$_ = $lastline . $_;
		undef $lastline;
	    }
	    if (m/\\\S*$/) {
		s/\\//g;
		$lastline = $_;
		next;
	    }

            if (/\[\s*(.*)\]/) {
                my $tmp = $1;
                $tmp =~ s/\s+$//;
#               print "Target: '$tmp'\n"; 
		@targets = version_filter_targets($tmp);
                next;
            }

            if (/\s*([_\S]+)\s*=\s*(.*)/) {
		$options{$1} = $2;
		print "$1 => $2\n" unless $quiet;
		next;
	    }

            if (/\s*([_\S]+)\s*:\s*(.*)/) {
		my $key = $1;
		my @value = (split /\s*,\s*/, $2);
		$subsets{$key} = [@value];
		@selected_subsets = select_subset (\%subsets, $forDistro);
#		print "selected sets: @selected_subsets\n" unless $quiet;
		# update terminality map
		my $ref;
		for $ref(@value) {
		    $referenced{$ref} = 1;
		}
                next;
            }

	    my $set;
	    my $match = 0;
	    for $set (@targets) {
		if (grep /^$set$/i, @selected_subsets) {
		    $match = 1;
		}
	    }
	    if (!$match) {
#		print "@subsets: @targets: skipping '$_'\n";
		next;
	    }

            push @Patches, find_file ($patch_dir, $_);
    }
    close (PatchList);

    checkTerminal (\%subsets, \%referenced, $forDistro);

    return @Patches;
}

sub applied_patches_list
{
    my @patches;
    
    foreach (glob($applied_patches."/???-*")) { 
	m/\~$/ && next; # backup file.
	push @patches, $_;
    }

    return @patches;
}

sub apply_patches {

    my @Patches = list_patches ($distro);

    print "\n" unless $quiet;

    for $opt (@required_opts) { 
        defined $options{$opt} || die "Required option $opt not defined"; 
    }

    if( ! -d $applied_patches ) {
        mkdir $applied_patches || die "Can't make directory $patch_dir: $!";
    }

    my $patch_num = 0;
    my %existing_patches;

    foreach (applied_patches_list()) {
        my $file = basename $_;

        $file =~ s/^([0-9]{3})-//;
        $existing_patches{$file} = $_;
        $patch_num = $1;
    }
    $patch_num++;

# Foo ! - we really need to look for different patches here and
# reverse them ahead of time, not just missing ones [!]

    my %obsolete_patches = %existing_patches;
    foreach $patch (@Patches) {
	delete $obsolete_patches{basename($patch)};
    }

    foreach $a (reverse sort keys %obsolete_patches) {
	$patch = $obsolete_patches{$a};
        print "Unapplying obsolete patch $patch\n";
        do_patch $patch, $base_cmd." -R";
        unlink $patch || die "Can't remove $patch $!";
	delete $existing_patches{$a};
    }

    foreach $patch (@Patches) {
        my $patch_file = basename($patch);
        
        my $is_applied = 0;
        
        if( exists $existing_patches{$patch_file} ) {
            my $applied_patch = $existing_patches{$patch_file};
            my $patch_content = slurp($patch);
            my $applied_content = slurp($applied_patch);
            if (length ($patch_content) == length ($applied_content) &&
                $patch_content eq $applied_content) {
                print "$patch_file already applied, skipped\n";
                $is_applied = 1;
            }
            else {
                print "$patch_file changed, unapplying\n";
                do_patch $applied_patch, $base_cmd." -R";
                unlink $applied_patch || die "Can't remove $applied_patch: $!";
            }
            delete $existing_patches{$patch_file};
        }

        if (!$is_applied) {
            print "\n" unless $quiet;
            do_patch $patch, $base_cmd;

            my $patch_copy = sprintf("%s/%03d-%s", $applied_patches, $patch_num++, $patch_file);

            print "copy $patch_file -> $patch_copy\n" unless $quiet;

            copy($patch, $patch_copy) 
                || die "Can't copy $patch to $patch_copy $!";
        }
    }

    if (keys %existing_patches) {
	die "Error - leftover obsolete patches\n";
    }
}

sub remove_patches {
    my @Patches = ();

    -d $applied_patches || return;

    foreach $patch_file (reverse (applied_patches_list())) {
        print "\nRemoving ".basename($patch_file)."...\n" unless $quiet;
        do_patch $patch_file, $base_cmd;
        unlink $patch_file;
    }
    rmdir $applied_patches
}

sub export_series {
    my @Patches = list_patches ($distro);

    for my $patch (@Patches) {
	$patch =~ s/^\Q$patch_dir\E\/..\///;
	print "$patch -p0\n";
    }
}

sub is_old_patch_version()
{
    my $Patch;
    my $ver_line;
    my $is_old = 1;

    open ($Patch, "patch --version|") || die "Can't run patch: $!";
    $ver_line = <$Patch>;
    $ver_line =~ m/\s+(\d+)\.(\d+)\.(\d+)/ || die "Can't get patch version\n";
    if ($1 >= 2 && $2 >= 5 && $3 >= 9) {
	$is_old = 0;
    }
    
    if ($is_old) {
	print "Old patch version - pruning LFs\n";
    }
    return $is_old;
}

(@ARGV > 1) ||
    die "Syntax:\n".
    "apply <path-to-patchdir> <src root> [--distro=Debian] [patch flags '--dry-run' eg.]\n".
    "apply <path-to-patchdir> --series-to";

%options = ();

$quiet = 0;
$remove = 0;
$export = 0;
$opts = "";
$distro = 'NLD';
$tag = '';
$dry_run = 0;
@required_opts = ( 'PATCHPATH' );
@arguments = ();


foreach $a (@ARGV) {
	if ($a eq '-R') {
	    $remove = 1;	    
	    push @arguments, $a;
	} elsif ($a =~ m/--series-from=(.*)/) {
	    $export = 1;
	    $quiet = 1;
	    $distro = $1;
	} elsif ($a eq '--quiet') {
	    $quiet = 1;
	} elsif ($a =~ m/--distro=(.*)/) {
	    $distro = $1;
	} elsif ($a =~ m/--tag=(.*)/) {
	    $tag = $1;
	} elsif ($a =~ m/--dry-run/g) {
	    $dry_run = 1;
	} else {
	    push @arguments, $a;
	}
}
$patch_dir = shift (@arguments);
substr ($patch_dir, 0, 1) eq '/' || die "apply.pl requires absolute paths";

$apply_list = $patch_dir.'/apply';

print "Execute with $opts for distro '$distro'\n" unless $quiet;

if ($dry_run) {
    my @Patches = list_patches ($distro);
    printf "Exiting before applying patches\n";
    
} elsif ($export) {
    export_series();

} else {
    $dest_dir = shift (@arguments);
    substr ($dest_dir, 0, 1) eq '/' || die "apply.pl requires absolute paths";
    $applied_patches = $dest_dir.'/applied_patches';

    $opts = join ' ', @arguments;
    $base_cmd = "patch -l -b -p0 $opts -d $dest_dir";
    if (is_old_patch_version()) {
	$base_cmd = 'sed \'s/^\(@.*\)\r$/\1/\' | ' . $base_cmd;
    }

    if ($remove) {
	remove_patches();
    } else {
	apply_patches();
    }
}
