import os,sys
import gminifb
import mystorage



from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app

from google.appengine.ext.webapp import template
os.environ['DJANGO_SETTINGS_MODULE'] = 'conf.settings'

# XXX: not needed, remove
#sys.path.append("/home/claudio/google_appengine/askthecrowd") 
#sys.path.append("/home/claudio/google_appengine/askthecrowd/conf") 

# Force Django to reload settings
from django.conf import settings
settings._target = None


_FbApiKey = "bd0a060ac2cb439c65254a9152344289"
_FbSecret = gminifb.FacebookSecret("d1e7cf7973bfd6cba048adeb8b811dc9")

_canvas_url = "http://apps.facebook.com/askthecrowd"
_app_name = "Ask the Crowd"


# I18NRequestHandler handles what django.middleware.locale.LocaleMiddleware
# does for setting in Cookie/Header. It wraps the
# google.appenine.ext.webapp.RequestHandler
from util import I18NRequestHandler


class MainPage(I18NRequestHandler):
  def get(self):

    cookie_django_language = self.request.get('cookie_django_language', '')
    if cookie_django_language:
      if cookie_django_language == 'unset':
        del self.request.COOKIES['django_language']
      else:
        self.request.COOKIES['django_language'] = cookie_django_language
      self.reset_language()

#
# XXX: remove for deploying
#
#    arguments = gminifb.validate(_FbSecret, self.request)
#
#    session_key = arguments["session_key"]
#    uid = arguments["user"]
#
#    usersInfo = gminifb.call("facebook.users.getInfo",
#              _FbApiKey, _FbSecret, session_key=session_key,
#              call_id=True, fields="name,pic_square",
#              uids=uid) # uids can be comma separated list

##    arguments = {
##      'user' : 518261219,
##      'added': 1,
##      'name' : "Claudio Garcia",
##      }

#    # is user already in our DB?
#    user_query = mystorage.User.gql('where fb_id = :1', arguments["user"] );
#
#    user = user_query.get()
#
#    if user:
#      print >> sys.stderr, "User there"
#    else:
#      print >> sys.stderr, "User not there, creating..."
#      user = mystorage.User();
#      user.fb_id = arguments["user"]
#      user.added_app = arguments["added"]
#      #XXX, we may not want to store the name, because every time
#      #it comes with the FB data
#      user.name = arguments["name"]
#      user.put()

##    print >> sys.stderr, user
##    print >> sys.stderr, "SYS PATH\n"
##    print >> sys.stderr, sys.path

    try:
      selected_lang = self.request.COOKIES['django_language']
    except:
      selected_lang = "en"
      
    template_values = {
      # XXX: remove before deploying
#      'user_name': usersInfo[0]["name"],
#      'user_name': 'Claudio Garcia',

      'language_' + selected_lang : 1

      }

    new_question_path = os.path.join(os.path.dirname(__file__), 'new_question.html')
    new_question_html = template.render(new_question_path, template_values)

    template_values['new_question_html'] = new_question_html
    
    path = os.path.join(os.path.dirname(__file__), 'index.html')
    self.response.out.write(template.render(path, template_values))


application = webapp.WSGIApplication(
                                     [('/', MainPage)],
                                     debug=True)

def main():
  run_wsgi_app(application)

if __name__ == "__main__":
  main()

