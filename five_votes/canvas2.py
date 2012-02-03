import os,sys

os.environ['DJANGO_SETTINGS_MODULE'] = 'conf.settings'

from google.appengine.dist import use_library
use_library('django', '1.2')

# see https://docs.djangoproject.com/en/1.2/topics/i18n/internationalization/
from django.utils.translation import ugettext as _

# Force Django to reload settings
from django.conf import settings
settings._target = None

from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app

from google.appengine.ext.webapp import template

# bring in the lang2name tag
template.register_template_library('templatetags.myfilters')


# I18nrequesthandler handles what django.middleware.locale.LocaleMiddleware
# does for setting in Cookie/Header. It wraps the
# google.appenine.ext.webapp.RequestHandler
from util import I18NRequestHandler

#
# Fluid canvas howto
#
#https://developers.facebook.com/blog/post/549

class MainPage(I18NRequestHandler):
  def post(self):
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

      path = os.path.join(os.path.dirname(__file__), 'index2.html')
      self.response.out.write(template.render(path, template_values))


application = webapp.WSGIApplication([
        ('/', MainPage),
        ], debug=True)

def main():
    run_wsgi_app(application)

if __name__ == "__main__":
  main()

