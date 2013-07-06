#!/usr/bin/env python
#	Aug 19 2011
#	Copyleft@2011 Published Under BSD Lisense
#			Ronald Liu
#	lzsdc01@gmail.com
#	FYI  http://lzsblog.appspot.com/%3Fp%3D291001
#
import sys,DNS,base64

def splitList(txt):
	arr = txt.split("\n")
	
	l = []
	for line in arr:
		if (not len(line)): #empty line
			continue
		if (line[0] == "!"): #Comment line
			continue
		elif(line[0:2] =="@@"):#Forbidding line
			continue
		elif(line.find("/")!=-1 or line.find("*")!=-1): #URL is ignored, only domains left
			continue
		
		#In this case, domain name is irrelevant to protocol(http or https)
		elif(line[0:2] =="||"):
			l.append(line[2:])
		elif(line[0] == "."):
			l.append(line[1:])
		else:
			l.append(line)
	
	return l
	
	

#Decode and decorate the input string
f = open("gfwlist.txt","r")
txt = f.read().replace("\n","")
txt = base64.decodestring(txt)

domains = splitList(txt)

#Set default DNS server
DNSServer=['8.8.8.8']
DNS.defaults['server']=DNSServer

DNS.DiscoverNameServers()

#These two varible are used to track the percentage of parsing process.
l = 0
a = len(domains)

for line in domains:
	request=DNS.Request()
	try:
		result=request.req(name=line,qtype="A")
	except DNS.Base.DNSError:
		msg=line+"\tTime Out\n"
	else:
		if not len(result.answers):
			msg=line+"\tNo record\n"
		else:
			msg=line+ "\tBingo\n"
			for i in result.answers:
				if (i["typename"]=='A'):	#Only A record is useful IP
					print line + " " + i["data"]
	print >> sys.stderr, line + msg, l*1.0/a, "%"