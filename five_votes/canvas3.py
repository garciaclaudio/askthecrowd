#!/usr/bin/env python
# coding: utf-8
# Copyright 2011 Facebook, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License. You may obtain
# a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.
import os, sys

from google.appengine.api import images

import time

os.environ['DJANGO_SETTINGS_MODULE'] = 'conf.settings'

from google.appengine.dist import use_library
use_library('django', '1.2')

# see https://docs.djangoproject.com/en/1.2/topics/i18n/internationalization/
from django.utils.translation import ugettext as _

# Force Django to reload settings
from django.conf import settings
settings._target = None

from util import I18NRequestHandler

from django.template.defaultfilters import register
from django.utils import simplejson as json
from functools import wraps
from google.appengine.api import urlfetch, taskqueue
from google.appengine.ext import db, webapp
from google.appengine.ext.webapp import util, template
from google.appengine.runtime import DeadlineExceededError
from random import randrange
from uuid import uuid4
from BeautifulSoup import BeautifulSoup, Comment


import Cookie
import base64
import cgi
#import conf
import datetime
import hashlib
import hmac
import logging
import time
import traceback
import urllib

# bring in the lang2name tag
template.register_template_library('templatetags.myfilters')



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

def htmlescape(text):
    """Escape text for use as HTML"""
    return cgi.escape(
        text, True).replace("'", '&#39;').encode('ascii', 'xmlcharrefreplace')


@register.filter(name=u'get_name')
def get_name(dic, index):
    """Django template filter to render name"""
    return dic[index].name


@register.filter(name=u'get_picture')
def get_picture(dic, index):
    """Django template filter to render picture"""
    return dic[index].picture


def select_random(lst, limit):
    """Select a limited set of random non Falsy values from a list"""
    final = []
    size = len(lst)
    while limit and size:
        index = randrange(min(limit, size))
        size = size - 1
        elem = lst[index]
        lst[index] = lst[size]
        if elem:
            limit = limit - 1
            final.append(elem)
    return final

_USER_FIELDS = u'name,email,picture,friends,gender'
class User(db.Model):
    user_id = db.StringProperty(required=True)
    access_token = db.StringProperty(required=True)
    name = db.StringProperty(required=True)
    picture = db.StringProperty(required=True)
    email = db.StringProperty()
    gender = db.StringProperty()
    friends = db.StringListProperty()
    dirty = db.BooleanProperty()

    def refresh_data(self):
        """Refresh this user's data using the Facebook Graph API"""
        me = Facebook().api(u'/me',
            {u'fields': _USER_FIELDS, u'access_token': self.access_token})
        self.dirty = False
        self.name = me[u'name']
        self.email = me.get(u'email')
        self.picture = me[u'picture']
        self.friends = [user[u'id'] for user in me[u'friends'][u'data']]
        return self.put()


class Counter(db.Model):
    count = db.IntegerProperty()

    @staticmethod
    # XXX, make this use transactions
    def get_next_question_id():
        q_counter = Counter.get_by_key_name('question_id')
        if q_counter is None:
          q_counter = Counter(key_name='question_id')
          q_counter.count = 1
        else:
          q_counter.count += 1 
        q_counter.put()
        return q_counter.count


class Question(db.Model):
    user_id = db.StringProperty(required=True)
    created = db.DateTimeProperty(auto_now=True)
    question_text = db.StringProperty()

    @staticmethod
    def find_by_user_ids(question, user_ids, limit=50):
        if user_ids:
            return Question.gql(u'WHERE question = :1 AND user_id IN :2', question, user_ids).fetch(limit)
        else:
            return []

class Answer(db.Model):
    question = db.ReferenceProperty(Question)
    user_id = db.StringProperty(required=True)
    answer_text = db.StringProperty()
    picture = db.BlobProperty()


class Vote(db.Model):
    answer = db.ReferenceProperty(Answer)
    question = db.ReferenceProperty(Question)
    user_id = db.StringProperty(required=True)
    num_votes = db.IntegerProperty()

    @staticmethod
    def find_by_user_ids(user_ids, question, limit=1000):
        if user_ids:
            return Vote.gql(u'WHERE question = :1 AND user_id IN :2', question, user_ids).fetch(limit)
        else:
            return []

class ResultsSummary(db.Model):
    answer = db.ReferenceProperty(Answer)
    question = db.ReferenceProperty(Question)
    male_votes = db.IntegerProperty()
    female_votes = db.IntegerProperty()


class QuestionException(Exception):
    pass


class FacebookApiError(Exception):
    def __init__(self, result):
        self.result = result

    def __str__(self):
        return self.__class__.__name__ + ': ' + json.dumps(self.result)


class Facebook(object):
    """Wraps the Facebook specific logic"""
    def __init__(self, app_id=settings.FACEBOOK_APP_ID,
            app_secret=settings.FACEBOOK_APP_SECRET):
        self.app_id = app_id
        self.app_secret = app_secret
        self.user_id = None
        self.access_token = None
        self.signed_request = {}

    def api(self, path, params=None, method=u'GET', domain=u'graph'):
        """Make API calls"""
        if not params:
            params = {}
        params[u'method'] = method
        if u'access_token' not in params and self.access_token:
            params[u'access_token'] = self.access_token
        result = json.loads(urlfetch.fetch(
            url=u'https://' + domain + u'.facebook.com' + path,
            payload=urllib.urlencode(params),
            method=urlfetch.POST,
            headers={
                u'Content-Type': u'application/x-www-form-urlencoded'})
            .content)
        if isinstance(result, dict) and u'error' in result:
            raise FacebookApiError(result)
        return result

    def load_signed_request(self, signed_request):
        """Load the user state from a signed_request value"""
        try:
            sig, payload = signed_request.split(u'.', 1)
            sig = self.base64_url_decode(sig)
            data = json.loads(self.base64_url_decode(payload))

            expected_sig = hmac.new(
                self.app_secret, msg=payload, digestmod=hashlib.sha256).digest()

            # allow the signed_request to function for upto 1 day
            if sig == expected_sig and \
                    data[u'issued_at'] > (time.time() - 86400):
                self.signed_request = data
                self.user_id = data.get(u'user_id')
                self.access_token = data.get(u'oauth_token')
        except ValueError, ex:
            pass # ignore if can't split on dot

    @property
    def user_cookie(self):
        """Generate a signed_request value based on current state"""
        if not self.user_id:
            return
        payload = self.base64_url_encode(json.dumps({
            u'user_id': self.user_id,
            u'issued_at': str(int(time.time())),
        }))
        sig = self.base64_url_encode(hmac.new(
            self.app_secret, msg=payload, digestmod=hashlib.sha256).digest())
        return sig + '.' + payload

    @staticmethod
    def base64_url_decode(data):
        data = data.encode(u'ascii')
        data += '=' * (4 - (len(data) % 4))
        return base64.urlsafe_b64decode(data)

    @staticmethod
    def base64_url_encode(data):
        return base64.urlsafe_b64encode(data).rstrip('=')


class CsrfException(Exception):
    pass


class BaseHandler(I18NRequestHandler):
    facebook = None
    user = None
    csrf_protect = False

    def initialize(self, request, response):
        """General initialization for every request"""

        super(BaseHandler, self).initialize(request, response)

        if settings.IN_DEV_SERVER:
            print >> sys.stderr, '[[[[ IN DEV SERVER ]]]]'
            self.user = User.get_by_key_name('518261219')
            self.csrf_token = '12345'
            self.init_lang()
            return

        try:
            self.init_facebook()
            self.init_csrf()
            self.response.headers[u'P3P'] = u'CP=HONK'  # iframe cookies in IE
            self.init_lang()

        except Exception, ex:
            self.log_exception(ex)
            raise

    def handle_exception(self, ex, debug_mode):
        """Invoked for unhandled exceptions by webapp"""
        self.log_exception(ex)
        self.render(u'error',
            trace=traceback.format_exc(), debug_mode=debug_mode)

    def log_exception(self, ex):
        """Internal logging handler to reduce some App Engine noise in errors"""
        msg = ((str(ex) or ex.__class__.__name__) +
                u': \n' + traceback.format_exc())
        if isinstance(ex, urlfetch.DownloadError) or \
           isinstance(ex, DeadlineExceededError) or \
           isinstance(ex, CsrfException) or \
           isinstance(ex, taskqueue.TransientError):
            logging.warn(msg)
        else:
            logging.error(msg)

    def set_cookie(self, name, value, expires=None):
        """Set a cookie"""
        if value is None:
            value = 'deleted'
            expires = datetime.timedelta(minutes=-50000)
        jar = Cookie.SimpleCookie()
        jar[name] = value
        jar[name]['path'] = u'/'
        if expires:
            if isinstance(expires, datetime.timedelta):
                expires = datetime.datetime.now() + expires
            if isinstance(expires, datetime.datetime):
                expires = expires.strftime('%a, %d %b %Y %H:%M:%S')
            jar[name]['expires'] = expires
        self.response.headers.add_header(*jar.output().split(u': ', 1))

    def render(self, name, **data):
        """Render a template"""
        if not data:
            data = {}
        data[u'js_conf'] = json.dumps({
            u'appId': settings.FACEBOOK_APP_ID,
            u'canvasName': settings.FACEBOOK_CANVAS_NAME,
            u'userIdOnServer': self.user.user_id if self.user else None,
        })
        data[u'logged_in_user'] = self.user
        data[u'message'] = self.get_message()
        data[u'csrf_token'] = self.csrf_token

        data[u'locale'] = self.locale
        data[u'language_' + self.selected_lang] = 1

        data[u'csrf_token'] = self.csrf_token
        data[u'canvas_name'] = settings.FACEBOOK_CANVAS_NAME

        data[u'IN_DEV_SERVER'] = settings.IN_DEV_SERVER
        data[u'NOT_IN_DEV_SERVER'] = not settings.IN_DEV_SERVER

        self.response.out.write(template.render(
            os.path.join(
                os.path.dirname(__file__), 'templates', name + '.html'),
            data))

    def init_facebook(self):
        """Sets up the request specific Facebook and User instance"""
        facebook = Facebook()
        user = None

        # initial facebook request comes in as a POST with a signed_request
        if u'signed_request' in self.request.POST:
            facebook.load_signed_request(self.request.get('signed_request'))
            # we reset the method to GET because a request from facebook with a
            # signed_request uses POST for security reasons, despite it
            # actually being a GET. in webapp causes loss of request.POST data.
            self.request.method = u'GET'
            self.set_cookie(
                'u', facebook.user_cookie, datetime.timedelta(minutes=1440))
        elif 'u' in self.request.cookies:
            facebook.load_signed_request(self.request.cookies.get('u'))

        # try to load or create a user object
        if facebook.user_id:
            print >> sys.stderr, 'getting user: ' + str(facebook.user_id)
            user = User.get_by_key_name(facebook.user_id)
            if user:
                # update stored access_token
                if facebook.access_token and \
                        facebook.access_token != user.access_token:
                    user.access_token = facebook.access_token
                    user.put()
                # refresh data if we failed in doing so after a realtime ping
                if user.dirty:
                    user.refresh_data()
                # restore stored access_token if necessary
                if not facebook.access_token:
                    facebook.access_token = user.access_token

            if not user and facebook.access_token:
                me = facebook.api(u'/me', {u'fields': _USER_FIELDS})
                try:
                    friends = [user[u'id'] for user in me[u'friends'][u'data']]

                    print >> sys.stderr, 'CREATING USER: ' + str(facebook.user_id)
                    print >> sys.stderr, 'NAME: ' + str( me[u'name'].encode('ascii', 'ignore') )
                    print >> sys.stderr, 'GENDER: ' + str( me[u'gender'] )

                    for user in me[u'friends'][u'data']:
                        print >> sys.stderr, '  Friend: ' + str( user[u'id'] ) + ' -' + str( user[u'name'].encode('ascii', 'ignore') )

                    user = User(key_name=facebook.user_id,
                        user_id=facebook.user_id, friends=friends,
                        access_token=facebook.access_token, name=me[u'name'],
                        email=me.get(u'email'), picture=me[u'picture'],
                        gender=me[u'gender'])
                    user.put()
                except KeyError, ex:
                    pass # ignore if can't get the minimum fields

        self.facebook = facebook
        self.user = user

    def init_csrf(self):
        """Issue and handle CSRF token as necessary"""
        self.csrf_token = self.request.cookies.get(u'c')
        if not self.csrf_token:
            self.csrf_token = str(uuid4())[:8]
            self.set_cookie('c', self.csrf_token)
        if self.request.method == u'POST' and self.csrf_protect and \
                self.csrf_token != self.request.POST.get(u'_csrf_token'):
            raise CsrfException(u'Missing or invalid CSRF token.')

    def init_lang(self):
        """language handling"""
        try:
            self.selected_lang = self.request.COOKIES['django_language']
        except:
            self.selected_lang = self.request.LANGUAGE_CODE

        lang = self.request.get('lang')

        print >> sys.stderr, 'LANG PARAM' + str(lang)

        if lang:
            if lang == 'unset':
                del self.request.COOKIES['django_language']
            else:
                self.request.COOKIES['django_language'] = lang
                self.reset_language()
                self.selected_lang = lang

         # needed for the like button
        self.locale = 'en_US'
        if self.selected_lang == 'es':
            self.locale = 'es_ES'

        print >> sys.stderr, 'SELECTED LANG' + str(self.selected_lang)
        print >> sys.stderr, 'DJANGO LANG' + str(self.request.LANGUAGE_CODE)


    def set_message(self, **obj):
        """Simple message support"""
        self.set_cookie('m', base64.b64encode(json.dumps(obj)) if obj else None)

    def get_message(self):
        """Get and clear the current message"""
        message = self.request.cookies.get(u'm')
        if message:
            self.set_message()  # clear the current cookie
            return json.loads(base64.b64decode(message))


def user_required(fn):
    """Decorator to ensure a user is present"""
    @wraps(fn)
    def wrapper(*args, **kwargs):
        handler = args[0]
        if handler.user:
            return fn(*args, **kwargs)
        handler.redirect(u'/')
    return wrapper


class MainPage(BaseHandler):
    """Show recent runs for the user and friends"""
    def get(self):
        self.render(u'index3')

    def post(self):
        self.render(u'index3')


class AjaxHandler(BaseHandler):

    def handle_new_question(self):
        question_text = sanitize_html( self.request.get('question') )
        error = ''
        if question_text == "":
            error = '* ' + _("Question text cannot be empty.");
        if error:
            result = { 'error' : error }
        else:
            new_question_id = Counter.get_next_question_id()
            new_question = Question(
                key_name = str(new_question_id),
                user_id=self.user.user_id,
                question_text = question_text
            )
            new_question.put()
            result = { 'error' : 0,
                       'question_text' : str(question_text),
                       'question_key_name' : str(new_question.key().name())
                       }
        return result

    def handle_new_answer(self):
        answer_text = sanitize_html( self.request.get('answer_text') )
        error = ''
        if answer_text == "":
            error = '* ' + _("Answer text cannot be empty.");

        question = Question.get_by_key_name( self.request.get('question_key_name') );

        if( question is None ): 
            error_msg = _("Question not found. Should not happen.")
            return { 'error_msg' : error_msg }

        if error:
            result = { 'error' : error }
        else:
            new_ans = Answer(
                question=question,
                user_id=self.user.user_id,
                answer_text = answer_text,
            )
            new_ans.save()
            result = { 'error' : 0,
                       'answer_text' : str(answer_text),
                       'answer_key' : str(new_ans.key())
                       }
        return result

    def handle_delete_answer(self):
        error = ''
        ans = Answer.get( self.request.get('answer_key') );

        if( ans is None ): 
            error_msg = _("Answer not found. Should not happen.")
            return { 'error_msg' : error_msg }
        if error:
            result = { 'error' : error }
        else:
            ans.delete()
            result = { 'error' : 0,
                       'deleted_answer_key' : str(self.request.get('answer_key')),
                       }
        return result

    def handle_upload_picture(self):
        error = ''        
#        time.sleep(3)
        ans = Answer.get( self.request.get('answer_key') );

        new_pic =  images.resize(self.request.body, 100, 100)
        ans.picture = db.Blob(new_pic)
        ans.put();

        result =  {'success':'true',
                   'answer_key': self.request.get('answer_key'),
                  }

        return result

    def handle_vote(self):
        answer_key = self.request.get('answer_key')
        ans = Answer.get( answer_key )
        vote_val = self.request.get('vote_val')

        import sys
        print >> sys.stderr, 'in handle_vote, question :' + str(ans.question)
        print >> sys.stderr, 'in handle_vote, user :' + str(self.user)
        print >> sys.stderr, 'in handle_vote, vote_val :' + str(vote_val)
    
        all_my_voted_ideas = Vote.gql( 'where question = :1 AND user_id = :2 AND num_votes>0', ans.question , self.user.user_id )

        votes_cast=0
        for vote in all_my_voted_ideas:
            print >> sys.stderr, 'in handle_vote 2-5:' + str(vote.key())
            votes_cast += int(vote.num_votes)

        print >> sys.stderr, 'in handle_vote 3:' + str(vote_val)

        import sys
        print >> sys.stderr, 'VOTES cast :' + str(votes_cast)

        votes_left = 5 - votes_cast

        vote_query = Vote.gql( 'where user_id = :1 and answer = :2',  self.user.user_id, ans )
        my_vote = vote_query.get()

        results_query = ResultsSummary.gql( 'where question = :1 and answer = :2', ans.question , ans )
        results_summary = results_query.get()

        if results_summary is None:
            results_summary = ResultsSummary( question = ans.question,
                                              answer = ans,
                                              male_votes = 0,
                                              female_votes = 0, )

        if my_vote:
            print >> sys.stderr, 'Vote there, update the count'

            if vote_val == "1":
                if votes_cast < 5:
                    my_vote.num_votes = my_vote.num_votes + 1
                    my_vote.put()
                    votes_left = votes_left-1
                    if self.user.gender == 'male':
                        results_summary.male_votes = results_summary.male_votes + 1
                    else:
                        results_summary.female_votes = results_summary.female_votes + 1
                    results_summary.put()
            elif vote_val == "-1":
                new_count = my_vote.num_votes - 1
                if new_count >= 0:
                    my_vote.num_votes = new_count
                    my_vote.put()
                    votes_left = votes_left + 1
                    if self.user.gender == 'male':
                        results_summary.male_votes = results_summary.male_votes - 1
                    else:
                        results_summary.female_votes = results_summary.female_votes - 1
                    results_summary.put()
        else:
            print >> sys.stderr, 'Vote not there '
            my_vote = Vote(user_id = self.user.user_id)
            my_vote.question = ans.question
            my_vote.answer = ans
            my_vote.num_votes = 1
            my_vote.put()
            if self.user.gender == 'male':
                results_summary.male_votes = results_summary.male_votes + 1
            else:
                results_summary.female_votes = results_summary.female_votes + 1
            results_summary.put()

        result_struct = { 'answer_key': answer_key, 'new_count' : my_vote.num_votes, 'votes_left' : votes_left }
        return result_struct


    def handle_get_results(self):
        friends = {}
        friend_ids = []
        for friend in select_random(
            User.get_by_key_name(self.user.friends), 300):
            friends[friend.user_id] = { 'name' : friend.name, 'user_id' : friend.user_id }
            friend_ids.append( str(friend.user_id) )
            print >> sys.stderr, 'ADDING FRIEND ID: ' + str(friend.user_id)

        question = Question.get_by_key_name( self.request.get('question_key_name') );

        summaries = ResultsSummary.gql( 'where question = :1', question )

        tot_votes = 0
        results = { 'all':[], 'male':[], 'female':[] }

        for summary in summaries:
            tot_votes += (summary.male_votes + summary.female_votes)
            results['all'].append([ str(summary.answer.key()), summary.male_votes + summary.female_votes ])
            results['male'].append([ str(summary.answer.key()), summary.male_votes ])
            results['female'].append([ str(summary.answer.key()), summary.female_votes ])

        friend_votes = Vote.find_by_user_ids( friend_ids, question )

        friends_with_votes = []
        for fv in friend_votes:
            print >> sys.stderr, 'FRIEND VOTE: ' + str(fv.answer.answer_text) + ', ' + str(fv.num_votes)
            if not results.has_key( 'friend_'+str(fv.user_id) ):
                results['friend_'+str(fv.user_id)] = []
                friends_with_votes.append(friends[fv.user_id])

            results['friend_'+str(fv.user_id)].append([ str(fv.answer.key()), fv.num_votes ])

        answers = Answer.gql( 'where question = :1', question )

        ans_struct = []
        ans_hash = {}
        for ans in answers:
            if ans.picture:
                has_pic = 1
            else:
                has_pic = 0
            ans_data = {
                'answer_key' : str(ans.key()),
                'answer_text' : str(ans.answer_text),
                'has_pic' : has_pic,
                'num_votes' : 0,
                }
            ans_struct.append( ans_data )
            ans_hash[ str(ans.key()) ] = ans_data

        sorted_ans = sorted(ans_struct, key=lambda k: k['num_votes'], reverse=True) 

        result_struct = { 'question_key_name': str(question.key().name()),
                          'question_text': str(question.question_text),
                          'answers': sorted_ans,
                          'total_votes': tot_votes,
                          'results': results,
                          'answers_hash':ans_hash,
                          'friends_with_votes':friends_with_votes,
                          }
        return result_struct


    def post(self):
        result_struct = { 'error' : '1' }
        action = self.request.get('action')

        if( action == 'upload_picture' ):
            result_struct = self.handle_upload_picture()

        if( action == 'delete_answer' ):
            result_struct = self.handle_delete_answer()

        if( action == 'vote' ):
            result_struct = self.handle_vote()

        self.response.headers['Content-Type'] = 'application/json'
        seri = json.dumps( result_struct )
        self.response.out.write(seri)


    def get(self):
        result_struct = { 'error' : '1' }

        action = self.request.get('action')

        if( action == 'delete_answer' ):
            result_struct = self.handle_delete_answer()

        if( action == 'create_question' ):
            result_struct = self.handle_new_question()

        if( action == 'add_answer' ):
            result_struct = self.handle_new_answer()

        if( action == 'get_results' ):
            result_struct = self.handle_get_results()

        self.response.headers['Content-Type'] = 'application/json'
        seri = json.dumps( result_struct )
        self.response.out.write(seri)

#
#
# http://localhost:8080/image?answer_key=agpmaXZlLXZvdGVzcg0LEgZBbnN3ZXIYgAEM

class GetImage(BaseHandler):
    def get(self):
        ans = Answer.get( self.request.get('answer_key') );
        if (ans and ans.picture):
            self.response.headers['Content-Type'] = 'image/jpg'
            self.response.out.write(ans.picture)
        else:
            self.error(404)

class QuestionHandler(BaseHandler):
    def get(self, question_key_name):
        print >> sys.stderr, 'in BASE HANDLER, QUESTIONHANDLER GET' + str(question_key_name) + '<<<<----'
        question = Question.get_by_key_name( question_key_name );
        answers = Answer.gql( 'where question = :1', question )

        tot_votes = 0
        votes_count_hash = {}

        user_name = ''
        if self.user:
            user_name = self.user.name
            # obtain votes by this user to this questions
            all_my_voted = Vote.gql( 'where question = :1 AND user_id = :2 AND num_votes>0', question , self.user.user_id )

            tot_votes = 0
            for vote in all_my_voted:
                votes_count_hash[ str(vote.answer.key()) ] = vote.num_votes
                tot_votes += vote.num_votes

        ans_struct = []
        for ans in answers:
            if ans.picture:
                has_pic = 1
            else:
                has_pic = 0

            ans_data = {
                'answer_key' : str(ans.key()),
                'answer_text' : str(ans.answer_text),
                'has_pic' : has_pic,
                }
            if votes_count_hash.has_key( str(ans.key()) ):
                ans_data['num_votes'] = votes_count_hash[ str(ans.key()) ]

            ans_struct.append( ans_data )

        self.render(u'index3',
                    user_name=user_name,
                    question=question,
                    question_key_name=str(question.key().name()),
                    answers=ans_struct,
                    votes_left= 5-tot_votes,
                    )

    def post(self, question_key_name):
        self.render(u'index3')


def main():
    routes = [
        ('/image', GetImage),
        ('/ajax.html', AjaxHandler),
        ('/q(.*)', QuestionHandler),
        (r'/', MainPage),
    ]
    application = webapp.WSGIApplication(routes,
        debug=os.environ.get('SERVER_SOFTWARE', '').startswith('Dev'))
    util.run_wsgi_app(application)


if __name__ == u'__main__':
    main()
