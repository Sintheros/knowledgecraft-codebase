#!/usr/bin/env ipython -i

from twill.commands import go, showforms, formclear, fv, show, submit
#import urllib2 #added JIS

page = 'http://129.21.142.118:8008/securesync/login/'
username = 'knowledgecraft'
password = 'knowledgecraft'
facility = 'dbae7005f9b45ce082b5fe0a0985946a'
 
go(page)
print "Forms:"
showforms()
 
try:
	# Force try using the first form found on a page.
	formclear('1')
	fv("1", "username", username)
	fv("1", "password", password)
	fv("1", "facility", facility)
	#fv("1", "csrfmiddlewaretoken", '3F1bMuIIM9ERzcp6ceEyFxlT51yJKsK6')
	submit('0')
	content = show()
	print 'debug twill post content:', content
 
except urllib2.HTTPError, e:
	sys.exit("%d: %s" % (e.code, e.msg))
except IOError, e:
	print e
except:
	passexit


page = "http://129.21.142.118:8008/coachreports/?group=1535408fa1415e25a4c781b63e66068c&topic=telling-time"
#manually extracted from URL
go(page)
show()