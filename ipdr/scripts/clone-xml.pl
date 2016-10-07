#!/usr/local/bin/perl -w

# Inputs (from stdin) an XML configuration and outputs the same
# XML, with clients cloned on standard output.
# The XML must have a single <Client> section defined.
# It will be cloned as defined by the parameters.

# Parameters:
#    IP
#    Port1 (used for both exporterPort and clientVersionPort)
#    Port2

# $Id: clone-xml.pl,v 1.2 2005/01/19 19:22:37 dmitryk Exp $

use strict;
use English;

require 5.005;

my $clientIPAddress = shift;
my $port1 = shift;
my $port2 = shift;

die "Missing parameter <IP>" unless defined $clientIPAddress;
die "Missing parameter <Port1>" unless defined $port1;
die "Missing parameter <Port2>" unless defined $port2;

my $xml = join("", <>);

$xml =~ m!(<Clients>\s*$)!m || die "No <Clients> in XML";
my $xmlBeforeClients = $PREMATCH;
my $xmlClientsTag = $1;
$xml = $POSTMATCH;

$xml =~ m!(\s*</Clients>)!s || die "No </Clients> in XML";
my $xmlClients = $PREMATCH;
my $xmlClientsEndTag = $1;
my $xmlAfterClients = $POSTMATCH;

undef $xml;

&checkForSingleClient($xmlClients);

my $xmlNewClients = "";
for (my $port = $port1; $port <= $port2; $port++) {
    my $c = $xmlClients;
    $c =~ s!<IP>.*?</IP>!<IP>$clientIPAddress</IP>! || die "No <IP>";
    $c =~ s!<Port>.*?</Port>!<Port>$port</Port>! || die "No <Port>";
    $c =~ s!<VerPort>.*?</VerPort>!<VerPort>$port</VerPort>! || die "No <VerPort>";
    $xmlNewClients .= $c;
}

print $xmlBeforeClients;
print $xmlClientsTag;
print $xmlNewClients;
print $xmlClientsEndTag;
print $xmlAfterClients;

exit 0;

sub checkForSingleClient {
    my ($xmlClients) = @_;
    $xmlClients =~ m!\s*?<Client>! || die "No <Client> in XML";
    $xmlClients = $POSTMATCH;
    die "Multiple <Client>s in XML" if $xmlClients =~ m!<Client>!;
}

