#!/usr/bin/env python
import webapp2
import os
import jinja2
import os, sys
import pprint

import urllib2

#from google.appengine.api import urlfetch

config = {}

class HomeHandler(webapp2.RequestHandler):
    def get(self):
        print >> sys.stderr, '=============== HERE I AM, FETCHING URL =============='
        template = jinja_environment.get_template('test_app.html')

#        proxy = urllib2.ProxyHandler({'https': 'localhost:8888'})
#        opener = urllib2.build_opener(proxy)
#        urllib2.install_opener(opener)
        response = urllib2.urlopen("https://www.google.nl").read()

#        print >> sys.stderr, '===============' + str(response)
#        result = urlfetch.fetch('http://www.google.com')

        self.response.out.write(template.render(dict(
            some_variable=str(response)
#            some_variable=result.content
        )))


jinja_environment = jinja2.Environment(
    loader=jinja2.FileSystemLoader(os.path.dirname(__file__))
)

app = webapp2.WSGIApplication(
    [('/', HomeHandler)],
    debug=True,
    config=config
)

