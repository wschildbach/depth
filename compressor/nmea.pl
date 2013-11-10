#!/usr/bin/perl

sub myXOR($)
{
	my $sum = "";
	my $s = shift;
	for (0..length($s)-1) {
		my $a = substr($s,$_,1);
		$sum = "$sum" ^ "$a";
	}
	return ord($sum) ;
}

while (<>) {
	chomp ;

#	print "$_\n";
	if ((/^\$(.*)\*([0-9a-zA-Z]{2})$/)) {
		$payload=$1; $checksum=$2;
		$c = myXOR($payload);
		if ($c != hex($checksum)) {
			print "cks($payload)=$c,(",sprintf("0x%02x",$c),") $checksum\n";
		}
	}
}
