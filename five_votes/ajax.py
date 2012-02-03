import os
import re
import sys

#import pprint

os.environ['DJANGO_SETTINGS_MODULE'] = 'conf.settings'
from google.appengine.dist import use_library
use_library('django', '1.2')

# Force Django to reload settings
from django.conf import settings
settings._target = None

from google.appengine.api import mail

from BeautifulSoup import BeautifulSoup, Comment

from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app

from google.appengine.ext import db
from google.appengine.api import urlfetch
from google.appengine.ext.webapp import template

template.register_template_library('templatetags.myfilters')

#from django.utils.html import truncatewords
#from django.utils.text import truncate_words

from util import I18NRequestHandler

from django.utils.translation import ugettext as _

import simplejson as json
#import re
import time

import facebook
import mystorage

# ZAP!
import tab_content

_FbApiKey = "c2015d9b196956aea3eda25950b90058"
_FbSecret = "e54dffeadd2eb85fd6d68e34559db78a"

# link to user's profile
# http://www.facebook.com/people/@/<FB_ID>


print >> sys.stderr, 'XXX IN_DEV_SERVER: ' + str(settings.IN_DEV_SERVER)


def truncate( value ):
    # be safe
    return value[0:25000]

#
# http://djangosnippets.org/snippets/1655/
#
def sanitize_html(value):
    valid_tags = 'em strong h1 h2 h3'.split()
    valid_attrs = 'href src'.split()
    soup = BeautifulSoup(value)
    for comment in soup.findAll(
        text=lambda text: isinstance(text, Comment)):
        comment.extract()
    for tag in soup.findAll(True):
        if tag.name not in valid_tags:
            tag.hidden = True
        tag.attrs = [(attr, val) for attr, val in tag.attrs
                     if attr in valid_attrs]        
    return truncate(soup.renderContents().decode('utf8').replace('javascript:', ''))


class MainPage(I18NRequestHandler):

    def get_tab_content( self ):

        letter_key = self.request.get('letter_key')

        user_letter = mystorage.Letter.get( letter_key );

        if( user_letter ): 
            tc = tab_content.TabContent( self )
            template_values = tc.template_values( self, user_letter )

            tab_path = os.path.join(os.path.dirname(__file__), 'tab_content.html')
            tab_c = template.render( tab_path, template_values )
            return { 'tab_c' : tab_c }

        return { 'tab_c' : _("not found") }

    
    def get_edit_match_html( self, match_key ):

        my_match = mystorage.Match.get( match_key );

        error = ''

        if( my_match ): 
#
# XXX, add this check back when ideas are in the database
#
#            if my_letter.num_signers > 0:
#                error = '* ' + _("Letter cannot be edited anymore, because it has been signed.")
#                return { 'error' : error }
#


            file_path = os.path.join(os.path.dirname(__file__), 'edit_match.html')

            eee = template.render( file_path,
                                   { 'match' : my_match,
                                     'match_key' : str(my_match.key())
                                     }
                                   )

            return { 'edit_match_html' : eee,
                     }

        return { 'error' : _("cannot find match; should not happen") }


    def get_add_trans_html( self, letter_key ):

        my_letter = mystorage.Letter.get( letter_key );

        error = ''

        if( my_letter ): 

            file_path = os.path.join(os.path.dirname(__file__), 'letter_trans.html')

            eee = template.render( file_path,
                                   {
                                     'src_letter_title' : my_letter.title,
                                     'letter_owner_fb_id' : my_letter.owner.fb_id,
                                     'letter_title' : '',
                                     'letter_summary' : '',
                                     'letter_body' : '',
                                     'action' : 'save_add_trans',
                                     'language_' + my_letter.lang : 1,
                                     'letter_key' : str(my_letter.key())
                                     }
                                   )

            return { 'letter_trans_html' : eee,
                     }

        return { 'error' : _("cannot find letter; should not happen") }


    def get_manage_sigs_html( self, letter_key ):

        import sys
        print >> sys.stderr, 'at get_manage_sigs_html' + str(letter_key)

        letter = mystorage.Letter.get( letter_key );

        letter_sigs = []
#
        import sys
        print >> sys.stderr, 'ARE THERE ANY SIGNERS? '

        signers_q = letter.signer_set
        signers = sorted( signers_q.fetch(1000), key=lambda signer:signer.name )
        for sig in signers:

            print >> sys.stderr, str(sig.key())

            letter_sigs.append( { 'name': sig.name,
                                  'location' : sig.location,
                                  'comment' : sig.comment,
                                  'pic_url' : sig.pic_url,
                                  'sig_key' : str(sig.key())
                                  })

        if( letter_sigs ): 

            print >> sys.stderr, "Rendering................"
            file_path = os.path.join(os.path.dirname(__file__), 'manage_sigs.html')

            xxx = template.render( file_path,
                                    { 'letter' : letter,
                                      'letter_key' : str(letter.key()),
                                      'letter_sigs' : letter_sigs
                                      } )

            print >> sys.stderr, xxx

            return template.render( file_path,
                                    { 'letter' : letter,
                                      'letter_key' : str(letter.key()),
                                      'letter_sigs' : letter_sigs
                                      } )

        import sys
        print >> sys.stderr, 'NO, NO SIGNERS...................... '

        return None

#
#        if( my_letter ): 
#
#            print >> sys.stderr, 'FOUND LETTER!!!!'
#
#            file_path = os.path.join(os.path.dirname(__file__), 'manage_sigs.html')
#
#            xxx = template.render( file_path,
#                                   { 'letter' : my_letter,
#                                     'letter_key' : str(my_letter.key())
#                                     }
#                                   )
#
#            print >> sys.stderr, xxx
#                    
#            return template.render( file_path,
#                                    { 'letter' : my_letter,
#                                      'letter_key' : str(my_letter.key())
#                                      }
#                                    )
#
#
#        return ""
#



    def get_user_letter_links( self ):

        user = self.get_user()

        user_letters = []
#
        import sys
        print >> sys.stderr, 'ARE THERE ANY LETTERS? '
        for letter in user.letter_set:

            print >> sys.stderr, str(letter.key())
            foo_letter = mystorage.Letter.get( letter.key() );
            print >> sys.stderr, str(foo_letter.url_name)

            ltrans = mystorage.LetterTranslation.gql( 'where letter = :1', letter.key() )

            num_trans = ltrans.count()

            ttt = ''
            if num_trans > 0:
                ttt = ltrans.fetch(50)

            user_letters.append( { 'title': letter.title,
                                   'url_name' : letter.url_name,
                                   'letter_key' : str( letter.key() ),
                                   'num_trans' : num_trans,
                                   'letter_trans' : ttt,
                                   })

        if( user_letters ): 
            file_path = os.path.join(os.path.dirname(__file__), 'letter_links.html')
            return template.render( file_path, { 'user_letters' : user_letters } )

        return ""



    def load_user_stuff( self ):
        result_struct = { 'all_is_fine' : 1 }
        return result_struct

    def rate_idea( self ):
        user = self.get_user()
        match_key = self.request.get('match_key')
        match = mystorage.Match.get( match_key )
        idea_key = self.request.get('idea_key')
        idea = mystorage.Idea.get( idea_key )
        rating_value = self.request.get('rating_value')
        vote_val = self.request.get('vote_val') or 0

        vote_query = mystorage.Vote.gql( 'where match = :1 AND owner = :2 and idea = :3', match , user, idea )

        my_vote = vote_query.get()

        import sys
        print >> sys.stderr, 'idea for rating:' + str(idea.title) + ' -- ' + str(vote_val)

        votes_left = 0
        all_my_voted_ideas = mystorage.Vote.gql( 'where match = :1 AND owner = :2 AND num_votes>0', match , user )
        votes_cast=0
        for vote in all_my_voted_ideas:
            votes_cast += vote.num_votes

        import sys
        print >> sys.stderr, 'VOTES cast :' + str(votes_cast)

        votes_left = 10 - votes_cast

        if my_vote:
            print >> sys.stderr, 'Vote there, update the count'

            if vote_val == "1":
                if votes_cast < 10:
                    my_vote.num_votes = my_vote.num_votes + 1
                    my_vote.put()
                    votes_left = votes_left-1
            elif vote_val == "-1":
                new_count = my_vote.num_votes - 1
                if new_count >= 0:
                    my_vote.num_votes = new_count
                    my_vote.put()
                    votes_left = votes_left + 1
        else:
            print >> sys.stderr, 'Vote not there '
            my_vote = mystorage.Vote();
            my_vote.match = match
            my_vote.idea = idea
            my_vote.owner = user
            my_vote.rating = rating_value
            my_vote.num_votes = 0

        my_vote.put()

        result_struct = { 'all_is_fine': 1, 'new_count' : my_vote.num_votes, 'rating_value': my_vote.rating, 'votes_left' : votes_left  }
        return result_struct



    def get_ideas_for_rating( self ):
        user = self.get_user()
        match_key = self.request.get('match_key')
        match = mystorage.Match.get( match_key )

        # XXX, note that this gives MINE, must swap (add !=)
        all_ideas_not_mine = mystorage.Idea.gql( 'where match = :1 AND owner = :2', match , user )

        all_my_votes = mystorage.Vote.gql( 'where match = :1 AND owner = :2', match , user )

        import sys
        print >> sys.stderr, 'ARE THERE ANY VOTES? '
        votes = {}
        for vote in all_my_votes:
            votes[vote.idea.key()] = 1;
            print >> sys.stderr, 'Voted for:' + str(vote.idea.title)

        import sys
        print >> sys.stderr, 'ARE THERE ANY IDEAS? '
        unvoted_ideas = []
        for idea in all_ideas_not_mine:
            if idea.key() not in votes:
                unvoted_ideas.append( {
                    'key': str(idea.key()),
                    'title': str(idea.title),
                    'desc': str(idea.desc),
                    'extended_desc': str(idea.extended_desc),
                    } )
                print >> sys.stderr, 'idea not voted:' + str(idea.title)
            else:
                print >> sys.stderr, 'idea YES voted:' + str(idea.title)

        result_struct = { 'all_is_fine': 1, 'unvoted_ideas' : unvoted_ideas }
        return result_struct


    def get_all_ideas( self ):
        user = self.get_user()
        match_key = self.request.get('match_key')
        match = mystorage.Match.get( match_key )

        match_ideas = mystorage.Idea.gql( 'where match = :1', match )

#        pprint.pprint( match_ideas )

        my_great_votes = mystorage.Vote.gql( 'where match = :1 AND owner = :2 AND rating=:3', match, user, "great" )
        great_ideas = []
        great_ideas_hash = {}
        for vote in my_great_votes:
            great_ideas_hash[ str(vote.idea.key()) ] = vote.idea
            great_ideas.append( {
                'key': str(vote.idea.key()),
                'title': str(vote.idea.title),
                'desc': str(vote.idea.desc),
                'extended_desc': str(vote.idea.extended_desc),
                'num': str(vote.idea.num),
                'num_votes': str(vote.num_votes),
                } )

        my_good_votes = mystorage.Vote.gql( 'where match = :1 AND owner = :2 AND rating=:3', match, user, "good" )
        good_ideas = []
        good_ideas_hash = {}
        for vote in my_good_votes:
            good_ideas_hash[ str(vote.idea.key()) ] = vote.idea
            good_ideas.append( {
                'key': str(vote.idea.key()),
                'title': str(vote.idea.title),
                'desc': str(vote.idea.desc),
                'extended_desc': str(vote.idea.extended_desc),
                'num': str(vote.idea.num),
                'num_votes': str(vote.num_votes),
                } )



        my_bad_votes = mystorage.Vote.gql( 'where match = :1 AND owner = :2 AND rating=:3', match, user, "bad" )
        bad_ideas = []
        bad_ideas_hash = {}
        for vote in my_bad_votes:
            bad_ideas_hash[ str(vote.idea.key()) ] = vote.idea
            bad_ideas.append( {
                'key': str(vote.idea.key()),
                'title': str(vote.idea.title),
                'desc': str(vote.idea.desc),
                'extended_desc': str(vote.idea.extended_desc),
                'num': str(vote.idea.num),
                'num_votes': str(vote.num_votes),
                } )

        # identify not-rated ideas, add to result
        not_rated_ideas = []
        for idea in match_ideas:
            print >> sys.stderr, 'great' + str( great_ideas_hash.has_key( str( idea.key() ) ) )
            print >> sys.stderr, 'good' + str( good_ideas_hash.has_key( str( idea.key() ) ) )
            print >> sys.stderr, 'bad' + str( bad_ideas_hash.has_key( str( idea.key() ) ) )

            if not great_ideas_hash.has_key( str( idea.key() ) ) and not  good_ideas_hash.has_key( str( idea.key() ) ) and not bad_ideas_hash.has_key( str( idea.key() ) ):
                not_rated_ideas.append( {
                        'key': str(idea.key()),
                        'title': str(idea.title),
                        'desc': str(idea.desc),
                        'extended_desc': str(idea.extended_desc),
                        'num': str(idea.num),
                        'num_votes': 0,
                        } )

        result_struct = { 'all_is_fine': 1,
                          'great_ideas' : great_ideas,
                          'good_ideas' : good_ideas,
                          'bad_ideas' : bad_ideas,
                          'not_rated_ideas' : not_rated_ideas,
                          }

        return result_struct



    def get_ideas_for_voting( self ):

#        import sys
#        print >> sys.stderr, 'ARE THERE ANY VOTES? '

        user = self.get_user()
        match_key = self.request.get('match_key')
        match = mystorage.Match.get( match_key )

        all_my_voted_ideas = mystorage.Vote.gql( 'where match = :1 AND owner = :2 AND num_votes>0', match , user )

        all_my_great_ideas = mystorage.Vote.gql( 'where match = :1 AND owner = :2 AND rating=:3', match , user, "great" )

        # deduplicate
        
        all_ideas = {}
        for vote in all_my_great_ideas:
            all_ideas[ str(vote.idea.key()) ] = vote
        for vote in all_my_voted_ideas:
            all_ideas[ str(vote.idea.key()) ] = vote

        votes_cast = 0
        votes = {}
        great_ideas = []
        for vote_key in all_ideas.keys():
            vote = all_ideas[vote_key]
            votes_cast += vote.num_votes
            great_ideas.append( {
                'key': str(vote.idea.key()),
                'title': str(vote.idea.title),
                'desc': str(vote.idea.desc),
                'extended_desc': str(vote.idea.extended_desc),
                'num': str(vote.idea.num),
                'num_votes': str(vote.num_votes),
                } )
#            print >> sys.stderr, 'great or voted idea:' + str(vote.idea.title) + ' num votes: ' + str(vote.num_votes)

        votes_left = 10 - votes_cast

        result_struct = { 'all_is_fine': 1,
                          'ideas_for_voting' : great_ideas,
                          'votes_left' : votes_left,
                          }
        return result_struct

    def get_user_match_links( self ):

        user = self.get_user()

        user_matches = []
#
        for match in user.match_set:
            user_matches.append( { 'title': match.title,
                                   'url_name' : match.url_name,
                                   'match_key' : str( match.key() )
                                   })

        if( user_matches ): 
            file_path = os.path.join(os.path.dirname(__file__), 'match_links.html')
            return template.render( file_path, { 'user_matches' : user_matches } )

        return ""


    def create_user( self ):
        if not settings.IN_DEV_SERVER:
            fbapi = facebook.Facebook( _FbApiKey, _FbSecret )
            if fbapi.check_connect_session(self.request):
                user_info = fbapi.users.getInfo( 
                    [fbapi.uid], 
                    ['uid', 'name', 'birthday', 'pic_square_with_logo', 'current_location'])[0]
            else:
                # this should not happen, but if it does, we should warn the user
                return ""
        else:
            user_info = {
                'uid' : 1234,
                'birthday': "SOME BIRTHDAY",
                'name' : "Claudio Garcia",
                'pic_square_with_logo' : "http://profile.ak.fbcdn.net/v230/37/86/q518261219_2697.jpg",
                'current_location' : {'city': 'Monterrey', 'state': 'N.L.', 'country': 'Mexico' }
                }


        usr = mystorage.User()

        usr.fb_id = user_info["uid"]
        usr.name = user_info['name']

        usr.put()

        return usr


    def get_user( self ):

        fb_uid = self.request.get('fb_uid')

        # XXX raise exception here!
        # (see http://docs.python.org/tutorial/errors.html)
        if( re.match( "\d+$", str(fb_uid) ) is None ):
            error = '* ' + _("Invalid FB UID, should not happen.")

        user_query = mystorage.User.gql('where fb_id = :1', int(fb_uid) )
        user = user_query.get()

        if not user:
           user = self.create_user() 

        return user


    def handle_new_match( self ):
        error = ""        
        user = self.get_user()

        if not user:
            error = '* ' + _("Could not create user, should not happen.")
            return { 'error' : error }
        
        import sys
        print >> sys.stderr, 'IN HANDLE USER: ' + str(user)

        match_title = sanitize_html( self.request.get('match_title') )
        if match_title == "":
            error = '* ' + _("Match title cannot be empty.");

        match_url_name = self.request.get('match_url_name')

        match_url_name = match_url_name[0:39]

        if match_url_name == "":
            error = error + '* ' + _("Match URL name cannot be empty. ");

        if not re.match( "^[_a-z\d]+$", match_url_name):
            error = error + '* ' + _("Match URL name should only contain lowercase a to z letters and underscores.");

        # is the url_name already in use?
        qq = mystorage.Match.gql( 'where url_name = :1', str(match_url_name) )
        existing_match = qq.count()
        
        if existing_match or match_url_name=="image" or match_url_name=="images" or match_url_name=="public_list":
            error = error + '* ' + _( "The URL name " ) + "\"" + match_url_name + "\"" + _(" already exists. Please choose a different one." );
        
        match_desc = sanitize_html( self.request.get('match_desc') )
        if match_desc == "":
            error = error + '* ' + _("Match description cannot be empty. ");

        if len(match_desc) > 240:
            error = error + '* ' + _("Match description should be shorter than 240 characters.")

        match_extended_desc = sanitize_html( self.request.get('match_extended_desc') )
        if match_extended_desc == "":
            error = error + '* ' + _("Extended description cannot be empty. ");

        if len(match_extended_desc) > (30000):
            error = error + '* ' + _("Extended description should be shorter than 30000 characters.")

        if error:
            result = { 'error' : error }
        else:
            match = mystorage.Match();
            match.owner = user
            match.title = match_title
            match.url_name = match_url_name
            match.desc = match_desc
            match.extended_desc = match_extended_desc
            match.num_ideas = 0
            match.put()

            # get user matchs html

            user_matches = self.get_user_match_links();

            # create question here
            result = { 'error' : 0,
                       'url_name' : match_url_name,
                       'title' : match_title,
                       'user_matches' : user_matches,
                       'match_key' : str(match.key())
                       }

        return result


    def handle_edit_match( self ):
        error = ""
        
        user = self.get_user()

        if not user:
            error = '* ' + _("Could not retrieve user, should not happen.")
            return { 'error' : error }

        match_key = self.request.get('match_key')

        existing_match = mystorage.Match.get( match_key );
        if not existing_match:
            error = '* ' + _("Could not retrieve existing match, should not happen.")
            return { 'error' : error }
        
#
# XXX, add this check back when ideas are in the database
#
#        if existing_match.num_signers > 0:
#            error = '* ' + _("Letter cannot be edited anymore, because it has been signed.")
#            return { 'error' : error }
#
        match_title = sanitize_html( self.request.get('match_title') )
        if match_title == "":
            error = '* ' + _("Match title cannot be empty. ");

        match_url_name = self.request.get('match_url_name')
        if match_url_name == "":
            error = error + '* ' + _("Match URL name cannot be empty. ");

        if not re.match( "^[_a-z\d]+$", match_url_name):
            error = error + '* ' + _("Match URL name should only contain lowercase a to z letters and underscores.");

        match_desc = sanitize_html( self.request.get('match_desc') )
        if match_desc == "":
            error = error + '* ' + _("Match description cannot be empty. ");

        if len(match_desc) > 240:
            error = error + '* ' + _("Match description should be shorter than 240 characters.")

        match_extended_desc = sanitize_html( self.request.get('match_extended_desc') )
        if match_extended_desc == "":
            error = error + '* ' + _("Extended description cannot be empty. ");

        if len(match_extended_desc) > (30000):
            error = error + '* ' + _("Match extended description should be shorter than 30000 characters.")

        # is the url_name already in use in other letters?
        qq = mystorage.Match.gql( 'where url_name = :1 AND url_name != :2', str(match_url_name), str( existing_match.url_name ) )
        duplicate_url_name = qq.count()

        if duplicate_url_name or match_url_name=="image" or match_url_name=="images" or match_url_name=="public_list":
            error = error + '* ' + _( "The URL name " ) + "\"" + match_url_name + "\"" + _(" already exists. Please choose a different one." );

        if error:
            result = { 'error' : error }
        else:
            existing_match.title = match_title
            existing_match.url_name = match_url_name
            existing_match.desc = match_desc
            existing_match.extended_desc = match_extended_desc
            existing_match.put()

            # get user matches html

            user_matches = self.get_user_match_links();

            # create question here
            result = { 'error' : 0,
                       'url_name' : match_url_name,
                       'title' : match_title,
                       'user_matches' : user_matches                       
                       }
        return result



    def handle_save_idea( self ):
        error = ""        
        user = self.get_user()

        if not user:
            error = '* ' + _("Could not create user, should not happen.")
            return { 'error' : error }
        
        import sys
        print >> sys.stderr, 'IN HANDLE USER: ' + str(user)

        match_key = self.request.get('match_key')

        usr_match = mystorage.Match.get( match_key );

        if( usr_match is None ): 
            error_msg = _("Match not found. Should not happen.")
            return { 'error_msg' : error_msg }

        idea_title = sanitize_html( self.request.get('idea_title') )
        if idea_title == "":
            error = '* ' + _("Idea title cannot be empty.");

        idea_desc = sanitize_html( self.request.get('idea_desc') )
        if idea_desc == "":
            error = error + '* ' + _("Idea description cannot be empty. ");

        if len(idea_desc) > 240:
            error = error + '* ' + _("Idea description should be shorter than 240 characters.")

        idea_extended_desc = sanitize_html( self.request.get('idea_extended_desc') )
        if idea_extended_desc == "":
            error = error + '* ' + _("Extended description cannot be empty. ");

        if len(idea_extended_desc) > (30000):
            error = error + '* ' + _("Extended description should be shorter than 30000 characters.")

        if error:
            result = { 'error' : error }
        else:
            idea = mystorage.Idea();
            idea.match = usr_match
            idea.owner = user
            idea.title = idea_title
            idea.desc = idea_desc
            idea.extended_desc = idea_extended_desc
            idea.num = usr_match.num_ideas + 1
            idea.put()

            usr_match.num_ideas = usr_match.num_ideas + 1
            usr_match.put()
            
            # XXX get html for new idea (needed?)
#            user_matches = self.get_user_match_links();

            # create question here
            result = { 'error' : 0,
                       'title' : idea_title,
                       'idea_key' : str(idea.key())
                       }
        return result


    def handle_save_add_trans( self ):

        error = ""
        
        user = self.get_user()

        if not user:
            error = '* ' + _("Could not retrieve user, should not happen.")
            return { 'error' : error }

        letter_key = self.request.get('letter_key')

        existing_letter = mystorage.Letter.get( letter_key );
        if not existing_letter:
            error = '* ' + _("Could not retrieve existing letter, should not happen.")
            return { 'error' : error }

        letter_title = sanitize_html( self.request.get('letter_title') )
        if letter_title == "":
            error = '* ' + _("Letter title cannot be empty. ");

        letter_lang = self.request.get('lang')
        if not re.match( "^[\-a-z]+$", letter_lang):
            error = error + '* ' + _("Strange chars in lang.");


        letter_summary = sanitize_html( self.request.get('letter_summary') )
        if letter_summary == "":
            error = error + '* ' + _("Letter summary cannot be empty. ");

        if len(letter_summary) > 240:
            error = error + '* ' + _("Letter summary should be shorter than 240 characters.")

        letter_text = sanitize_html( self.request.get('letter_text') )
        if letter_text == "":
            error = error + '* ' + _("Letter text cannot be empty. ");

        if len(letter_text) > (30000):
            error = error + '* ' + _("Letter text should be shorter than 30000 characters.")

# XXX, HERE I AM (check not working)!!
#[DONE] XXX add check for existing translation
# XXX also check that trans is not same lang as src. letter

        # is the url_name already in use in other letters?
#        qq = mystorage.LetterTranslation.gql( 'where letter = :1 AND lang = :2', existing_letter,  letter_lang )
#        qq = mystorage.LetterTranslation.gql( 'where lang = :1', "en" )

        qq = mystorage.LetterTranslation.gql( 'where lang = :1 AND letter = :2', letter_lang, existing_letter.key() )

        duplicate_trans = qq.count()

        import sys
        print >> sys.stderr, 'DUPLICATE TRANS: ' + str(duplicate_trans) + ' -- ' + str(letter_key) + ' -- ' + str( letter_lang ) + ' existing lang: ' + str(existing_letter.lang)

        if ( duplicate_trans or ( existing_letter.lang == letter_lang ) ):
            error = error + '* ' + _( "A translation to this language already exists." );

        if error:
            result = { 'error' : error }
        else:
            trans = mystorage.LetterTranslation();
            trans.lang = letter_lang
            trans.letter = existing_letter.key()
            trans.title = letter_title
            trans.summary = letter_summary
            trans.body = letter_text
            trans.put()

            # get user letters html

            user_letters = self.get_user_letter_links();

            # create question here
            result = { 'error' : 0,
                       'title' : letter_title,
                       'user_letters' : user_letters,
                       'letter_key' : str(existing_letter.key())
                       }


        return result


    def send_email( self, signer_name, signer_comment, num_signers_count ):

        body = "\n\n" + signer_name + " has signed the letter at http://save-la-pastora.appspot.com.\n\n"

        body = body + str(num_signers_count) + " persons have signed so far.\n\n"
        if signer_comment:
            body = body + "Signer comment:\n\n" + signer_comment

        mail.send_mail(sender="Save La Pastora <claudio.garcia@stanfordalumni.org>",
              to=["Femsa <claudio.garcia@booking.com>","Heineken <claudio.garcia@stanfordalumni.org>"],
              subject = signer_name + ", new Save La Pastora signer",
              body=body)


    def increment_counter( self, counter_name, on_off ):

        the_counter = mystorage.Counter.get_by_key_name( counter_name )

        if (on_off == 'on'):
            if the_counter is None:
                the_counter = mystorage.Counter(key_name=counter_name)
                the_counter.count = 1
            else:
                the_counter.count += 1

            the_counter.put()
                
        if the_counter is None:
            return 0

        return the_counter.count


    def delete_match( self ):
        error_msg = ''

        match_key = self.request.get('match_key')
        usr_match = mystorage.Match.get( match_key );

        if( usr_match is None ): 
            error_msg = _("Match not found. Should not happen.")
            return { 'error_msg' : error_msg }

#
#        # delete IDEAS when there
#        try:
#            while True:
#                q = usr_letter.signer_set
#                assert q.count()
#                db.delete(q.fetch(500))
#                time.sleep(0.5)
#        except Exception, e:
#            pass
#

        usr_match.delete()

        user_matches = self.get_user_match_links();
        
        return { 'match_deleted' : 1,
                 'user_matches' : user_matches }


    def delete_trans( self ):
        error_msg = ''

        letter_key = self.request.get( 'del_trans_letter_key' )

        usr_letter = mystorage.Letter.get( letter_key )

        trans_lang = self.request.get('del_trans_lang')

        if( usr_letter is None ): 
            error_msg = _("Letter not found. Should not happen.")
            return { 'error_msg' : error_msg }

        qqq = mystorage.LetterTranslation.gql( 'where lang = :1 AND letter = :2', trans_lang, usr_letter.key() )

        letter_trans = qqq.get()

        if( letter_trans is None ): 
            error_msg = _("Translation not found. Should not happen.")
            return { 'error_msg' : error_msg }

        # keep track of how many singers will be deleted
        sss = mystorage.Signer.gql( 'where lang = :1 AND letter = :2', trans_lang, usr_letter.key() )
        num_signers_trans = sss.count()
        
        try:
            while True:
                sss = mystorage.Signer.gql( 'where lang = :1 AND letter = :2', trans_lang, usr_letter.key() )
                assert sss.count()
                db.delete( sss.fetch(500) )
                time.sleep( 0.5 )
        except Exception, e:
            pass

        letter_trans.delete()
        usr_letter.num_signers = usr_letter.num_signers - num_signers_trans
        usr_letter.put()
        
        user_letters = self.get_user_letter_links();
        
        return { 'trans_deleted' : 1,
                 'user_letters' : user_letters }

        
    def handle_new_signer( self ):

        error_msg = ''
        debug_msg = ''

        fb_uid_from_form = self.request.get('fb_uid')
        user_comment = sanitize_html( self.request.get('user_comment') )
        show_in_wall = self.request.get('show_in_wall')
        email_my_comment = self.request.get('email_my_comment')
        letter_key = self.request.get('letter_key')
        letter_lang = self.request.get('letter_lang')

        usr_letter = mystorage.Letter.get( letter_key );

        if( usr_letter is None ): 
            error_msg = _("Letter not found. Should not happen.")
            return { 'error_msg' : error_msg }

        simple_form_submit = int(self.request.get('simple_form_submit'))

        if len(user_comment) > 2000:
            error_msg = _("Comment should be shorter than 2000 characters.")
            return { 'error_msg' : error_msg }

        # validate other fields

        if simple_form_submit==1:

            signer_name = sanitize_html( self.request.get('signer_name') )
            signer_location = sanitize_html( self.request.get('signer_location') )

            if len(signer_name) < 5:
                error_msg = _("Name cannot be empty or shorter than 5 characters.")
                return { 'error_msg' : error_msg }

            if len(signer_location) < 5:
                error_msg = _("City and country cannot be empty or shorter than 5 characters.")
                return { 'error_msg' : error_msg }
        else:

            if not settings.IN_DEV_SERVER:
                fbapi = facebook.Facebook( _FbApiKey, _FbSecret )
                if fbapi.check_connect_session(self.request):
                    user_info = fbapi.users.getInfo( 
                        [fbapi.uid], 
                        ['uid', 'name', 'birthday', 'pic_square_with_logo', 'current_location'])[0]
                else:
                    # this should not happen, but if it does, we should warn the user
                    error_msg = _("Could not recognize you as a facebook user, please report problem")
            else:
                user_info = {
                    'uid' : 1234,
                    'birthday': "SOME BIRTHDAY",
                    'name' : "Claudio Garcia",
                    'pic_square_with_logo' : "http://profile.ak.fbcdn.net/v230/37/86/q518261219_2697.jpg",
                    'current_location' : {'city': 'Monterrey', 'state': 'N.L.', 'country': 'Mexico' }
                    }

            signer_query = mystorage.Signer.gql('where fb_id = :1 AND letter = :2', int(user_info["uid"]), usr_letter.key() )

            signer = signer_query.get()

            if signer:
                # should not happen
                error_msg = _("Signer already there. Should not happen.")
                return { 'error_msg' : error_msg }

        signer = mystorage.Signer()

        if simple_form_submit==1:
            signer.name = signer_name
            signer.location = signer_location
        else:
            signer.fb_id = user_info["uid"]
            signer.name = user_info['name']
            signer.pic_url = user_info['pic_square_with_logo']
            signer.picture = db.Blob(urlfetch.Fetch(signer.pic_url).content)
            if user_info['current_location']:
                signer.location = user_info['current_location']['city'] + ', ' + user_info['current_location']['state'] + ', ' + user_info['current_location']['country']
            else:
                signer.location = ''

        signer.letter = usr_letter
        signer.lang = letter_lang
        signer.comment = user_comment
        signer.put()

        if usr_letter.num_signers is None:
            usr_letter.num_signers = 1
        else:
            usr_letter.num_signers = usr_letter.num_signers + 1
            
        usr_letter.put()

#        debug_msg = 'Signer was not there, added...' + signer.name.encode("utf-8")


        return { 'error_msg'          : error_msg,
                 'debug_msg'          : debug_msg,
                 'num_signers_count'  : usr_letter.num_signers,
                 'name'               : signer.name,
                 'location'           : signer.location,
                 'pic_url'            : signer.pic_url,
                 'comment'            : signer.comment,
                 'simple_form_submit' : simple_form_submit,
                 }


    def post(self):

#
#        if settings.IN_DEV_SERVER:
#            time.sleep(1)
#

        action = self.request.get('action')

        if( action == 'del_match' ):
            result_struct = self.delete_match()

        if( action == 'del_trans' ):
            result_struct = self.delete_trans()

        if( action == 'save_add_trans' ):
            result_struct = self.handle_save_add_trans()

        if( action == 'sign_letter' ):
            result_struct = self.handle_new_signer()

        if action == "save_idea":
            result_struct = self.handle_save_idea()

        if action == "new_match":
            result_struct = self.handle_new_match()

        if action == "save_edit_match":
            result_struct = self.handle_edit_match()

#        import sys
#        print >> sys.stderr, 'DOES IT GET HERE 2?' + str( result_struct )

        self.response.headers['Content-Type'] = 'text/plain'
        seri = json.dumps( result_struct )

#        print >> sys.stderr, 'DOES IT GET HERE 3?' + str( seri )

        self.response.out.write(seri)


    def get(self):
#
#        if settings.IN_DEV_SERVER:
#            time.sleep(1)
#
        action = self.request.get('action')

        result_struct = { 'error' : '1' }

        if( action == 'del_sig' ):
            sig_key = self.request.get('sig_key')
            sss = mystorage.Signer.get( sig_key );
            if( sss ):
                sss.letter.num_signers = sss.letter.num_signers - 1
                sss.letter.put()                
                sss.delete()
                result_struct = { 'sig_deleted' : 1,
                                  }

        if( action == 'manage_sigs_html' ):

            letter_key = self.request.get('letter_key')
            
            mmm = self.get_manage_sigs_html( letter_key );

            if mmm is None:
                result_struct = { 'no_signers_yet' : 1,
                                  }
            else:
                result_struct = { 'manage_sigs_html' : mmm,
                                  }

        if( action == 'edit_match_html' ):

            match_key = self.request.get('match_key')
            
            result_struct = self.get_edit_match_html( match_key );


        if( action == 'add_trans' ):

            letter_key = self.request.get('letter_key')
            
            result_struct = self.get_add_trans_html( letter_key );


        if( action == 'del_match_data' ):
            match_key = self.request.get('match_key')
            my_match = mystorage.Match.get( match_key )
            if( my_match ): 
                result_struct = { 'title' : my_match.title,
#                                  'num_ideas' : my_match.num_ideas,
                                  'num_ideas' : 999,
                                  }

        if( action == 'del_trans_data' ):
            letter_key = self.request.get('letter_key')
            trans_lang = self.request.get('lang')
            my_letter = mystorage.Letter.get( letter_key )
            
            qqq = mystorage.LetterTranslation.gql( 'where lang = :1 AND letter = :2', trans_lang, my_letter.key() )

            letter_trans = qqq.get()

            sss = mystorage.Signer.gql( 'where lang = :1 AND letter = :2', trans_lang, my_letter.key() )

            num_signers = sss.count()
            
            if( letter_trans ): 
                result_struct = { 'title' : letter_trans.title,
                                  'num_signers' : num_signers,
                                  }
            else:
                result_struct = { 'hello' : 'world',
                                  'count' : qqq.count(),
                                  'lang' : str(trans_lang),
                                  'key' : str(letter_key),
                                  }


        if( action == 'load_existing_matches' ):
            user_matches = self.get_user_match_links();
            result_struct = { 'has_matches' : 1,
                              'user_matches' : user_matches }


        if( action == 'load_user_stuff' ):
            result_struct = self.load_user_stuff();

        if( action == 'get_ideas_for_rating' ):
            result_struct = self.get_ideas_for_rating();

        if( action == 'get_ideas_for_voting' ):
            result_struct = self.get_ideas_for_voting();

        if( action == 'get_all_ideas' ):
            result_struct = self.get_all_ideas();

        if( action == 'rate_idea' ):
            result_struct = self.rate_idea();

        if( action == 'check_if_already_signed' ):

            letter_key = self.request.get('letter_key')
            fb_uid = self.request.get('fb_uid')

            my_letter = mystorage.Letter.get( letter_key )            

            signer = None            

            if re.match( "\d+$", fb_uid ):
                signer_query = mystorage.Signer.gql( 'where fb_id = :1 AND letter = :2', int(fb_uid), my_letter.key() )
                signer = signer_query.get()

            if not signer:
                result_struct = { 'already_signed_letter' : 0 }
            else:
                result_struct = { 'already_signed_letter' : 1 }

        if( action == 'tab_content' ):
            result_struct = self.get_tab_content()

#        self.response.headers['Content-Type'] = 'text/plain'
        self.response.headers['Content-Type'] = 'application/json'
        seri = json.dumps( result_struct )
        self.response.out.write(seri)



application = webapp.WSGIApplication(
                                     [('/ajax.html', MainPage)],
                                     debug=True)

def main():
    run_wsgi_app(application)

if __name__ == "__main__":
    main()
