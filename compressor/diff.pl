#!/bin/perl

open(FH1, '<', "$ARGV[0]") or die $!;
open(FH2, '<', "$ARGV[1]") or die $!;

while (<FH1>) {
	@f1 = split(",");
	$a = <FH2>;
	@f2 = split(",",$a);
	
	($x,$y,$z,$d) = @f2[1..3,5];

	$r = sqrt($x*$x+$y*$y+$z*$z);
	$x /= $r; $y /= $r; $z /= $r;

	$x -= $f1[1]; $y -= $f1[2]; $z -= $f1[3]; $d -= $f1[5];
	$r = sqrt($x*$x+$y*$y+$z*$z);
	$errmax = $r if ($r > $errmax);
	$erravg += $r;

	$d = abs($d);
	$derrmax = $d if ($d > $derrmax);
	$derravg += $d;
	
	$i++;
#	print join(",",@f1[1..3]),"\n";
#	print join(",",@f2[1..3]),"\n";
#	print 6371000*$r,"\n";
}

$erravg /= $i;
$derravg /= $i;

$erravg *= 6371000;
$errmax *= 6371000;

print "avg pos error: $erravg; max pos error: $errmax\n";
print "avg depth error: $derravg; max depth error: $derrmax\n";

close FH1; close FH2;
