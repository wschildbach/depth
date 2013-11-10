#!/usr/bin/perl

use POSIX;

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

$PI = 3.1415926;

LINE:
while (<>) {
  chomp ;

  if (($payload,$checksum)=(/^\$(.*)\*([0-9a-zA-Z]{2})$/)) {
    #		$payload=$1; $checksum=$2;
    $c = myXOR($payload);
    
    if ($c != hex($checksum)) {next LINE;}

    @f = split(",");
    if ($f[0] eq '$GPRMC' & $f[2] eq 'A') {
      ($hms,$lat,$lat_ns,$long,$long_ew,$dmy)=@f[1,3,4,5,6,9];
      ($hour,$min,$sec)     = unpack("a2a2a2", $hms);
      ($lat_deg,$lat_min)   = unpack("a3a7", $lat);
      ($long_deg,$long_min) = unpack("a3a7", $long);
      ($day,$mon,$year)     = unpack("a2a2a2", $dmy);
      # canonical angles (0-2 pi)
      $lat = 2*$PI*($lat_ns eq "N" ? 1:-1)*($lat_deg+$lat_min/60)/360;
      $long = 2*$PI*($long_ew eq "E" ? 1:-1)*($long_deg+$long_min/60)/360;
      # mapping to non-singular representation (see http://www.navlab.net/Publications/A_Nonsingular_Horizontal_Position_Representation.pdf)
      @pos = (sin($lat), sin($long)*cos($lat), -cos($long)*cos($lat));
      #			print "lat=$lat long=$long pos=",join(",",@pos),"\n";
      # canonical time format
      $time  = POSIX::mktime ($sec, $min, $hour, $day, $mon, $year);
      #			print "time=$time\n";
      $time += 40*365.25*24*60*60;
      #			$time -= POSIX::mktime(1,1,1,1,1,2010);
      #			print "time=$time\n";
    }
    if ($f[0] eq '$PGRME') {
      ($hprec,$hunits) = @f[1,2];
      warn ("unknown precision units") if ($hunits ne 'M');
    }
    if ($f[0] eq '$INDPT') {
      $depth = $f[1]; #if ($depth eq "") {warn("empty depth");}
      if (defined($time) && $depth ne "") {
	$pos_string = join(",",  @pos);
	printf("%f, %s, %f, %f\n",$time,$pos_string,$hprec,$depth);
	# print "$time,$pos_string,$hprec,$depth\n";
	# a hack so we only output one depth value
	undef $time ;
      }
    }
  }
}
