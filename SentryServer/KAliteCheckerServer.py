#!/usr/bin/env python

# Copyright Jon Berg , turtlemeat.com
# Modified by nikomu @ code.google.com     
#https://code.google.com/p/python-simple-fileserver/#Code_Snippet
import string,cgi,time
from os import curdir, sep
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer


import os # os. path

CWD = os.path.abspath('.')
## print CWD

PORT = 29876


MSG = """

Welcome to the KALite Checker Server.

Here are some examples of the kinds of URLs you are expected to provide.

http://localhost:29876/KALITE?userName=JSstudent&HREF=/math/arithmetic/addition-subtraction/basic_addition/e/number_line/

http://localhost:29876/RANDQ?qFile=somefile.txt
(If somefile.txt is in the same directory as KAliteCheckerServer.py, 
you will get back a random line from that file.)



"""


def linkify(MSG):
    """ Turn URLs into clickable links"""
    MSG =MSG.replace('\n',' <br/>\n')
    fixedWords=[]
    for word in MSG.split():
        if 'http:' in word:
            fixedWord = '<a href="' + word + '">\n' + word + '</a>\n'
        else:
            fixedWord = word
        fixedWords.append(fixedWord)
    return ' '.join(fixedWords).replace('<br/>','<br/>\n')
 

UPLOAD_PAGE = 'upload.html' # must contain a valid link with address and port of the server     s


def make_index( relpath ):     

    abspath = os.path.abspath(relpath) # ; print abspath
    flist = os.listdir( abspath ) # ; print flist
    
    rellist = []
    for fname in flist :     
        relname = os.path.join(relpath, fname)
        rellist.append(relname)
    
    # print rellist
    inslist = []
    for r in rellist :     
        inslist.append( '<a href="%s">%s</a><br>' % (r,r) )
    
    # print inslist
    
    page_tpl = "<html><head></head><body>%s</body></html>"     
    
    ret = page_tpl % ( '\n'.join(inslist) , )
    
    return ret


# -----------------------------------------------------------------------
from urlparse import urlparse, parse_qs
from loginToKALITE import status

class MyHandler(BaseHTTPRequestHandler):

    def startPage(self):
        self.send_response(200)
        self.send_header('Content-type',	'text/html')
        self.end_headers()


    def do_GET(self):
        TESTING = False
        try:
            
            if self.path.upper().startswith('/KALITE'):
                self.send_response(200)
                self.send_header('Content-type',	'text/html')
                self.end_headers()

                parsedURL = urlparse(self.path)
                parsedQuery = parse_qs( parsedURL.query) 
                
                if TESTING:
                    self.wfile.write('KALITE<hr><pre>')
                    self.wfile.write('self.path: \t' + self.path + '\n\n')
    
                    self.wfile.write('parsedURL: ' +            str( parsedURL ) + '\n\n' )
                    self.wfile.write('parsedURL.query: ' +      str( parsedURL.query ) + '\n\n')
                    self.wfile.write('parse_qs( parsedURL.query): ' +      str( parsedQuery) + '\n\n')

                if 'userName' in parsedQuery and 'HREF' in parsedQuery:
                    userName = parsedQuery['userName'][0]
                    HREF =  parsedQuery['HREF'][0]                    
                    try:
                        self.wfile.write(status( userName, HREF ) )
                    except KeyError as e:
                        print 'ERROR:  KeyError:', e, 'userName', userName, 'HREF', HREF
                        self.wfile.write('ERROR:  KeyError ' + str(e))
                    return
                else:
                    self.wfile.write('<hr>ERROR!<br/>  ' + str( parsedQuery ).replace(',',',<br/><hr>' ) )
                 
                       
            if self.path.upper().startswith('/RANDQ'):
                qMSG = """
                    Try http://localhost:29876/RANDQ?qFile=somefile.txt
                """
                self.send_response(200)
                self.send_header('Content-type',	'text/html')
                self.end_headers()

                parsedURL = urlparse(self.path)
                parsedQuery = parse_qs( parsedURL.query) 
                
                if TESTING:
                    self.wfile.write('KALITE<hr><pre>')
                    self.wfile.write('self.path: \t' + self.path + '\n\n')
    
                    self.wfile.write('parsedURL: ' +            str( parsedURL ) + '\n\n' )
                    self.wfile.write('parsedURL.query: ' +      str( parsedURL.query ) + '\n\n')
                    self.wfile.write('parse_qs( parsedURL.query): ' +      str( parsedQuery) + '\n\n')

                if 'qFile' in parsedQuery:
                    from random import choice
                    qContent = open( parsedQuery['qFile'][0] ).readlines()                   
                    self.wfile.write('<pre>' + choice( qContent ) + '</pre>')

                else:
                    self.wfile.write(linkify(qMSG))
                return
                """
                    userName = parsedQuery['userName'][0]
                    HREF =  parsedQuery['HREF'][0]                    
                    try:
                        self.wfile.write(status( userName, HREF ) )
                    except KeyError as e:
                        print 'ERROR:  KeyError:', e, 'userName', userName, 'HREF', HREF
                        self.wfile.write('ERROR:  KeyError ' + str(e))
                    return
                else:
                    self.wfile.write('<hr>ERROR!<br/>  ' + str( parsedQuery ).replace(',',',<br/><hr>' ) )
                """
            
            
            
            
            if self.path.upper().startswith('/HTTP://KHANACADEMY.ORG'):
                self.send_response(200)
                self.send_header('Content-type',	'text/html')
                self.end_headers()
                
                self.wfile.write('KHAN CALL<hr>')
                self.wfile.write(self.path)
                return

            if self.path == '/' :     
                page = make_index( '.' )
                self.send_response(200)
                self.send_header('Content-type',	'text/html')
                self.end_headers()
                self.wfile.write( linkify(MSG) )
                #self.wfile.write(page)
                return     


            """if self.path.endswith(".html"):
                ## print curdir + sep + self.path
                f = open(curdir + sep + self.path)
                #note that this potentially makes every file on your computer readable by the internet

                self.send_response(200)
                self.send_header('Content-type',	'text/html')
                self.end_headers()
                self.wfile.write(f.read())
                f.close()
                return"""
                
            if self.path.endswith(".esp"):   #our dynamic content
                self.send_response(200)
                self.send_header('Content-type',	'text/html')
                self.end_headers()
                self.wfile.write("hey, today is the" + str(time.localtime()[7]))
                self.wfile.write(" day in the year " + str(time.localtime()[0]))
                return
                
            else:
                self.send_response(200)
                self.send_header('Content-type',	'text/html')
                self.end_headers()
                self.wfile.write('Default page.<hr>')
                self.wfile.write(self.path)
            return # be sure not to fall into "except:" clause ?       
                
        except IOError as e :  
            # debug     
            print e
            self.send_error(404,'File Not Found: %s' % self.path)
     

    def do_POST(self):
        # global rootnode ## something remained in the orig. code     
        try:
            ctype, pdict = cgi.parse_header(self.headers.getheader('content-type'))     

            if ctype == 'multipart/form-data' :     

                # original version :     
                '''
                query=cgi.parse_multipart(self.rfile, pdict)
                upfilecontent = query.get('upfile')
                print "filecontent", upfilecontent[0]
                '''

                # using cgi.FieldStorage instead, see 
                # http://stackoverflow.com/questions/1417918/time-out-error-while-creating-cgi-fieldstorage-object     
                fs = cgi.FieldStorage( fp = self.rfile, 
                                       headers = self.headers, # headers_, 
                                       environ={ 'REQUEST_METHOD':'POST' } # all the rest will come from the 'headers' object,     
                                       # but as the FieldStorage object was designed for CGI, absense of 'POST' value in environ     
                                       # will prevent the object from using the 'fp' argument !     
                                     )
                ## print 'have fs'

            else: raise Exception("Unexpected POST request")
                
                
            fs_up = fs['upfile']
            filename = os.path.split(fs_up.filename)[1] # strip the path, if it presents     
            fullname = os.path.join(CWD, filename)

            # check for copies :     
            if os.path.exists( fullname ):     
                fullname_test = fullname + '.copy'
                i = 0
                while os.path.exists( fullname_test ):
                    fullname_test = "%s.copy(%d)" % (fullname, i)
                    i += 1
                fullname = fullname_test
                
            if not os.path.exists(fullname):
                with open(fullname, 'wb') as o:
                    # self.copyfile(fs['upfile'].file, o)
                    o.write( fs_up.file.read() )     


            self.send_response(200)
            self.end_headers()
            
            self.wfile.write("<HTML><HEAD></HEAD><BODY>POST OK.<BR><BR>");
            self.wfile.write( "File uploaded under name: " + os.path.split(fullname)[1] );
            self.wfile.write(  '<BR><A HREF=%s>back</A>' % ( UPLOAD_PAGE, )  )
            self.wfile.write("</BODY></HTML>");
            
            
        except Exception as e:
            # pass
            print e
            self.send_error(404,'POST to "%s" failed: %s' % (self.path, str(e)) )
            
            
def timestamp():
    import datetime
    return datetime.datetime.now().strftime("%A, %d. %B %Y %I:%M%p")

def main():

    try:
        print MSG
        import webbrowser
        webbrowser.open( 'http://localhost:29876' )

        server = HTTPServer(('', 29876), MyHandler)
        print
        print 'OK: ready to respond.'
        print 'started httpserver',  timestamp()
        server.serve_forever()
    except KeyboardInterrupt:
        print '^C received, shutting down server'
        server.socket.close()

if __name__ == '__main__':
    main()

