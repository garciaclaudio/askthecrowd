# -*- coding: utf-8 -*-

from google.appengine.ext import webapp

register = webapp.template.create_template_register()


lang_to_name = {'en': 'English', 'es': 'Español'}

def lang2name(value):

    if value is None:
        return 'should not happen'

    return lang_to_name[value]

register.simple_tag( lang2name )

