USE_I18N = True

from django.utils.translation import ugettext as _

# Valid languages
LANGUAGES = (
    # 'en', 'es' should match the directories in conf/locale/*
    ('en', _('English')),
    ('es', _('Spanish')),
    )

# This is a default language
LANGUAGE_CODE = 'es'

DEBUG = True
TEMPLATE_DEBUG = DEBUG

#IN_DEV_SERVER = True
IN_DEV_SERVER = True
IN_DEV_SERVER_OLD = False

FACEBOOK_APP_ID = '331909936825023'

FACEBOOK_APP_SECRET = '9f17f1ecae197ca6bf22d809442be538'

# Canvas Page name.
FACEBOOK_CANVAS_NAME = 'five_votes'

