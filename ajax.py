from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app
import simplejson as json
#import re
import time

import facebook
import mystorage


_FbApiKey = "bd0a060ac2cb439c65254a9152344289"

_FbSecret = "d1e7cf7973bfd6cba048adeb8b811dc9"

_canvas_url = "http://apps.facebook.com/askthecrowd"
_app_name = "Ask the Crowd"


class MainPage(webapp.RequestHandler):

    def handle_new_question( self ):

        error = ""

        # validate fields here
        question_text = self.request.get('question')
        if question_text == "":
            error = '* ' + _("Question cannot be empty.");

        ideas_last_date = self.request.get('ideas_last_date')

        ideas_last_date = ideas_last_date.replace('/',' ')

        try:
            result = time.strptime( ideas_last_date, '%m %d %Y' )
        except:
            error = error + ' * ' + _("Invalid date field, please correct.")

        participants = self.request.get('participants')

        participant_emails = participants.split(',')

        if( len( participant_emails ) == 1 and participant_emails[0] == '' ):
            error = error + ' * ' + _("Participants field must not be empty.")

        if( len( participant_emails ) > 10 ):
            error = error + ' * ' + _("Participant email addresses must be 10 or less.")

#        import sys
#        print >> sys.stderr, 'LEN: ' + str( len( participant_emails ) )

        extended_desc = self.request.get('extended_desc')
        closing_date = self.request.get('closing_date')
        voting_from_beginning = self.request.get('voting_from_beginning')
        show_who_votes_for_what = self.request.get('show_who_votes_for_what')
        use_aliases = self.request.get('use_aliases')
        reveal_alias_identities = self.request.get('reveal_alias_identities')
        question_prize = self.request.get('question_prize')
        users_can_invite_others = self.request.get('users_can_invite_others')
        public_view = self.request.get('public_view')
        public_participation = self.request.get('public_participation')

        if error:
            result = json.dumps( { 'error' : error } )
        else:
            # create question here
            result = json.dumps( { 'error' : 0 } )

            question = mystorage.Question();

#            question.owner = ???

            question.question_text = question_text
            question.ideas_last_date = ideas_last_date
            question.extended_desc = extended_desc
            question.closing_date = closing_date
            question.voting_from_beginning = voting_from_beginning
            question.show_who_votes_for_what = show_who_votes_for_what
            question.use_aliases = use_aliases
            question.reveal_alias_identities = reveal_alias_identities
            question.question_prize = question_prize
            question.users_can_invite_others = users_can_invite_others
            question.public_view = public_view
            question.public_participation = public_participation
# XXX, enable
#            question.put()

        return result

    def post(self):
        self.response.headers['Content-Type'] = 'text/plain'

        form_name = self.request.get('form_name')

        if form_name == "new_question":
            result = self.handle_new_question()
                
        self.response.out.write(result)

    def get(self):

        action = self.request.get('action')
        fbuid = self.request.get('fbuid')

        if action == "init_tabs":

            ongoing_html = ' REPLACE '
            completed_html = ' REPLACE '
            settings_html = ' REPLACE '
            error_msg = ''
            debug_msg = ''
            
            fbapi = facebook.Facebook( _FbApiKey, _FbSecret ) 

            if fbapi.check_connect_session(self.request):

                user_info = fbapi.users.getInfo( 
                    [fbapi.uid], 
                    ['uid', 'name', 'birthday'])[0]

                fooXXX = 'req: ' + str(self.request) + '---------VALID' + 'name' + user_info['name'] + 'birthday' + user_info['birthday'] + 'uid' + str(user_info['uid'])

                # look up user
                user_query = mystorage.User.gql('where fb_id = :1', user_info["uid"] );
                user = user_query.get()

                if user:
                    debug_msg = 'User is there already, deleting to test adding...'
                    user.delete()
                    
                else:
                    user = mystorage.User();
                    user.fb_id = user_info["uid"]
                    #XXX, we may not want to store the name, because every time
                    #it comes with the FB data
                    user.name = user_info["name"]
                    user.put()

                    debug_msg = 'User was not there, added...'

            else:
                # this should not happen, but if it does, we should warn the user
                fooXXX = 'req: ' + str(self.request) + '---------INVALID!!!!'
                error_msg = _("Could not recognize you as a facebook user, please report problem")

# XXX: I AM HERE! (what's in the request???, why is validate failing???)

#        arguments = gminifb.validate(_FbSecret, self.request)
#        session_key = arguments["session_key"]
#        uid = arguments["user"]

            self.response.headers['Content-Type'] = 'text/plain'

            seri = json.dumps( { 'ongoing_html' : ongoing_html + fooXXX, 'completed_html' : completed_html, 'settings_html' : settings_html, 'error_msg' : error_msg, 'debug_msg' : debug_msg } )

            self.response.out.write(seri)





application = webapp.WSGIApplication(
                                     [('/ajax.html', MainPage)],
                                     debug=True)

def main():
    run_wsgi_app(application)

if __name__ == "__main__":
    main()
