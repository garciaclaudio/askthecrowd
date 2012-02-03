from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app

import sys
import gminifb
_FbApiKey = "bd0a060ac2cb439c65254a9152344289"
_FbSecret = gminifb.FacebookSecret("d1e7cf7973bfd6cba048adeb8b811dc9")

_canvas_url = "http://apps.facebook.com/askthecrowd"
_app_name = "Ask the Crowd"


class MainPage(webapp.RequestHandler):
  def get(self):

    arguments = gminifb.validate(_FbSecret, self.request)

#    usersInfo = gminifb.call("facebook.users.getInfo",
#              _FbApiKey, _FbSecret, session_key=arguments["session_key"],
#              call_id=True, fields="name,pic_square",
#              uids=arguments["user"]) # uids can be comma separated list

#    name = usersInfo[0]["name"]
#    photo = usersInfo[0]["pic_square"]
#    else:
#        session_key = 
#        uid = arguments["user"]

    self.response.headers['Content-Type'] = 'text/plain'
    self.response.out.write("Hello, webapp World!\n")
    self.response.out.write("REQUEST:\n")
    self.response.out.write(self.request)
    self.response.out.write("\nAND arguments:\n")
    self.response.out.write(arguments)
#    self.response.out.write("\nAND usersInfo:\n")
#    self.response.out.write(usersInfo)


application = webapp.WSGIApplication(
                                     [('/', MainPage)],
                                     debug=True)

def main():
  run_wsgi_app(application)

if __name__ == "__main__":
  main()

