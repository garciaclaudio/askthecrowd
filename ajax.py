from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app
import simplejson as json
#import re
import time

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

        import sys
        print >> sys.stderr, 'LEN: ' + str( len( participant_emails ) )

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
            question.question_text = question_text

            question.put()

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

        self.response.headers['Content-Type'] = 'text/plain'

        seri = json.dumps( { 'ongoing_html' : 'ongoing html', 'completed_html' : 'completed html', 'settings_html' : 'settings html' } )

        self.response.out.write(seri)





application = webapp.WSGIApplication(
                                     [('/ajax.html', MainPage)],
                                     debug=True)

def main():
    run_wsgi_app(application)

if __name__ == "__main__":
    main()
