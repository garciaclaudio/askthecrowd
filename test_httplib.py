
import urllib2


proxy = urllib2.ProxyHandler({'https': 'localhost:8888'})
opener = urllib2.build_opener(proxy)
urllib2.install_opener(opener)

response = urllib2.urlopen("https://www.python.org").read()

print response

#f = urllib2.urlopen('https://www.python.org/')

#print f.read(300)



"""
import httplib

conn = httplib.HTTPSConnection('localhost',8888)
conn.set_tunnel('mail.google.com',443)

#conn = httplib.HTTPSConnection("mail.google.com")

conn.request("GET", "/")
r1 = conn.getresponse()
print r1.status, r1.reason



import httplib
c = httplib.HTTPSConnection('localhost',8888)
c.set_tunnel('google.com',443)
c.request('GET','/')
r1 = c.getresponse()
print r1.status,r1.reason
data1 = r1.read()
print len(data1)
print data1
End of code. I can put anything I want here.

#import sys
#import httplib

#conn = httplib.HTTPSConnection()

conn = httplib.HTTPSConnection('localhost',8888)

#conn.set_tunnel('www.google.com',443)

#conn.request("GET", "/")

#r1 = conn.getresponse()
#print r1.status,r1.reason
#data1 = r1.read()
#print len(data1)

#response = conn.getresponse()
#data = response.read()

#print >> sys.stderr, str(data)

#conn = httplib.HTTPSConnection('webproxy.corp.booking.com', 3128)
#c.set_tunnel('twitter.com',443)
#c.request('GET','/')
#r1 = c.getresponse()

"""
