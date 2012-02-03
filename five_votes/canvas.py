import os,sys, math

os.environ['DJANGO_SETTINGS_MODULE'] = 'conf.settings'

from google.appengine.dist import use_library
use_library('django', '1.2')

# see https://docs.djangoproject.com/en/1.2/topics/i18n/internationalization/
from django.utils.translation import ugettext as _

# Force Django to reload settings
from django.conf import settings
settings._target = None

import mystorage

from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app

from google.appengine.ext.webapp import template

# ZAP!
import tab_content

template.register_template_library('templatetags.myfilters')


# I18NRequestHandler handles what django.middleware.locale.LocaleMiddleware
# does for setting in Cookie/Header. It wraps the
# google.appenine.ext.webapp.RequestHandler
from util import I18NRequestHandler


#
#print >> sys.stderr, 'XXX TEXT ' + str(settings.TEMPLATE_DEBUG)
#print >> sys.stderr, 'XXX TEXT ' + str(settings.LANGUAGE_CODE)
#print >> sys.stderr, 'XXX PRUEBA --------> ' + _('hello world')
#


#
#http://www.djangoproject.com/documentation/0.96/templates/#built-in-filter-reference
#
#from django.utils.html import linebreaks



def linebreaksbr(value):
    "Converts newlines into <br />s"
    return value.replace('\n', '<br />')


class GetImage(I18NRequestHandler):
  
    def get(self):

        fb_uid = self.request.get('fb_uid')

        signer_query = mystorage.Signer.gql('where fb_id = :1', int(fb_uid) )
        signer = signer_query.get()

#        if not signer:
#          print >> sys.stderr, 'SIGNER NOT THERE' + str(fb_uid)
#        else:
#          print >> sys.stderr, 'SIGNER ALREADY THERE'  + str(fb_uid)

        if (signer and signer.picture):
            self.response.headers['Content-Type'] = 'image/jpg'
            self.response.out.write(signer.picture)


class UserMatch(I18NRequestHandler):
    def get( self, match_url_name ):

        # is the url_name already in use?
        qq = mystorage.Match.gql( 'where url_name = :1', str(match_url_name) )

        user_match = qq.get()

        if( user_match ): 

            template_values = {
                'IN_DEV_SERVER' : settings.IN_DEV_SERVER,
                'NOT_IN_DEV_SERVER' : not settings.IN_DEV_SERVER,
                'match_key' : str(user_match.key()),
                'match_desc' : user_match.desc,
                'match_extended_desc' : linebreaksbr( user_match.extended_desc ),
                'match_title' : user_match.title,
                }

            path = os.path.join( os.path.dirname(__file__), 'match.html' )

            self.response.out.write( template.render(path, template_values) )

        else:
            self.response.out.write( 'not found' )            

class MainPage(I18NRequestHandler):
  def get(self):

      lang = self.request.get('lang', '')

      if lang:
          if lang == 'unset':
              del self.request.COOKIES['django_language']
          else:
              self.request.COOKIES['django_language'] = lang
              self.reset_language()

      try:
          selected_lang = self.request.COOKIES['django_language']
      except:
          selected_lang = self.request.LANGUAGE_CODE

#      print >> sys.stderr, 'SELECTED LANG' + str(selected_lang)
#      print >> sys.stderr, 'DJANGO LANG' + str(self.request.LANGUAGE_CODE)

      # needed for the like button
      locale = 'en_US'
      if lang == 'es':
          locale = 'es_ES'

      template_values = {
          'language_' + selected_lang : 1,
          'locale' : locale,
          'IN_DEV_SERVER' : settings.IN_DEV_SERVER,
          'NOT_IN_DEV_SERVER' : not settings.IN_DEV_SERVER
          }

      path = os.path.join(os.path.dirname(__file__), 'index.html')
      self.response.out.write(template.render(path, template_values))


application = webapp.WSGIApplication([
    ('/image', GetImage),
    ( r'/(.+)', UserMatch ),
    ('/', MainPage),
    ],
                                     debug=True)

def main():
  run_wsgi_app(application)

if __name__ == "__main__":
  main()

