##!/usr/bin/perl
#
# Copyright Andreas Schlapbach, schlpbch@iam.unibe.ch, 2001
# http://iamexwiwww.unibe.ch/studenten/kfinger
#
# Touch at your own risk.


# Highlight mail addresses or url

$mails    = '<A HREF="mailto:';
$urls	  = '<A HREF="';
$urlspace = '">';
$urlend   = '</A>';

# Highlight various information, configurable via the CSS file,

$finger	 	= '<CODE class="finger">';
$domainName	= '<CODE class="domainName">';
$ipNumber  	= '<CODE class="ipNumber">';
$os	 	= '<CODE class="os">';
$username 	= '<CODE class="username">';
$directory	= '<CODE class="directory">';
$shell 		= '<CODE class="shell">';
$notLoggedIn 	= '<CODE class="Login">';
$loggedIn  	= '<CODE class="noLogin">';
$newMail 	= '<CODE class="newMail">';
$plan 		= '<CODE class="plan">';
$noNewMail 	= '<CODE class="noNewMail">';
$noPlan 	= '<CODE class="noPlan">';
$close	 	= '</CODE>';

# Those names get skipped, so if there's a user with such a name, bad luck.

@keywords=('Welcome','Login','finger','No');
$keywordlist = join '|', @keywords;

$FINGERCMD   = "$ARGV[0]";  # The complete path to the finger cmd
$CSSFILE     = "$ARGV[1]";  # The complete path to the CSS file
$REFRESHRATE = "$ARGV[2]";  # The intervals in seconds until the page gets updated
$HOST        = "$ARGV[3]";  # host name
$USER        = "$ARGV[4]";  # user name

# HTML Header

print <<HTMLHeader;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
  <meta http-equiv="refresh" content="$REFRESHRATE">
  <TITLE>finger $USER\@$HOST</TITLE>
  <LINK type="text/css" rel="stylesheet" href="file:$CSSFILE"> 
</HEAD>
<BODY>
  <TABLE class="mainTable" cellspacing="0">
  <TR> 
    <TH colspan="1">
      <H1>finger    $USER\@$HOST</H1>
    </TH>
  </TR>
  <TR>   
    <TH>
      <TABLE class="courierText" cellpadding="0" cellspacing="2">
HTMLHeader

# Run finger command and save it into a buffer

$buffer = `$FINGERCMD $USER\@$HOST`;
@lines = split /^/m, $buffer;

# Do highlighting using perl regular expressions on every line received.
# Order is important here.

foreach $output (@lines)
 {
  $output =~ s/((\w)+\@((\w)+(.))*(\w)+)/$mails$1$urlspace$1$urlend/gi; # Highlight email address  
  $output =~ s/((http|ftp)(:\/\/)(\S)+)/$urls$1$urlspace$1$urlend/gi;   # Highlight urls  
  $output =~ s/((\d)+\.(\d)+\.(\d)+\.(\d)+)/$ipNumber$1$close/gi;       # Highlight IP number
  $output =~ s/((\w)+\.(\w)+\.(\w|-)+\s)/$domainName$1$close/gi;        # Highlight domain name  (\s is important)
  $output =~ s/(finger:)/$finger$1$close/gim;                           # Highlight finger
  $output =~ s/(Linux)/$os$1$close/gim;                                 # Highlight Linux
  if ($USER)  # is $USER nil ?
   {
    $output =~ s/^Login:\s*(\w*)/Login: $mails$1\@$HOST$urlspace$1$urlend/gi;
    $output =~ s/^Login Name:\s*(\w*)/Login Name:$mails$1\@$HOST$urlspace$1$urlend/gi;
    $output =~ s/Name:(((\s*)(\w+))+\n)/Name:$username$1$close\n/gi;  # Linux
    $output =~ s/In real life:(((\s*)(\w+))+\n)/In real life:$username$1$close\n/gi;  # Solaris
    $output =~ s/^Directory:((\s*)(\/(\w)+)+)/Directory:$directory$1$close/gi;  # Highlight Directory
    $output =~ s/Shell:((\s*)(\/(\w)+)+)/Shell:$shell$1$close/gi; # Highlight Shell
    $output =~ s/(not presently logged)/$notLoggedIn$1$close/gi;
    $output =~ s/con (\w*)/con $loggedIn$1$close/gi;
    $output =~ s/^(New mail)/$newMail$1$close/gi;
    $output =~ s/^(No mail.)/$noNewMail$1$close/gim;
    $output =~ s/^(Plan:)/$plan$1$close/gi;
    $output =~ s/^(No plan.)/$noPlan$1$close/gim;
   }
  else
   {
    $output =~ s/^(\w+)/$mails$1\@$HOST$urlspace$1$urlend/m unless ($output =~ m/$keywordlist/m);  
   }
  # line consists of white space only?
  if ($output =~ m/\S/gi) 
   {
    print "        <TR><TD><PRE>$output</PRE></TD></TR>\n";
   } 
  else 
   {
    print "        <TR><TD><PRE>  </PRE></TD></TR>\n";
   }    
}

print "      </TABLE>\n";
print "    </TH>\n";

# Finger-Talk options

if ($USER)  # is $USER nil ?
{
print <<UserQuery; 
  </TR>
  <TR>
    <TH class="commandText" colspan="2">
      <A HREF='finger://$USER\@$HOST'>finger</A>
    </TH>
  </TR>
UserQuery
}
else
{
print <<HostQueryHead;
    <TH>
      <TABLE class="courierText" cellpadding="0" cellspacing="2">
HostQueryHead

    @lines = split /^/m, $buffer;
    foreach $output2 (@lines) 
     {
      if ($output2 =~ m/^(\w+)/gi and not ($output2 =~ m/$keywordlist/m)) 
       {
        $USER = $&; 
        print "        <TR><TD><PRE><A HREF='finger://$USER\@$HOST'>finger</A>\n</PRE></TD></TR>\n";
        # - <A HREF='talk://$USER\@$HOST'>talk</A>\n</PRE></TD></TR>\n";
       } 
      else 
       {      
        print "        <TR><TD><PRE>  </PRE></TD></TR>\n";  	
       }
     }    

print <<HostQueryTail;
      </TABLE> 
    </TH>
  </TR>
HostQueryTail
}

# HTMLTail

print <<HTMLTail;
  <TR>
    <TH class="niceText">refresh rate: $REFRESHRATE seconds.</TH>
  </TR>
</TABLE>
</BODY>
</HTML>
HTMLTail
