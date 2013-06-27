#!/usr/bin/env python
import webapp2
import os
import jinja2
import os, sys
import pprint

from google.appengine.api import urlfetch

os.environ['http_proxy'] = 'http://webproxy.corp.booking.com:3128'
os.environ['https_proxy'] = 'http://webproxy.corp.booking.com:3128'

config = {}

class HomeHandler(webapp2.RequestHandler):
    def get(self):
        print >> sys.stderr, '=============== HERE I AM =============='
        template = jinja_environment.get_template('test_app.html')

        result = urlfetch.fetch('http://www.google.com')

        self.response.out.write(template.render(dict(
            some_variable=result.content
        )))


jinja_environment = jinja2.Environment(
    loader=jinja2.FileSystemLoader(os.path.dirname(__file__))
)

app = webapp2.WSGIApplication(
    [('/', HomeHandler)],
    debug=True,
    config=config
)

