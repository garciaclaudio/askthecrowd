import os,sys, math
import mystorage

os.environ['DJANGO_SETTINGS_MODULE'] = 'conf.settings'
from django.conf import settings
settings._target = None


def linebreaksbr(value):
    "Converts newlines into <br />s"
    return value.replace('\n', '<br />')


class TabContent():

    def __init__(self, req_handler):
        self.req_handler = req_handler
        
    def template_values(self, req_handler, user_match ):

        lang = req_handler.request.get('lang', '')

        try:
            lang = req_handler.request.COOKIES['django_language']
        except:
            lang = req_handler.request.LANGUAGE_CODE

        if lang:
            req_handler.request.COOKIES['django_language'] = lang
            req_handler.reset_language()

        template_values = {
            'match_desc' : linebreaksbr( user_match.desc ),
            'letter' : user_letter,
            'letter_key' : str(user_letter.key()),
            'num_trans' : num_trans,
            'selected_trans' : selected_trans,
            'all_trans' : ttt,
            }

        if( user_letter.num_signers >= user_letter.signatures_before_display ):
            signers_q = user_letter.signer_set
            signers = sorted( signers_q.fetch(1000), key=lambda signer:signer.name )

            column_len = int( math.ceil( float( len(signers) )/ float(3) ) )

            template_values['show_signers'] = 1
            template_values['signers'] = signers
            template_values['signers_1'] = signers[ 0 : column_len ]
            template_values['signers_2'] = signers[ column_len : 2 * column_len ]
            template_values['signers_3'] = signers[ 2 * column_len : 3 * column_len ]
        else:
            template_values['hide_letters'] = 1

#            print >> sys.stderr, 'LETTER!!!' + str(template_values['letter'].body)

        return template_values

