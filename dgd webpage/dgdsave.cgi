#!/usr/bin/perl

$my_input = $ENV{QUERY_STRING};
@fv_pairs = split /\&/ , $my_input;
foreach $pair (@fv_pairs) 
{
 if($pair=~m/([^=]+)=(.*)/) 
 {
  $field = $1; 
  $value = $2; 
  $value =~ s/\+/ /g; 
  $value =~ s/%([\dA-Fa-f]{2})/pack("C", hex($1))/eg; 
  $INPUT{$field}=$value; 
 } 
}

my $file = "../dgd/$INPUT{Name}-$INPUT{ipAddr}.csv"; 
open (FILE, ">>$file") or die "Cannot open $file: $!"; 
print FILE "Conqueror\t";
print FILE "Manager\t";
print FILE "Wanderer\t";
print FILE "Participant\t";
print FILE "Sex\t";
print FILE "Age\t";
print FILE "Gamer Type\t";
print FILE "Gplay liked\t";
print FILE "Gplay hated\t";
print FILE "StudentNum\t";
print FILE "Name\t";
print FILE "Client IP\n";
print FILE "$INPUT{Type1}\t";
print FILE "$INPUT{Type2}\t";
print FILE "$INPUT{Type3}\t";
print FILE "$INPUT{Type4}\t";
print FILE "$INPUT{Sex}\t";
print FILE "$INPUT{Age}\t";
print FILE "$INPUT{GamerType}\t";
print FILE "$INPUT{GpLiked1},$INPUT{GpLiked2},$INPUT{GpLiked3}\t";
print FILE "$INPUT{Part1Q3}\t";
print FILE "$INPUT{StuNum}\t";
print FILE "$INPUT{Name}\t";
print FILE "$INPUT{ipAddr}";
close(FILE);

print "Content-type: text/html\n\n";
print qq~<script language=javascript>alert("Submission Saved")</script>~;