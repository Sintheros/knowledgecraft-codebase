#!/usr/bin/env ipython -i

from twill.commands import go, showforms, formclear, fv, show, submit
#import urllib2 #added JIS
def login():
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

def queryOnePage(topic='addition-subtraction'): #this is the reference tester.  looks specifically for addition-subtraction
    page = "http://129.21.142.118:8008/coachreports/?group=1535408fa1415e25a4c781b63e66068c&topic=" + topic
    #manually extracted from URL
    go(page)
    content = show()
    
    print 80*'\n'
    
    from BeautifulSoup import BeautifulSoup
    soup = BeautifulSoup( content )
    
    def between(start_marker, end_marker, raw_string ):
        start = raw_string.index(start_marker) + len(start_marker)
        end = raw_string.index(end_marker, start)
        return raw_string[start:end]
    
    
    tables=soup.findAll('table')
    
    studentSpans = tables[0].findAll('span')
    students  = [span['title'] for span in studentSpans]
    userNames = [between('(',')',student )  for student in students]
    print 'userNames:::', userNames
    print
    
    statuses = tables[1]
    statusRows = statuses.findAll('tr')
    exerciseHREFs = [ anchor['href'] for anchor in statusRows[0].findAll('a')]
    
    userStatusRows = statusRows[1:]
    userStatuses={}
    
    for i in range(len(userNames)):
        userStatuses[ userNames[i] ] = [ status['title'] for status in userStatusRows[i].findAll('td') ]    
    
    from pprint import pprint as P
    
    return {'userNames':userNames, 
            'userStatuses':userStatuses, 
            'exerciseHREFs': exerciseHREFs}

def showData(coachingPageData):  
    """use this to understand or display the data format"""
    D = coachingPageData
    userNames, userStatuses, exerciseHREFs = D['userNames'], D['userStatuses'], D['exerciseHREFs']
    
    TAB = '\t'
    for userName in userNames:
        print
        for i in range(len(exerciseHREFs)):
            print userName, TAB, userStatuses[userName][i], TAB, exerciseHREFs[i]
 
from BeautifulSoup import BeautifulSoup

login()
go( "http://129.21.142.118:8008/coachreports/" )
soup = BeautifulSoup( show() )

#get all the topics (like 'addition-subtraction') from the select widget on the page 
selectDivs = soup.findAll('div', {'class':'selection'}) 
divWIthTopics = selectDivs[1]
optionTags = divWIthTopics.findAll('option')[1:] #first one is empty
topics = [option['value'] for option in optionTags]

onePage = queryOnePage( topics[0] )
showData(onePage)

"""
for value in optionsValues: 
    print 'value', value
    queryOnePage(value)
    print 'That was value', value
    print
    print
 """