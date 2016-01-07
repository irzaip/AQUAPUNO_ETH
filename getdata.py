import urllib2

time_get = 500
url_getfrom = "http://192.168.30.93/parsed.html?GETINFO=1"
url_postto = "http://192.168.35.200/AQUAPUNO/mydata/input?"

temp = 20
moist = 20
light = 10
flow1 = 10
flow2 = 10
relay1 = 20
relay2 = 10
relay3 = 10
relay4 = 11


try:
    page = urllib2.urlopen(url_getfrom,None,5)
    b = page.read().splitlines()[5]
    print b
    if b[0:4]=="MYDT":
        a = b.split(":")
        temp = int(float(a[2]))
        moist = int(float(a[3]))
        light = a[4]
        flow1 = a[5]
        flow2 = a[6]
        relay1 = a[7]
        relay2 = a[8]
        relay3 = a[9]
        relay4 = a[10]
    print "GET DATA OK"   
except:
    print "ERROR RETRIEVING"



param = "temp="+str(temp)+"&moist="+str(moist)+"&light="+str(light)+"&flow1="+str(flow1)+"&flow2="+str(flow2)
param = param + "&relay1="+str(relay1)+"&relay2="+str(relay2)+"&relay3="+str(relay3)+"&relay4="+str(relay4)

try:
    print url_postto+param
    page = urllib2.urlopen(url_postto+param)
    print "DONE POSTING"
except:
    print "ERROR POSTING"
    

