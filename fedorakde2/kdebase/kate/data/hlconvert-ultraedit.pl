#!/usr/bin/perl

$filename = $ARGV[0];

$newfilename = $filename;
$newfilename =~ s/\.txt/\.xml/gi;

$stdDel = "!%&()*+,-./:;<=>?[]^{|}~ \t";

$z = 0;
open (FILE,"<$filename");
foreach $_ (<FILE>)
{
   $file[$z] = $_;
   $z++;
}
close FILE;

open (FILE,">$newfilename");

print FILE "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print FILE "<!DOCTYPE language SYSTEM \"language.dtd\">\n";

# get the name of the hl
@field = split /\"/, $file[0];
$hlname = @field[1];

# casesensitive ?
$case = 1;
if ( $file[0] =~ /Nocase/ )
{
  $case = 0;
}

# get the extensions of the hl
@field = split /File Extensions =/, $file[0];
@field = split / /, @field[1];

$ext = "";
$c = 1;
while (@field[$c] ne "")
{
  @field[$c] =~ s/\n//gi;
  @field[$c] =~ s/\r//gi;

  if (length(@field[$c]) > 0)
  {
    $ext .= "*.@field[$c];";
  }

  $c++;
}

# get the single line comment
@field = split /Line Comment =/, $file[0];
@field = split / /, @field[1];
$linecomment = @field[1];

# get the multi line comment
@field = split /Block Comment On =/, $file[0];
@field = split / /, @field[1];
$commentstart = @field[1];

@field = split /Block Comment Off =/, $file[0];
@field = split / /, @field[1];
$commentend = @field[1];

# get the string delimeter
@field = split /String Chars =/, $file[0];
@field = split / /, @field[1];
$delimeter = @field[1];

print FILE "<language name=\"$hlname\" extensions=\"$ext\" mimetype=\"\">\n";
print FILE "  <highlighting>\n";

$i=0;
$s=0;
while ($i <= $z)
{
  if ($file[$i] =~ /\/C[1234567890]/)
  {
    $sections[$s] = $i;

    @field = split /\"/, $file[$sections[$s]];
    $listname[$s] = @field[1];
    if ($listname[$s] eq "")
    {
      $listname[$s] = "WordList$s";
    }
    else
    {
      $listname[$s] .= $s;
    }

    $s++;
  }
  if ($file[$i] =~ /\/Delimiters/)
  {
    @field = split /Delimiters = #/, $file[$i];
    $weakdel = @field[1];
  }
  $i++;
}

$weakdel = $stdDel;

$weakdel =~ s/</\&lt;/gi;
$weakdel =~ s/>/\&gt;/gi;
$weakdel =~ s/\&/\&amp;/gi;
$weakdel =~ s/\"/\&quot;/gi;
$weakdel =~ s/\n//gi;
$weakdel =~ s/\r//gi;

$n=0;
while (($n < $s) && ($file[$sections[$n]] ne ""))
{
  print FILE "    <list name=\"$listname[$n]\">\n";

  $t = $sections[$n]+1;

  $end = $sections[$n+1];
  if ($end eq "")
  {
    $end = $z+1;
  }

  while ($t < $end)
  {
    @field = split / /, $file[$t];

    $c = 0;
    while (length(@field[$c]) > 0)
    {
      @field[$c] =~ s/\n//gi;
      @field[$c] =~ s/\r//gi;

      @field[$c] =~ s/</\&lt;/gi;
      @field[$c] =~ s/>/\&gt;/gi;
      @field[$c] =~ s/\&/\&amp;/gi;
      @field[$c] =~ s/\"/\&quot;/gi;

      if (length(@field[$c]) > 0)
      {
        print FILE "      <item>@field[$c]</item>\n";
      }

      $c++;
    }

    $t++;
  }

  print FILE "    </list>\n";
  $n++;
}

print FILE "    <contexts>\n";
print FILE "      <context name=\"Normal\" attribute=\"0\" lineEndContext=\"0\">\n";

$n=0;
while (($n < $s) && ($file[$sections[$n]] ne ""))
{
  $str = $n+4;
  print FILE "        <keyword attribute=\"$str\" context=\"0\" String=\"$listname[$n]\" />\n";
  $n++;
}

if ($delimeter ne "")
{
  print FILE "      <DetectChar char=\"&quot;\" attribute=\"1\" context=\"1\" \/>\n";
}

print FILE "      </context>\n";

if ($delimeter ne "")
{
  print FILE "      <context attribute=\"1\" lineEndContext=\"1\" name=\"String\">\n";
  print FILE "        <LineContinue attribute=\"1\" context=\"2\"/>\n";

  if ($delimeter =~ /'/)
  {
    print FILE "        <HlCStringChar attribute=\"2\" context=\"1\"/>\n";
  }

  if ($delimeter =~ /\"/)
  {
    print FILE "        <DetectChar attribute=\"1\" context=\"0\" char=\"&quot;\"/>\n";
  }

  print FILE "      </context>\n";

  print FILE "      <context attribute=\"1\" lineEndContext=\"2\" name=\"Continue\">\n";
  print FILE "        <RegExpr String=\"^\" attribute=\"1\" context=\"1\"/>\n";
  print FILE "      </context>\n";
}

if ($linecomment ne "")
{
 #     <context attribute="10" lineEndContext="0">
  #      <RegExpr attribute="3" context="2" String="(FIXME|TODO)" />
   #   </context>
}

if (($commentstart ne "") && ($commentend ne ""))
{

 # <context attribute="10" lineEndContext="3">
  #      <Detect2Chars attribute="10" context="0" char="*" char1="/"/>
   #     <RegExpr attribute="3" context="3" String="(FIXME|TODO)" />
    #  </context>
}


print FILE "    </contexts>\n";
print FILE "    <itemDatas>\n";
print FILE "      <itemData name=\"Normal\" defStyleNum=\"dsNormal\"/>\n";
print FILE "      <itemData name=\"String\"  defStyleNum=\"dsString\"/>\n";
print FILE "      <itemData name=\"String Char\"  defStyleNum=\"dsChar\"/>\n";
print FILE "      <itemData name=\"Comment\" defStyleNum=\"dsComment\"/>\n";

$n=0;
while (($n < $s) && ($file[$sections[$n]] ne ""))
{
  print FILE "      <itemData name=\"$listname[$n]\" defStyleNum=\"dsNormal\" color=\"#ff0000\" selColor=\"#ff0000\" bold=\"1\" italic=\"0\" />\n";
  $n++;
}

print FILE "    </itemDatas>\n";
print FILE "  </highlighting>\n";
print FILE "  <general>\n";

if (($linecomment ne "") || ($commentstart ne "") && ($commentend ne ""))
{
  print FILE "    <comments>\n";

  if ($linecomment ne "")
  {
    print FILE "      <comment name=\"singleLine\" start=\"$linecomment\" />\n";
  }

  if (($commentstart ne "") && ($commentend ne ""))
  {
    print FILE "      <comment name=\"multiLine\" start=\"$commentstart\" end=\"$commentend\" />\n";
  }

  print FILE "    </comments>\n";
}

print FILE "    <keywords casesensitive=\"$case\"/>\n";

if ($weakdel ne "")
{
  print FILE "    <keywords weakDeliminator=\"$weakdel\"/>\n";
}

print FILE "  </general>\n";
print FILE "</language>\n";

close FILE;
